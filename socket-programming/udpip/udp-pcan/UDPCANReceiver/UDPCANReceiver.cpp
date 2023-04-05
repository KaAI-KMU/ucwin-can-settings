#include "UDPCANReceiver.h"

UDPCANReceiver::UDPCANReceiver()
{
	Init();
	while (1)
	{
		Receive();
	}
}

UDPCANReceiver::~UDPCANReceiver()
{
	Close();
}

void UDPCANReceiver::Init()
{
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
	}

	// Create a socket
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "Error creating socket" << std::endl;
	}

	// Bind the socket to any address and the specified port.
	ZeroMemory(&receiverAddr, sizeof(receiverAddr));
	receiverAddr.sin_family = AF_INET;
	receiverAddr.sin_port = htons(LOCALPORT);
	receiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	retVal = bind(m_socket, (SOCKADDR*)&receiverAddr, sizeof(receiverAddr));
	if (retVal == SOCKET_ERROR)
	{
		std::cout << "Error binding socket" << std::endl;
	}
}

void UDPCANReceiver::Receive()
{
	// Receive data until the peer closes the connection
	peerAddrLen = sizeof(peerAddr);
	retVal = recvfrom(m_socket, buffer, BUFSIZE, 0, (SOCKADDR*)&peerAddr, &peerAddrLen);
	if (retVal == SOCKET_ERROR)
	{
		std::cout << "Error receiving data" << std::endl;
	}
	else
	{
		std::string id = GetIDString(buffer);
		std::cout << "ID: " << id << " " << GetDataString(buffer) << std::endl;

		switch (std::stoi(id)) {
		case 710:
			Parser710(buffer);
			break;
		case 711:
			Parser711(buffer);
			break;
		}
	}
}

void UDPCANReceiver::Close()
{
	// Close the socket
	closesocket(m_socket);
	WSACleanup();
}


std::string UDPCANReceiver::GetIDString(char buffer[])
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

std::string UDPCANReceiver::GetDataString(char buffer[])
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

void UDPCANReceiver::Parser710(char buffer[])
{
	// 0 ~ 255
	unsigned char byte1 = static_cast<unsigned char>(buffer[1] & 0xFF);
	unsigned char byte2 = static_cast<unsigned char>(buffer[2] & 0xFF);

	// Steering값 하나로 합치기
	// unsigned short range: 0 ~ 65535
	unsigned short value = (byte2 << 8) | byte1;
	//std::cout << "value: " << value << std::endl;

	/// byte1이 0~255까지 돌면 byte2가 1 증가
	/// +-90도 기준으로 2700 ~ 840
	/// 중간값은 1770
	/// 값은 실험할때마다 달라지므로 사용시기에 따라 달라질 수 있음 (2023.04.05 기준 정문규)
	/// uc-win에서 steering은 -1 에서 1 사이의 값만 받기 때문에 변경
	/// 840 ~ 2700 -> -1 ~ 1
	/// 왼쪽을 -1, 오른쪽을 1로 설정하기 때문에 마지막 mSteering에 -1 추가
	const unsigned short middle = 1770;
	const unsigned short max = 2700;
	const unsigned short diff = max - middle;
	if (value > 2700) {
		mSteering = -1.0;
	}
	else if (value < 840) {
		mSteering = 1.0;
	}
	else {
		mSteering = -(static_cast<double>(value) - middle) / diff;
	}
	// std::cout << "steering: " << mSteering << std::endl;
}

void UDPCANReceiver::Parser711(char buffer[])
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
	//std::cout << "mThrottle: " << mThrottle << std::endl;
}