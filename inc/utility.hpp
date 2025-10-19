#pragma once

#include <cstdarg>
#include <stdexcept>
#include <string>
#include <string_view>

// ANSI escape codes for nicer terminal output.
#define ANSI_RED	"\x1b[31m"
#define ANSI_GREEN	"\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RESET	"\x1b[0m"

// This attribute makes clang check the format string of a printf-like function,
// and issue a warning if it's invalid. The argument should be the 1-based index
// of the format string parameter.
#define CHECK_FORMAT(x) __attribute__((format(printf, x, x + 1)))

void safeClose(int& fd);
bool matchIgnoreCase(const char* a, const char* b);
char* nextListItem(char*& list);

// Logging functions.
void log(const std::string_view& string);
inline void log(char chr) { log(std::string_view(&chr, 1)); }
inline void log(char* string) { log(std::string_view(string)); }
inline void log(const char* string) { log(std::string_view(string)); }
inline void log(const std::string& string) { log(std::string_view(string)); }

template <typename Type>
void log(const Type& value)
{
	log(std::to_string(value));
}

template <typename First, typename... Rest>
void log(const First& first, const Rest&... rest)
{
	log(first);
	log(rest...);
}

template <typename... Arguments>
void logInfo(const Arguments&... arguments)
{
	log(ANSI_GREEN "[INFO] " ANSI_RESET, arguments..., "\n");
}

template <typename... Arguments>
void logWarn(const Arguments&... arguments)
{
	log(ANSI_YELLOW "[WARN] " ANSI_RESET, arguments..., "\n");
}

template <typename... Arguments>
void logError(const Arguments&... arguments)
{
	log(ANSI_RED "[ERROR] " ANSI_RESET, arguments..., "\n");
}

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
