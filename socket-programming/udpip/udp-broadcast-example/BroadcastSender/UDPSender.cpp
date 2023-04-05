#include "UDPSender.h"

UDPSender::UDPSender()
{
	Init();
	while (1)
	{
		Send();
	}
}

UDPSender::~UDPSender()
{
	Close();
}

void UDPSender::Init()
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

	// Enable broadcast
	retVal = setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bEnableBroadcast, sizeof(bEnableBroadcast));
	if (retVal == SOCKET_ERROR)
	{
		std::cout << "Error enabling broadcast" << std::endl;
	}

	// Set up the sockaddr structure
	ZeroMemory(&senderAddr, sizeof(senderAddr));
	senderAddr.sin_family = AF_INET;
	senderAddr.sin_port = htons(REMOTEPORT);
	senderAddr.sin_addr.s_addr = inet_addr(REMOTEIP.c_str());
}

void UDPSender::Send()
{
	// 직접 받기
		/*
		std::cout << "Enter message: ";
		std::cin.getline(buf, BUFSIZE);
		*/

		// 계속 보내기
	sprintf_s(buf, "Hello %d", i++);

	retVal = sendto(m_socket, buf, strlen(buf), 0, (SOCKADDR*)&senderAddr, sizeof(senderAddr));
	if (retVal == SOCKET_ERROR)
	{
		std::cout << "Error sending data" << std::endl;
	}
}

void UDPSender::Close()
{
	// Close the socket
	closesocket(m_socket);
	WSACleanup();
}