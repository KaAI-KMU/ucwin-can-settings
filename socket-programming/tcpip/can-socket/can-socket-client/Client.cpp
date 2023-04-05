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

    // Steering�� �ϳ��� ��ġ��
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    //std::cout << "value: " << value << std::endl;

    /// byte1�� 0~255���� ���� byte2�� 1 ����
    /// +-90�� �������� 62825 ~ 60947
    /// �߰����� 61886
    /// ���� �����Ҷ����� �޶����Ƿ� ���ñ⿡ ���� �޶��� �� ���� (2023.04.03 ���� ������)
    /// uc-win���� steering�� -1 ���� 1 ������ ���� �ޱ� ������ ����
    /// 60947 ~ 62825 -> -1 ~ 1
    /// ������ -1, �������� 1�� �����ϱ� ������ ������ mSteering�� -1 �߰�
    const unsigned short middle = 61886;
    const unsigned short diff = 939;
    mSteering = -(static_cast<double>(value) - middle) / diff;
    //std::cout << "steering: " << mSteering << std::endl;
}

void Client::Parser711(char buffer[])
{
    unsigned char byte1 = static_cast<unsigned char>(buffer[5] & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>(buffer[6] & 0xFF);

    // Throttle�� �ϳ��� ��ġ��
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    // std::cout << "value: " << value << std::endl;

    /// Throttle ������ ���߳����ϱ� ������ ���� �������� �ۼ� (2023.04.03 ���� ������)
    /// 620 ~ 3480 �� ���� ���� 0 �Ǵ� 1 ó��
    /// steering�� ���������� 0 ~ 1�� ����
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