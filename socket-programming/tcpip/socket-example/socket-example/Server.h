#pragma once
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib") // Link to Winsock library

class Server
{
private:
    WSADATA wsaData; // Winsock initialization data
    SOCKET sock, newSock;
    sockaddr_in server, client;
    int c;
    const int port = 8888;

public:
    Server();
    ~Server();

    void AcceptConnection();
    void ReceiveData();
    void SendData();
};

