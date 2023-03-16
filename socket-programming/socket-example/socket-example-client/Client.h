#pragma once
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

class Client
{
private:
    WSADATA wsaData; // Winsock initialization data
    SOCKET sock;
    sockaddr_in server;
    const int port = 8888;

public:
    Client();
    ~Client();

    void ConnectToServer();
    void SendData();
    void ReceiveData();
};

