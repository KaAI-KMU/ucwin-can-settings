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

	// Steering�� �ϳ��� ��ġ��
	// unsigned short range: 0 ~ 65535
	unsigned short value = (byte2 << 8) | byte1;
	//std::cout << "value: " << value << std::endl;

	/// byte1�� 0~255���� ���� byte2�� 1 ����
	/// +-90�� �������� 2700 ~ 840
	/// �߰����� 1770
	/// ���� �����Ҷ����� �޶����Ƿ� ���ñ⿡ ���� �޶��� �� ���� (2023.04.05 ���� ������)
	/// uc-win���� steering�� -1 ���� 1 ������ ���� �ޱ� ������ ����
	/// 840 ~ 2700 -> -1 ~ 1
	/// ������ -1, �������� 1�� �����ϱ� ������ ������ mSteering�� -1 �߰�
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
	//std::cout << "mThrottle: " << mThrottle << std::endl;
}