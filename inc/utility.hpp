#pragma once

#include <cstdarg>
#include <stdexcept>

// This attribute makes clang check the format string of a printf-like function,
// and issue a warning if it's invalid. The argument should be the 1-based index
// of the format string parameter.
#define CHECK_FORMAT(x) __attribute__((format(printf, x, x + 1)))

void safeClose(int& fd);
void logInfo(const char* format, ...) CHECK_FORMAT(1);
void logWarn(const char* format, ...) CHECK_FORMAT(1);
void logError(const char* format, ...) CHECK_FORMAT(1);
bool matchIgnoreCase(const char* a, const char* b);
char* nextListItem(char*& list);

/**
 * Throw an exception with an error message given using printf-style formatting
 * syntax. Throws std::runtime_error by default; other exception types can be
 * used, for example `throwf<std::logic_error>("message")`.
 */
template <typename ErrorType = std::runtime_error>
CHECK_FORMAT(1)
void throwf(const char* format, ...)
{
	// Get the length of the formatted string.
	va_list args;
	va_start(args, format);
	int length = 1 + vsnprintf(nullptr, 0, format, args);
	va_end(args);

	// Format the string into a temporary buffer.
	std::string message(length, '\0');
	va_start(args, format);
	vsnprintf(message.data(), length, format, args);
	va_end(args);

	// Throw an exception with the formatted error message.
	throw ErrorType(message);
}
