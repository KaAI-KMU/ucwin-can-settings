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
    char buffer[1024] = { 0 };
    if (recv(sock, buffer, 1024, 0) < 0) {
        std::cerr << "Receiving data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string id = GetIDString(buffer);

    switch (std::stoi(id)) {
    case 50: 
        Parser50(buffer);
        break;
    case 631:
        Parser631(buffer);
        break;
    case 710:
        Parser710(buffer);
        break;
    case 711:
        Parser711(buffer);
        break;
    }
}

std::string Client::GetIDString(char buffer[])
{
    char strTemp[MAX_PATH] = { 0 };
    std::string id = "";
    for (int i = 9; i > 7; i--)
    {
        sprintf_s(strTemp, sizeof(strTemp), "%X", buffer[i]);
        id.append(strTemp);
    }
    return id;
}

void Client::Parser50(char buffer[])
{
    
}

void Client::Parser631(char buffer[])
{

}

void Client::Parser710(char buffer[])
{
    int tmp1 = buffer[1];
    int tmp2 = buffer[2];
    int tmp3 = tmp2 * 256 + tmp1;
    if (tmp3 < 1000)
        mSteering = tmp3 / 10;
    else {
        tmp3 = tmp3 ^ 0b1111111111111111;
        mSteering = ~tmp3 / 10;
    }
    std::cout << "710 " << mSteering << std::endl;
}

void Client::Parser711(char buffer[])
{
    int tmp1 = buffer[5];
    int tmp2 = buffer[6];
    mThrottle = tmp2 * 256 + tmp1;

    std::cout << "711 " << mThrottle << std::endl;
}

Client::~Client()
{
    // Close socket and clean up Winsock
    closesocket(sock);
    WSACleanup();
}