#include "ControlBySimulator.h"

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
    Cb_RibbonMenuItemOnClick callback1 = 
        std::bind(&ControlBySimulator::OnButtonGetCANDataClick, this);
    p_cbHandleButtonClick1 = ribbonButton1->SetCallbackOnClick(callback1);

    ribbonButton2 = ribbonGroup->CreateButton(L"ButtonControlBySimulator2");
    ribbonButton2->SetCaption(L"Control Vehicle");
    ribbonButton2->SetWidth(120);
    Cb_RibbonMenuItemOnClick callback2 = 
        std::bind(&ControlBySimulator::OnButtonControlCarClick, this);
    p_cbHandleButtonClick2 = ribbonButton2->SetCallbackOnClick(callback2);

    F8TrafficSimulationProxy traffic = 
        g_applicationServices->GetSimulationCore()->GetTrafficSimulation();
    Cb_TrafficTransientObjectDeleted callbackTransientDeleted = 
        std::bind(&ControlBySimulator::OnTransientDeleted, this, std::placeholders::_1);
    p_cbHandleTransientDeleted = 
        traffic->RegisterEventTransientObjectDeleted(callbackTransientDeleted);
}

void ControlBySimulator::StopProgram()
{
    ribbonButton1->UnsetCallbackOnClick(p_cbHandleButtonClick1);
    ribbonButton2->UnsetCallbackOnClick(p_cbHandleButtonClick2);
    ribbonGroup->DeleteControl(ribbonButton1);
    ribbonGroup->DeleteControl(ribbonButton2);
    ribbonTab->DeleteGroup(ribbonGroup);
    if (ribbonTab->GetRibbonGroupsCount() == 0)
        g_applicationServices->GetMainForm()->GetMainRibbonMenu()->DeleteTab(ribbonTab);
}

void ControlBySimulator::OnTransientDeleted(F8TransientInstanceProxy instance)
{
    std::wstring proxyName = instance->GetProxyName();
    if (instance->GetTransientType() == _TransientCar)
    {
        F8TransientCarInstanceProxy carProxy = 
            std::static_pointer_cast<F8TransientCarInstanceProxy_Class>(instance);
        RemoveControlledInstance(carProxy);
    }
}

void ControlBySimulator::OnButtonControlCarClick()
{
    F8MainDriverProxy driver = g_applicationServices->GetSimulationCore()->GetTrafficSimulation()->GetDriver();
    if (Assigned(driver)) {
        F8TransientCarInstanceProxy car = driver->GetCurrentCar();
        if (Assigned(car))
        {
            RemoveControlledInstance(car);
            AddControlledInstance(car);
        }
    }
}

void ControlBySimulator::RemoveControlledInstance(F8TransientCarInstanceProxy inst)
{
    decltype(vehicleDataDict)::iterator itr;
    itr = vehicleDataDict.find(inst->GetID());
    if (itr != vehicleDataDict.end())
    {
        inst->UnregisterCallbackOnBeforeCalculateMovement(itr->second.cbHandleOnBeforeCalculateMovement);
        vehicleDataDict.erase(itr);
    }
}

void ControlBySimulator::AddControlledInstance(F8TransientCarInstanceProxy inst)
{
    VehicleData data;
    data.proxy = inst;
    Cb_TransientOnBeforeCalculateMovement callback1 = 
        std::bind(&ControlBySimulator::OnVehicleBeforeCalculateMovement, this, std::placeholders::_1, std::placeholders::_2);
    data.cbHandleOnBeforeCalculateMovement = 
        inst->RegisterCallbackOnBeforeCalculateMovement(callback1);
    vehicleDataDict.insert(std::make_pair(inst->GetID(), data));
}

void ControlBySimulator::OnVehicleBeforeCalculateMovement(double dTime, F8TransientInstanceProxy instance)
{
    if (instance->GetTransientType() == _TransientCar) {
        F8TransientCarInstanceProxy proxyCar = 
            std::static_pointer_cast<F8TransientCarInstanceProxy_Class>(instance);
        ControlVehicle(proxyCar, dTime);
    }
}

void ControlBySimulator::ControlVehicle(F8TransientCarInstanceProxy& proxyCar, double time)
{
    decltype(vehicleDataDict)::iterator itr;
    itr = vehicleDataDict.find(proxyCar->GetID());
    if (itr != vehicleDataDict.end()) {
        const auto& data = itr->second;
        double steering = data.steering;
        double throttle = data.throttle;
        double brake = data.brake;

        proxyCar->SetEngineOn(true);
        proxyCar->SetSteering(steering);
        proxyCar->SetThrottle(throttle);
        proxyCar->SetBrake(brake);
        proxyCar->SetClutch(0.0);
    }
}

void ControlBySimulator::OnButtonGetCANDataClick()
{
    ReceiveCANData();
}

void ControlBySimulator::ReceiveCANData()
{
    DataParser();
}

void ControlBySimulator::DataParser()
{
    
}