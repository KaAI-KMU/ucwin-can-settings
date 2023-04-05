#pragma once
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32")

class UDPSender
{
public:
	UDPSender();
	~UDPSender();

private:
	void Init();
	void Send();
	void Close();
	
	const std::string REMOTEIP = "255.255.255.255";
	const int REMOTEPORT = 9000;
	WSADATA wsaData;
	SOCKET m_socket;
	BOOL bEnableBroadcast = TRUE;
	int retVal;
	sockaddr_in senderAddr;
	char buf[512 + 1];
	int len;
	int i = 0;
};

