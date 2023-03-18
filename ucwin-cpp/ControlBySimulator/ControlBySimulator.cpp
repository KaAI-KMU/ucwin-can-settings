#include "ControlBySimulator.h"
#include <iostream>

ControlBySimulator::ControlBySimulator()
{

}

ControlBySimulator::~ControlBySimulator()
{

}

void ControlBySimulator::StartProgram()
{
	F8MainFormProxy mainForm = g_applicationServices->GetMainForm();
    F8MainRibbonProxy ribbonMenu = mainForm->GetMainRibbonMenu();

    ribbonTab = ribbonMenu->GetTabByName(L"KaAI");
    if (!Assigned(ribbonTab)) {
        ribbonTab = ribbonMenu->CreateTab(L"KaAI", 10000);
        ribbonTab->SetCaption(L"KaAI API");
    }

    ribbonGroup = ribbonTab->CreateGroup(L"ControlBySimulator", 700);
    ribbonGroup->SetCaption(L"Control By Simulator");

    ribbonButton1 = ribbonGroup->CreateButton(L"ButtonControlBySimulator1");
    ribbonButton1->SetCaption(L"Get CAN data");
    ribbonButton1->SetWidth(120);
    Cb_RibbonMenuItemOnClick callback1 = std::bind(&ControlBySimulator::OnButtonGetCANDataClick, this);
    p_cbHandle1 = ribbonButton1->SetCallbackOnClick(callback1);

    ribbonButton2 = ribbonGroup->CreateButton(L"ButtonControlBySimulator2");
    ribbonButton2->SetCaption(L"Control Vehicle");
    ribbonButton2->SetWidth(120);
    Cb_RibbonMenuItemOnClick callback2 = std::bind(&ControlBySimulator::OnButtonControlCarClick, this);
    p_cbHandle2 = ribbonButton2->SetCallbackOnClick(callback2);
}

void ControlBySimulator::StopProgram()
{
    ribbonButton1->UnsetCallbackOnClick(p_cbHandle1);
    ribbonButton2->UnsetCallbackOnClick(p_cbHandle2);
    ribbonGroup->DeleteControl(ribbonButton1);
    ribbonGroup->DeleteControl(ribbonButton2);
    ribbonTab->DeleteGroup(ribbonGroup);
    if (ribbonTab->GetRibbonGroupsCount() == 0)
        g_applicationServices->GetMainForm()->GetMainRibbonMenu()->DeleteTab(ribbonTab);
}

void ControlBySimulator::OnButtonGetCANDataClick()
{
    
}

void ControlBySimulator::OnButtonControlCarClick()
{

}

void ControlBySimulator::ReceiveCANData()
{

}

void ControlBySimulator::DataParser()
{

}

void ControlBySimulator::ControlVehicle()
{

}

void ControlBySimulator::OnVehicleBeforeCalculateMovement()
{

}