#pragma once
#include "F8API.h"
#include "pch.h"
#include "windows.h"
#include <map>
#include <iostream>
#include <memory>

struct VehicleData {
	F8TransientInstanceProxy proxy;
	void* cbHandleOnBeforeCalculateMovement;
	double time = 0.0;
	double steering = 0.0;
	double throttle = 1.0;
	double brake = 0.0;
};

static const double pi = 3.141592653589793;

class ControlBySimulator
{
private:
	F8MainRibbonTabProxy ribbonTab;
	F8MainRibbonGroupProxy ribbonGroup;
	F8MainRibbonButtonProxy ribbonButton1, ribbonButton2;
	void* p_cbHandleButtonClick1, * p_cbHandleButtonClick2, * p_cbHandleTransientDeleted;

	std::map<int, VehicleData> vehicleDataDict;
	
public:
	ControlBySimulator();
	~ControlBySimulator();

	void StartProgram();
	void StopProgram();

private:
	void OnButtonControlCarClick();
	void OnTransientDeleted(F8TransientInstanceProxy instance);
	void RemoveControlledInstance(F8TransientCarInstanceProxy inst);
	void AddControlledInstance(F8TransientCarInstanceProxy inst);
	void OnVehicleBeforeCalculateMovement(double dTime, F8TransientInstanceProxy instance);
	void ControlVehicle(F8TransientCarInstanceProxy& proxyCar, double time);

	void OnButtonGetCANDataClick();
	void ReceiveCANData();
	void DataParser();
};