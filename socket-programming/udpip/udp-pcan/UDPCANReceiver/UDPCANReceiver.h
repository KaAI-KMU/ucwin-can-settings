#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32")

class UDPCANReceiver
{
public:
	UDPCANReceiver();
	~UDPCANReceiver();

private:
	void Init();
	void Receive();
	void Close();
	std::string GetIDString(char buffer[]);
	std::string GetDataString(char buffer[]);
	void Parser111(char buffer[]);
	void Parser710(char buffer[]);
	void Parser711(char buffer[]);
	double mSteering = 0;
	double mThrottle = 0;
	double mBrake = 0;

	const int LOCALPORT = 9000;
	sockaddr_in receiverAddr;
	WSADATA wsaData;
	SOCKET m_socket;
	int retVal;
	// Set up the sockaddr structure
	const int BUFSIZE = 1024;
	sockaddr_in peerAddr;
	char buffer[1024 + 1];
	int peerAddrLen;
};

