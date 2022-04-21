#pragma once

#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "util.hpp"

// Abstract interface for file IO.
// Might be useful for portability concerns in the future.
namespace dbpp::io
{

/**
 * Creates a directory if it does not exist.
 */
inline void
ensure_dir(const std::string &path)
{
	if (mkdir(path.c_str(), 0755) == -1 && errno != EEXIST)
	{
		die_errno("Could not create directory %s", path.c_str());
	}
}

/**
 * Checks if a file exists.
 */
inline bool
file_exists(const std::string &path)
{
	struct stat sb;
	return stat(path.c_str(), &sb) == 0;
}

/**
 * Removes a directory.
 */
inline void
remove_dir(const std::string &path)
{
	if (rmdir(path.c_str()) == -1)
	{
		die_errno("Could not remove directory %s", path.c_str());
	}
}

/**
 * Represents an opened file, holding its file descriptor.
 */
struct File
{
	// A mapping of file descriptors to file names.
	inline static std::unordered_map<int, std::string> fd_to_path;

	// The file descriptor of the file.
	int fd;

	File() {}

	/**
	 * Initialises a `File` object given an open file descriptor.
	 */
	File(int fd) : fd(fd) {}

	/**
	 * Steals a file descriptor from another `File` object.
	 */
	File(File &&other)
		: fd(other.fd)
	{
		other.fd = -1;
	}

	/**
	 * Copies a file descriptor from another `File` object.
	 */
	File(const File &other)
		: fd(other.fd) {}

	/**
	 * Steals a file descriptor from another `File` object.
	 */
	File &
	operator=(File &&other)
	{
		fd = other.fd;
		other.fd = -1;
		return *this;
	}

	/**
	 * Copies a file descriptor from another `File` object.
	 */
	File &
	operator=(const File &other)
	{
		fd = other.fd;
		return *this;
	}

	/**
	 * Converts the `File` object to a human-readable string.
	 */
	friend std::ostream &
	operator<<(std::ostream &os, const File &file)
	{
		os << "File { fd = " << file.fd << " }";
		return os;
	}

	/**
	 * Closes the file.
	 */
	void
	close()
	{
		if (fd != -1)
		{
			::close(fd);
		}
	}

	/**
	 * Opens a file in read/write mode from a given path.
	 *
	 * @returns A `File` object representing the opened file.
	 */
	static File
	open_rw(const std::string &path)
	{
		// Open the file in read/write mode,
		// creating it if it doesn't already exist.

		int fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);

		if (fd == -1)
		{
			die_errno("Could not open file %s", path.c_str());
		}

		// Store the file path.

		fd_to_path[fd] = path;

		return File(fd);
	}

	/**
	 * Creates a read/write temporary file.
	 *
	 * @returns A `File` object representing the opened file.
	 */
	static File
	create_temp()
	{
		char file_name[] = "/tmp/dbpp-XXXXXX";

		// Replace the Xs with random characters.

		for (int i = 10; i < 16; i++)
		{
			int index = rand() % sizeof(temp_file_chars) - 1;
			file_name[i] = temp_file_chars[index];
		}

		// Create the file and open it.

		int fd = open(file_name, O_RDWR | O_CREAT, 0644);

		if (fd == -1)
		{
			die_errno("Could not create temporary file");
		}

		// Store the file path.

		fd_to_path[fd] = file_name;

		return File(fd);
	}

	/**
	 * Checks if the file still exists.
	 */
	bool
	exists()
	{
		return file_exists(fd_to_path[fd]);
	}

	/**
	 * Returns the size of the file.
	 */
	size_t
	size()
	const
	{
		struct stat st;
		if (fstat(fd, &st) == -1)
		{
			die_errno("Could not get file size");
		}

		return st.st_size;
	}

	/**
	 * Returns the path of the file.
	 */
	const std::string &
	get_path()
	{
		return fd_to_path[fd];
	}

	/**
	 * Truncates the file to zero length, effectively clearing it.
	 */
	void
	clear()
	{
		if (ftruncate(fd, 0) == -1)
		{
			die_errno("Could not truncate file");
		}
	}

	/**
	 * Removes the file from disk.
	 */
	void
	remove()
	{
		if (unlink(fd_to_path[fd].c_str()) == -1)
		{
			die_errno("Could not remove file %s",
				fd_to_path[fd].c_str());
		}
	}

	/**
	 * Reads a given number of bytes from the file at a given offset.
	 *
	 * @param offset The offset to read from.
	 * @param size The number of bytes to read.
	 * @param buf The buffer to read into.
	 * @returns The number of bytes read.
	 */
	size_t
	read_at(size_t offset, size_t size, void *buf)
	const
	{
		ssize_t bytes_read = pread(fd, buf, size, offset);

		if (bytes_read == -1)
		{
			// TODO: Handle.

			die_errno("Could not read from offset %lu", offset);
		}

		return bytes_read;
	}

	/**
	 * Reads the entire file into a given buffer.
	 *
	 * @param buf The buffer to read into.
	 */
	void
	read_entire_file(void *buf)
	const
	{
		size_t size = this->size();

		if (::read(fd, buf, size) == -1)
		{
			// TODO: Handle.

			die_errno("Could not read from file");
		}
	}

	/**
	 * Reads a length-prefixed string from the file at a given offset.
	 *
	 * @param offset The offset to read from.
	 * @returns The string read from the file.
	 */
	std::string
	read_string_at(size_t offset)
	const
	{
		// Seek to the given offset.

		if (lseek(fd, offset, SEEK_SET) == -1)
		{
			// TODO: Handle.

			die_errno("Could not seek to offset %lu", offset);
		}

		// Read the length of the string.

		size_t length;

		if (::read(fd, &length, sizeof(size_t)) == -1)
		{
			// TODO: Handle.

			die_errno("Could not read from offset %lu", offset);
		}

		// Read the string.

		std::vector<char> bytes(length);

		if (::read(fd, bytes.data(), length) == -1)
		{
			// TODO: Handle.

			die_errno("Could not read from offset %lu", offset);
		}

		return std::string(bytes.begin(), bytes.end());
	}

	/**
	 * Appends bytes at the end of the file.
	 *
	 * @param data A pointer to the bytes to append.
	 * @param size The number of bytes to append.
	 */
	void
	append(char *data, size_t size)
	{
		// Seek to the end of the file.

		if (lseek(fd, 0, SEEK_END) == -1)
		{
			// TODO: Handle.

			die_errno("Could not seek to EOF");
		}

		// Write the bytes.

		if (write(fd, data, size) == -1)
		{
			// TODO: Handle.

			die_errno("Could not write to EOF");
		}
	}
};

}