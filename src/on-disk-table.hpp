#pragma once

#include <string>
#include <type_traits>
#include <unistd.h>
#include <vector>

#include "in-memory-table.hpp"
#include "io.hpp"

namespace dbpp
{

/**
 * Represents an on-disk stored table.
 * Provides methods to access content of the table.
 */
template <typename Record>
struct OnDiskTable
{
	// Number of entries that fit in a block.
	static const constexpr size_t ENTRIES_PER_BLOCK
		= 4096 / sizeof(Record);

	// The size of one full block.
	static const constexpr size_t BLOCK_SIZE
		= ENTRIES_PER_BLOCK * sizeof(Record);

	// The path of the root directory of the table.
	std::string root_path;

	// File that contains the rows of this table.
	io::File rows_file;

	// Buffer that holds pending writes to the rows file.
	Record *rows_write_buffer = new Record[ENTRIES_PER_BLOCK];

	// Current size of the rows write buffer.
	size_t rows_write_buffer_size = 0;

	// Indicates whether this table is temporary.
	// If it is, its open files will be deleted upon destruction.
	bool temp = false;

	OnDiskTable() {}

	/**
	 * Constructs an `OnDiskTable` object from the given files.
	 *
	 * @param root_path The path of the root directory of the table.
	 * @param rows_file The file containing the rows of the table.
	 * @param temp Whether this table is temporary.
	 */
	OnDiskTable(const std::string &root_path, io::File rows_file,
			bool temp = false)
		: root_path(root_path),
		  rows_file(rows_file),
		  temp(temp) {}

	/**
	 * Move constructor.
	 */
	OnDiskTable(OnDiskTable &&other)
		: root_path(other.root_path),
		  rows_file(std::move(other.rows_file)),
		  rows_write_buffer(other.rows_write_buffer),
		  rows_write_buffer_size(other.rows_write_buffer_size),
		  temp(other.temp)
	{
		other.rows_write_buffer = nullptr;
	}

	/**
	 * Destructor.
	 */
	~OnDiskTable()
	{
		// If the file has been stolen, that means the table was
		// `std::move`d and we shouldn't do anything.

		if (rows_file.fd == -1)
		{
			return;
		}

		// Flush pending writes to the rows file.

		if (!temp && rows_write_buffer_size > 0)
		{
			flush_write_buffer();
		}

		// Free the write buffer.

		delete[] rows_write_buffer;

		// If the table is temporary, truncate its files.
		// TODO: Maybe deleting the files is better. How can we do
		// this with only the file descriptor?

		if (temp)
		{
			rows_file.clear();
		}

		// Close the files.

		rows_file.close();
	}

	/**
	 * Converts the table to a human-readable string.
	 */
	friend std::ostream &
	operator<<(std::ostream &os, const OnDiskTable &table)
	{
		os << "OnDiskTable { root_path = " << table.root_path
			<< ", rows = [ ";

		for (const Record &record : table)
		{
			os << record << " ";
		}

		os << std::string("] }");

		return os;
	}

	/**
	 * Opens a table that is stored on disk by its path.
	 */
	static OnDiskTable
	open(const std::string &path)
	{
		/*
		Tables are structured as follows on disk:
		\- `<table_name>/`
		   |* `rows`
		   |* `heap`
		   \- `indexes/`
		      \* `<index_name>`

		The `rows` file contains the rows of the table.

		The `heap` file contains data that is pointed to by columns
		that are variable sized. E.g. strings or blobs.

		The `indexes` directory contains indexes that are used to
		speed up queries.

		`<index_name>` files contain the index data. There can be
		several types of indexes, so there is no generic file format
		that is used for all index files.
		*/

		// Ensure that the table directory exists.

		io::ensure_dir(path);

		// Open all files and construct a `Table` object from them.

		io::File rows_file = io::File::open_rw(path + "/rows");

		return OnDiskTable(path, rows_file);
	}

	/**
	 * Creates a temporary on-disk table.
	 */
	static OnDiskTable
	create_temp()
	{
		// Create a temp file for the rows.

		io::File rows_file = io::File::create_temp();

		return OnDiskTable("dummy", rows_file, true);
	}

	/**
	 * Returns the number of records in this table.
	 */
	size_t
	size()
	const
	{
		return rows_file.size() / sizeof(Record)
			+ rows_write_buffer_size;
	}

	/**
	 * Clears the table, removing all rows.
	 */
	void
	clear()
	{
		rows_file.clear();
		rows_write_buffer_size = 0;
	}

	/**
	 * Removes the table from disk.
	 */
	void
	remove()
	{
		rows_file.remove();
		io::remove_dir(root_path);
	}

	/**
	 * Flushes the write buffer into the rows file.
	 */
	void
	flush_write_buffer()
	{
		// Write the buffer to the rows file.

		rows_file.append((char *) rows_write_buffer,
			rows_write_buffer_size * sizeof(Record));

		// Reset the buffer size.

		rows_write_buffer_size = 0;
	}

	/**
	 * Inserts a new row into the table.
	 */
	void
	insert(const Record &record)
	{
		// Write the record to the rows file buffer.

		memcpy(rows_write_buffer + rows_write_buffer_size,
			&record, sizeof(Record));
		rows_write_buffer_size++;

		// If the write buffer is full, flush it to disk.

		if (rows_write_buffer_size == ENTRIES_PER_BLOCK)
		{
			flush_write_buffer();
		}
	}

	/**
	 * Reads an entry from the table at the given index.
	 */
	Record
	read(size_t index)
	{
		// Check if we need to read from the rows file or the write
		// buffer.

		size_t num_entries = rows_file.size() / sizeof(Record);

		if (index >= num_entries)
		{
			// Read the entry from the write buffer.

			return rows_write_buffer[index - num_entries];
		}

		// Read the entry from the rows file.

		Record record;
		rows_file.read_at(index * sizeof(Record), sizeof(Record), &record);

		return record;
	}

	/**
	 * Filters the table using the given predicate.
	 *
	 * @param filter The filter function to use.
	 * @returns An `InMemoryTable` containing the filtered rows.
	 */
	template <typename Filter>
	InMemoryTable<Record>
	filter_into_memory(Filter filter)
	{
		InMemoryTable<Record> table;

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				table.insert(record);
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 *
	 * @param filter The filter function to use.
	 * @returns An `OnDiskTable` containing the filtered rows.
	 */
	template <typename Filter>
	OnDiskTable<Record>
	filter_into_disk(Filter filter)
	{
		OnDiskTable<Record> table = create_temp();

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				table.insert(record);
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 * Then maps the rows using the given function.
	 *
	 * @param filter The filter function to use.
	 * @param map The map function to use.
	 * @returns An `InMemoryTable` containing the filter-mapped rows.
	 */
	template <typename OutRecord, typename Filter, typename Map>
	InMemoryTable<OutRecord>
	filter_map_into_memory(Filter filter, Map map)
	{
		InMemoryTable<OutRecord> table;

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				// Map the rows using the mapping function.

				table.insert(map(record));
			}
		}

		return table;
	}

	/**
	 * Filters the table using the given predicate.
	 * Then maps the rows using the given function.
	 *
	 * @param filter The filter function to use.
	 * @param map The map function to use.
	 * @returns An `OnDiskTable` containing the filter-mapped rows.
	 */
	template <typename OutRecord, typename Filter, typename Map>
	OnDiskTable<OutRecord>
	filter_map_into_disk(Filter filter, Map map)
	{
		auto table = OnDiskTable<OutRecord>::create_temp();

		// Iterate over all rows in the table.

		for (const Record &record : *this)
		{
			// Filter the rows by the predicate.

			if (filter(record))
			{
				// Map the rows using the mapping function.

				table.insert(map(record));
			}
		}

		return table;
	}

	/**
	 * Reads the entire table into an in-memory table.
	 */
	InMemoryTable<Record>
	read_into_memory()
	{
		// Read the entire rows file into an in-memory table.

		InMemoryTable<Record> table;
		table.rows.resize(rows_file.size() / sizeof(Record));
		rows_file.read_entire_file(table.rows.data());

		return table;
	}

	/**
	 * Iterator that iterates over all records of the table.
	 */
	template <bool IsConst>
	struct IteratorBase
	{
		// The phase of the iterator.
		enum Phase
		{
			// Indicates the iterator is reading from the rows file.
			ROWS_FILE,

			// Indicates the iterator is done reading the entire
			// rows file, and is now reading from the rows buffer.
			ROWS_BUFFER,

			// Indicates the iterator is done reading table.
			END
		}
		phase;

		// Reference to the table.
		const OnDiskTable &table;

		// The current position in the file.
		size_t rows_file_pos;

		// Buffer that holds the current block from the rows file.
		Record *block;

		// The number of records in the current block.
		size_t block_size;

		// The index of the current pointed block.
		size_t block_pos;

		/**
		 * Constructs an iterator that starts a the beginning of the
		 * table.
		 */
		IteratorBase(const OnDiskTable &table)
			: table(table),
			  phase(ROWS_FILE),
			  rows_file_pos(0)
		{
			// Read the first block.

			block = new Record[BLOCK_SIZE];
			read_next_block();
		}

		/**
		 * Constructs an iterator that points to the end of the table.
		 */
		IteratorBase(const OnDiskTable &table, Phase phase)
			: table(table),
			  phase(phase),
			  block(nullptr) {}

		/**
		 * Copy constructor.
		 */
		IteratorBase(const IteratorBase &other)
			: table(other.table),
			  rows_file_pos(other.rows_file_pos),
			  phase(other.phase),
			  block_size(other.block_size),
			  block_pos(other.block_pos)
		{
			// Copy the block.

			block = new Record[BLOCK_SIZE];
			memcpy(block, other.block, block_size);
		}

		/**
		 * Destructor. Frees any existing block.
		 */
		~IteratorBase()
		{
			if (phase == ROWS_FILE)
			{
				delete[] block;
			}
		}

		/**
		 * Reads the next block from the file.
		 */
		void
		read_next_block()
		{
			// Read the next block.

			ssize_t bytes_read = table.rows_file.read_at(
				rows_file_pos, BLOCK_SIZE, (void *) block);

			// Update the block size and position index.

			block_size = bytes_read;
			block_pos = 0;

			if (bytes_read > 0)
			{
				return;
			}

			// We go to the next phase, so we can delete the block.

			delete[] block;
			block = nullptr;

			// If the rows write buffer is empty, we
			// can skip straight to the end phase.

			if (table.rows_write_buffer_size == 0)
			{
				phase = END;
			}

			// Start reading from the rows write buffer.

			else
			{
				block = table.rows_write_buffer;
				block_size = table.rows_write_buffer_size
					* sizeof(Record);
				phase = ROWS_BUFFER;
			}
		}

		/**
		 * Returns a reference to the next record in the table.
		 */
		template <bool T = true>
		typename std::enable_if<T && !IsConst, Record &>::type
		operator*()
		{
			return block[block_pos];
		}

		/**
		 * Returns a read-only reference to the next record in
		 * the table.
		 */
		const Record &
		operator*()
		const
		{
			return block[block_pos];
		}

		/**
		 * Returns a pointer to the next record in the table.
		 */
		Record *
		operator->()
		{
			return block + block_pos;
		}

		/**
		 * Advances the iterator to the next record.
		 */
		IteratorBase &
		operator++()
		{
			block_pos++;

			if (block_pos * sizeof(Record) == block_size)
			{
				switch (phase)
				{
				case ROWS_FILE:
					// Advance to the next block.

					rows_file_pos += block_size;
					read_next_block();
					break;

				case ROWS_BUFFER:
					// We're done.

					phase = END;
					break;

				default:
					break;
				}
			}

			return *this;
		}

		/**
		 * Advances the iterator to the next record.
		 */
		IteratorBase
		operator++(int)
		const
		{
			IteratorBase old = *this;
			operator++();
			return old;
		}

		/**
		 * Returns whether two iterators are equal.
		 */
		bool
		operator==(const IteratorBase &other)
		const
		{
			if (phase != other.phase)
			{
				return false;
			}

			switch (phase)
			{
			case ROWS_FILE:
				return rows_file_pos == other.rows_file_pos
					&& block_pos == other.block_pos;

			case ROWS_BUFFER:
				return block_pos == other.block_pos;

			default:
				return true;
			}
		}

		/**
		 * Returns whether two iterators are unequal.
		 */
		bool
		operator!=(const IteratorBase &other)
		const
		{
			return !operator==(other);
		}

		/**
		 * Returns whether this iterator is less than another iterator.
		 */
		bool
		operator<(const IteratorBase &other)
		const
		{
			if (phase != other.phase)
			{
				return phase < other.phase;
			}

			switch (phase)
			{
			case ROWS_FILE:
				if (rows_file_pos < other.rows_file_pos)
				{
					return true;
				}
				else if (rows_file_pos > other.rows_file_pos)
				{
					return false;
				}
				else
				{
					return block_pos < other.block_pos;
				}

			case ROWS_BUFFER:
				return block_pos < other.block_pos;

			default:
				return false;
			}
		}
	};

	using Iterator = IteratorBase<false>;
	using ConstIterator = IteratorBase<true>;

	/**
	 * Returns an iterator that points to the first record of the table.
	 */
	Iterator
	begin()
	{
		return Iterator(*this);
	}

	/**
	 * Returns an iterator that points to the end of the table.
	 */
	Iterator
	end()
	{
		return Iterator(*this, Iterator::END);
	}

	/**
	 * Returns a read-only iterator that points to the first record of
	 * the table.
	 */
	ConstIterator
	begin()
	const
	{
		return ConstIterator(*this);
	}

	/**
	 * Returns a read-only iterator that points to the end of the table.
	 */
	ConstIterator
	end()
	const
	{
		return ConstIterator(*this, ConstIterator::END);
	}
};

}