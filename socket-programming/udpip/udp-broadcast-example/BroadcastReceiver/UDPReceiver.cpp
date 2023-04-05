#include "UDPReceiver.h"

UDPReceiver::UDPReceiver()
{
	Init();
	while (1)
	{
		Receive();
	}
}

UDPReceiver::~UDPReceiver()
{
	Close();
}

void UDPReceiver::Init()
{
	// Initialize Winsock
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create a socket
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "Error creating socket" << std::endl;
	}

	// Bind the socket to any address and the specified port.
	sockaddr_in receiverAddr;
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

void UDPReceiver::Receive()
{
	// Receive data until the peer closes the connection
	peerAddrLen = sizeof(peerAddr);
	retVal = recvfrom(m_socket, buf, BUFSIZE, 0, (SOCKADDR*)&peerAddr, &peerAddrLen);
	if (retVal == SOCKET_ERROR)
	{
		std::cout << "Error receiving data" << std::endl;
	}
	else
	{
		buf[retVal] = '\0';
		std::cout << "Received: " << buf << std::endl;
	}
}

void UDPReceiver::Close()
{
	// Close the socket
	closesocket(m_socket);
	WSACleanup();
}