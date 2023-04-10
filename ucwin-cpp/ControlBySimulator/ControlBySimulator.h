#pragma once
#include "F8API.h"
#include "pch.h"
#include <map>
#include <iostream>
#include <memory>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sstream>
#include <thread>
#include <future>

#pragma comment(lib, "ws2_32")

struct VehicleData {
	F8TransientInstanceProxy proxy;
	void* cbHandleOnBeforeCalculateMovement;
	double steering = 0.0;
	double throttle = 0.0;
	double brake = 0.0;
};

class ControlBySimulator
{
private:
	// uc-win member
	F8MainRibbonTabProxy ribbonTab;
	F8MainRibbonGroupProxy ribbonGroup;
	F8MainRibbonButtonProxy ribbonButton1, ribbonButton2;
	void* p_cbHandleButtonClick1, * p_cbHandleButtonClick2, 
		* p_cbHandleTransientDeleted;

	std::map<int, VehicleData> vehicleDataDict;

	double mSteering = 0.0;
	double mThrottle = 0.0;
	double mBrake    = 0.0;

	// socket member
	const int LOCALPORT = 9000;
	sockaddr_in receiverAddr;
	WSADATA wsaData;
	SOCKET m_socket;
	int retVal;
	std::thread receiveThread;

	// Set up the sockaddr structure
	const int BUFSIZE = 1024;

public:
	ControlBySimulator();
	~ControlBySimulator();

	void StartProgram();
	void StopProgram();

private:
	// callback control car
	void OnButtonControlCarClick();
	void OnTransientDeleted(F8TransientInstanceProxy instance);
	void RemoveControlledInstance(F8TransientCarInstanceProxy inst);
	void AddControlledInstance(F8TransientCarInstanceProxy inst);
	void OnVehicleBeforeCalculateMovement(double dTime, F8TransientInstanceProxy instance);

	// callback get CAN data
	void OnButtonGetCANDataClick();
	void InitializeSock();
	void StartReceiveCANDataThread();
	void ReceiveCANData();
	void StopReceiveCANDataThread();

	// Parsing CAN data
	std::string GetIDString(char buffer[]);
	void DataParser(int id, char buffer[]);
	void Parser111(char buffer[]);
	void Parser710(char buffer[]);
	void Parser711(char buffer[]);
};