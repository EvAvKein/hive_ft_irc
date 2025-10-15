#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <string>

#include "irc.h"

void safeClose(int& fd)
{
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

Server::Server(const char* port, const char* password)
    : port(port), password(password)
{
}

void Server::start()
{
    printf("Starting with password '%s'\n", password);

    // Ignore SIGINT.
    struct sigaction sa = {};
    sa.sa_handler = [] (int) {}; // Do-nothing function.
    sigaction(SIGINT, &sa, nullptr);

    // Get the address info for the server socket.
    addrinfo* addr = nullptr;
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;     // Allow either IPv4 or IPv6.
    hints.ai_socktype = SOCK_STREAM; // Use TCP (instead of UDP).
    hints.ai_flags = AI_PASSIVE;     // Ask for an IP address.
    getaddrinfo("localhost", port, &hints, &addr); // FIXME: No error handling

    // Open a socket for incoming connections.
    sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol); // FIXME: No error handling.

    // Allow the same port to be reused. Without this, bind() will sometimes
    // fail with "address already in use."
    const int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    // Bind the socket to the target port.
    bind(sockfd, addr->ai_addr, addr->ai_addrlen); // FIXME: No error handling.
    freeaddrinfo(addr);

    // Listen for incoming connections.
    listen(sockfd, MAX_BACKLOG); // FIXME: No error handling.
    printf("Listening on port %s...\n", port);

    // Start listening for incoming connections.
    while (true) {

        // Receive the next connection.
        sockaddr_storage storage = {};
        socklen_t size = sizeof(storage);
        sockaddr* address = reinterpret_cast<sockaddr*>(&storage);
        int connection = accept(int(sockfd), address, &size);
        if (connection == -1 && errno == EINTR) // Handle Ctrl + C
            break;
        printf("Incoming connection!\n");

        while (true) {
            char buffer[512];
            ssize_t length = recv(connection, buffer, sizeof(buffer), 0);
            if (length <= 0) {
                printf("Connection closed by client!\n");
                close(connection);
                break;
            }
            printf("Received '%.*s'\n", int(length), buffer);
        }
    }
    printf("\rInterrupted by user\n");
}

Server::~Server()
{
    printf("Closing connection...\n");
    safeClose(sockfd);
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

    // Start the server.
    Server server(argv[1], argv[2]);
    server.start();
}
