#pragma once

#define PORT_MAX 65535 // The maximum allowed port number.
#define MAX_BACKLOG 20 // Maximum length of the pending connection queue.

class Server
{
public:
    Server(const char* port, const char* password);
    ~Server();

    void start();

private:
    int sockfd = -1;
    const char* port = nullptr;
    const char* password = nullptr;
};
