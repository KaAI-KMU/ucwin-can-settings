#include "Server.h"

Server::Server()
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
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port); // Set to desired port number

    // Bind socket to IP address and port
    if (bind(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(sock, SOMAXCONN);
}

void Server::AcceptConnection()
{
    // Accept incoming connection
    c = sizeof(sockaddr_in);
    if ((newSock = accept(sock, (sockaddr*)&client, &c)) == INVALID_SOCKET) {
        std::cerr << "Accepting connection failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::ReceiveData()
{
    // Receive data from client
    char buffer[1024] = { 0 };
    if (recv(newSock, buffer, 1024, 0) < 0) {
        std::cerr << "Receiving data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Client message: " << buffer << std::endl;
}

void Server::SendData()
{
    // Send data to client
    char message[] = "Hello, client!";
    if (send(newSock, message, strlen(message), 0) < 0) {
        std::cerr << "Sending data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    // Close sockets and clean up Winsock
    closesocket(newSock);
    closesocket(sock);
    WSACleanup();
}