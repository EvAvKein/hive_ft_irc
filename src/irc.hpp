#pragma once

#include <fcntl.h>
#include <map>
#include <netdb.h>
#include <stdexcept>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT_MAX 65535 // The maximum allowed port number.
#define MAX_BACKLOG 20 // Maximum length of the pending connection queue.
#define MAX_MESSAGE_PARTS 15 // Maximum number of parts/params in one message.
#define MAX_EVENTS 10 // The maximum number of events received by epoll.

// ANSI escape codes for nicer terminal output.
#define ANSI_RED	"\x1b[31m"
#define ANSI_GREEN	"\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RESET	"\x1b[0m"

// This attribute makes clang check the format string of a printf-like function,
// and issue a warning if it's invalid. The argument should be the 1-based index
// of the format string parameter.
#define CHECK_FORMAT(x) __attribute__((format(printf, x, x + 1)))

class Client
{
public:
	int socket = -1;			// The socket used for the client's connection
	std::string nick;			// The client's nickname
	std::string user;			// The client's user name
	std::string input;			// Buffered data from recv()
	std::string output;			// Buffered data for send()
	const char *prefix = "";	// Prefix symbol (either "" or "@")

	// Send a string to the client.
	void send(const std::string_view& string);

	// Send a single value of numeric type (using std::to_string).
	template <typename Type>
	void send(const Type& value)
	{
		send(std::string_view(std::to_string(value)));
	}

	// Send multiple values by recursively calling other send() functions.
	template <typename First, typename... Rest>
	void send(const First& first, const Rest&... rest)
	{
		send(first);
		send(rest...);
	}

	// Send multiple values and add a CRLF line break at the end.
	template <typename... Arguments>
	void sendLine(const Arguments&... arguments)
	{
		send(arguments...); // Send all the arguments.
		send("\r\n"); // Add a newline at the end.
	}

	// Types that must be converted to string_view to be sent.
	void send(char character) { send(std::string_view(&character, 1)); }
	void send(char* string) { send(std::string_view(string)); }
	void send(const char* string = "") { send(std::string_view(string)); }
	void send(const std::string& string) { send(std::string_view(string)); }
};

class Channel
{
public:

	char symbol = '=';						// The channel symbol (one of '=', '@', '*')
	std::string name;						// The name of the channel
	std::string topic;						// The current topic
	std::string key;						// Key needed to join the channel
	std::map<std::string, Client*> members;	// Clients joined to this channel

	static bool isValidName(const char* name);
};

class Server
{
public:
	Server(const char* port, const char* password);
	~Server();

	void eventLoop(const char* host, const char* port);

private:
	bool setNonBlocking(int fd);
	void closeAndClean(const std::string& msg, int sockfd, struct addrinfo* result);
	int  createListenSocket(const char* host, const char* port, bool isNonBlocking);

	void receiveFromClient(Client& client);

	void parseMessage(Client& client, std::string message);
	void handleMessage(Client& client, int argc, char** argv);

	// Handlers for specific messages.
	void handleUser(Client& client, int argc, char** argv);
	void handleNick(Client& client, int argc, char** argv);
	void handlePass(Client& client, int argc, char** argv);
	void handleCap(Client& client, int argc, char** argv);
	void handleJoin(Client& client, int argc, char** argv);

	// Find entities by name.
	Client* findClientByName(std::string_view name);
	Channel* findChannelByName(std::string_view name);

	const char* _port = nullptr;
	const char* _password = nullptr;
	int _serverFd = -1;
	int _epollFd = -1;
	std::map<int, Client> _clients;
	std::map<std::string, Channel> _channels;
};

// utility.cpp
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
