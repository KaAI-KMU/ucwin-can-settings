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
    std::cout << "Client: " << GetIDString(buffer) << " " << GetDataString(buffer) << std::endl;
    
    switch (std::stoi(id)) {
    case 710:
        Parser710(buffer);
        break;
    case 711:
        Parser711(buffer);
        break;
    case 50:
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

std::string Client::GetDataString(char buffer[])
{
    char strTemp[MAX_PATH] = { 0 };
    std::string result = "";
    for (int i = 0; i < 10; i++)
    {
        sprintf_s(strTemp, sizeof(strTemp), "%02X ", buffer[i]);
        result.append(strTemp);
    }

    return result;
}

void Client::Parser710(char buffer[])
{
    // 0 ~ 255
    unsigned char byte1 = static_cast<unsigned char>(buffer[1] & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>(buffer[2] & 0xFF);

    // Steering값 하나로 합치기
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    //std::cout << "value: " << value << std::endl;

    /// byte1이 0~255까지 돌면 byte2가 1 증가
    /// +-90도 기준으로 62825 ~ 60947
    /// 중간값은 61886
    /// 값은 실험할때마다 달라지므로 사용시기에 따라 달라질 수 있음 (2023.04.03 기준 정문규)
    /// uc-win에서 steering은 -1 에서 1 사이의 값만 받기 때문에 변경
    /// 60947 ~ 62825 -> -1 ~ 1
    /// 왼쪽을 -1, 오른쪽을 1로 설정하기 때문에 마지막 mSteering에 -1 추가
    const unsigned short middle = 61886;
    const unsigned short diff = 939;
    mSteering = -(static_cast<double>(value) - middle) / diff;
    //std::cout << "steering: " << mSteering << std::endl;
}

void Client::Parser711(char buffer[])
{
    unsigned char byte1 = static_cast<unsigned char>(buffer[5] & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>(buffer[6] & 0xFF);

    // Throttle값 하나로 합치기
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    // std::cout << "value: " << value << std::endl;

    /// Throttle 범위가 들쭉날쭉하기 때문에 현재 기준으로 작성 (2023.04.03 기준 정문규)
    /// 620 ~ 3480 이 범위 밖은 0 또는 1 처리
    /// steering과 마찬가지로 0 ~ 1로 변경
    const unsigned short min = 620;
    const unsigned short max = 3480;
    const unsigned short diff = max - min;
    if (value < 620)
    {
		mThrottle = 0;
	}
    else if (value > 3480)
    {
        mThrottle = 1;
	}
    else {
        mThrottle = (static_cast<double>(value) - min) / diff;
    }
    // std::cout << "mThrottle: " << mThrottle << std::endl;
}

Client::~Client()
{
    // Close socket and clean up Winsock
    closesocket(sock);
    WSACleanup();
}