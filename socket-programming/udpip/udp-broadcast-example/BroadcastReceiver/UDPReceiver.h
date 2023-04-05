#pragma once
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32")

class UDPReceiver
{
public:
	UDPReceiver();
	~UDPReceiver();

private:
	void Init();
	void Receive();
	void Close();

	const int LOCALPORT = 9000;
	WSADATA wsaData;
	SOCKET m_socket;
	int retVal;
	// Set up the sockaddr structure
	const int BUFSIZE = 512;
	sockaddr_in peerAddr;
	char buf[512 + 1];
	int peerAddrLen;
};

