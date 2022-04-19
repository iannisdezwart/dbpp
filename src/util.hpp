#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <string>

namespace dbpp
{

#define die_errno(fmt, ...) do { \
	fprintf(stderr, "%s:%d: " fmt "\n", \
		__FILE__, __LINE__, ##__VA_ARGS__); \
	fprintf(stderr, "Errno: %s\n", strerror(errno)); \
	abort(); \
} while (false)

// Letters that are allowed in a temporary file name.
const char temp_file_chars[] =
	"abcdefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"0123456789";

}