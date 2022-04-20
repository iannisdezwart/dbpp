#pragma once

#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
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
 * Represents an opened file, holding its file descriptor.
 */
struct File
{
	// The file descriptor of the file.
	int fd;

	/**
	 * Initialises a `File` object given an open file descriptor.
	 */
	File(int fd) : fd(fd) {}

	/**
	 * Closes the file.
	 */
	void
	close()
	{
		::close(fd);
	}

	/**
	 * Creates a human readable representation of a `File`.
	 */
	std::string
	to_string()
	const
	{
		char buf[1024];
		snprintf(buf, sizeof(buf), "File @%p { fd = %d }", this, fd);
		return std::string(buf);
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

		return File(fd);
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
		// Seek to the given offset.

		if (lseek(fd, offset, SEEK_SET) == -1)
		{
			// TODO: Handle.

			die_errno("Could not read from offset %lu", offset);
		}

		// Read the given number of bytes.

		ssize_t bytes_read = ::read(fd, buf, size);

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

		if (::write(fd, data, size) == -1)
		{
			// TODO: Handle.

			die_errno("Could not write to EOF");
		}
	}
};

}