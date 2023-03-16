#include "Client.h"

Client::Client()
{
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set server information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddress.c_str()); // Set to server IP address
    server.sin_port = htons(port); // Set to server port number
}

void Client::ConnectToServer()
{
    // Connect to server
    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Client::SendData()
{
    // Send data to server
    const std::string message = "Hello, server!";
    if (send(sock, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Sending data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Client::ReceiveData()
{
    // Receive data from server
    char buffer[1024] = {0};
    if (recv(sock, buffer, 1024, 0) < 0) {
        std::cerr << "Receiving data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server message: " << buffer << std::endl;
}

Client::~Client()
{
    // Close socket and clean up Winsock
    closesocket(sock);
    WSACleanup();
}