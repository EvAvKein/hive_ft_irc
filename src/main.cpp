#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>

#define PORT_MAX 65535

void start(int port, const char* password)
{
	printf("port: %d\n", port);
	printf("password: '%s'\n", password);
}

int main(int argc, char** argv)
{
	// Check that two arguments were given.
	if (argc != 3) {
		printf("usage: ./ircserv <port> <password>\n");
		return EXIT_FAILURE;
	}

	// Convert the port to an integer.
	char* end = nullptr;
	long port = strtol(argv[1], &end, 10);

	// Check that it's in the valid range of port numbers.
	if (errno == ERANGE || port < 0 || port > PORT_MAX) {
		printf("error: port number must be in [0, %d]\n", PORT_MAX);
		return EXIT_FAILURE;
	
	// Check that the whole string was used.
	} else if (strlen(argv[1]) == 0 || end != argv[1] + strlen(argv[1])) {
		printf("error: invalid port number '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}
	start(port, argv[2]);
}
