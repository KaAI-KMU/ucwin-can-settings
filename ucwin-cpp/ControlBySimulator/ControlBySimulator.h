#pragma once
#include "F8API.h"
#include "pch.h"
#include "windows.h"

class ControlBySimulator
{
private:
	F8MainRibbonTabProxy ribbonTab;
	F8MainRibbonGroupProxy ribbonGroup;
	F8MainRibbonButtonProxy ribbonButton1, ribbonButton2;
	void* p_cbHandle1, * p_cbHandle2;
	

public:
	ControlBySimulator();
	~ControlBySimulator();

	void StartProgram();
	void StopProgram();

private:
	void OnButtonGetCANDataClick();
	void OnButtonControlCarClick();
	void ReceiveCANData();
	void DataParser();
	void ControlVehicle();
	void OnVehicleBeforeCalculateMovement();
};

