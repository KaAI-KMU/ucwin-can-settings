#pragma once
#include "F8API.h"
#include "pch.h"
#include "Serial.h"
#include <map>
#include <iostream>
#include <memory>
#include <WinSock2.h>
#include <sstream>

#pragma comment(lib, "ws2_32")

struct VehicleData {
	F8TransientInstanceProxy proxy;
	void* cbHandleOnBeforeCalculateMovement;
	void* cbReceiveCANData;
	void* cbReceiveBrakeData;
	double time = 0.0;
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

	// winsock member
	WSADATA wsaData;
	SOCKET sock;
	sockaddr_in server;
	const int port = 8888;
	const std::string ipAddress = "127.0.0.1";

	double mSteering = 0.0;
	double mThrottle = 0.0;
	double tmpSteer = 0.0;
	double tmpThrottle = 0.0;

	// serial member
	double mBrake    = 0.0;
	Serial* SP;
	char brakeData[256] = "";
	int dataLength = 255;
	int readResult = 0;

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
	void ControlVehicle(F8TransientCarInstanceProxy proxyCar, double time);

	// callback get CAN data
	void OnButtonGetCANDataClick();
	void InitializeSock();
	void ConnectToServer();
	void ReceiveCANData();
	std::string GetIDString(char buffer[]);
	void DataParser(int id, char buffer[]);
	void Parser710(char buffer[]);
	void Parser711(char buffer[]);

	// callback get Brake data
	void InitializeSerial();
	void ReceiveBrakeData();
};