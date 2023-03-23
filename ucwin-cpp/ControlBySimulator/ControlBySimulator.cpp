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
    ribbonButton1->SetCaption(L"Set winsocket");
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

    // Close socket and clean up Winsock
    closesocket(sock);
    WSACleanup();
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
    Cb_TransientOnBeforeCalculateMovement callback2 =
        std::bind(&ControlBySimulator::ReceiveCANData, this);
    data.cbReceiveCANData =
        inst->RegisterCallbackOnBeforeCalculateMovement(callback2);
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

        proxyCar->SetEngineOn(true);
        proxyCar->SetSteering(mSteering);
        proxyCar->SetThrottle(mThrottle);
        proxyCar->SetBrake(mBrake);
        proxyCar->SetClutch(0.0);
    }
}

void ControlBySimulator::OnButtonGetCANDataClick()
{
    InitializeSock();
    ConnectToServer();
}

void ControlBySimulator::InitializeSock()
{
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set server information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddress.c_str()); // Set to server IP address
    server.sin_port = htons(port); // Set to server port number
}

void ControlBySimulator::ConnectToServer()
{
    // Connect to server
    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ControlBySimulator::ReceiveCANData()
{
    // Receive data from server
    char buffer[1024] = { 0 };
    if (recv(sock, buffer, 1024, 0) < 0) {
        std::cerr << "Receiving data failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::string id = GetIDString(buffer);
        DataParser(std::stoi(id), buffer);
    }
}

std::string ControlBySimulator::GetIDString(char buffer[])
{
    char strTemp[MAX_PATH] = { 0 };
    std::string id = "";
    for (int i = 9; i > 7; i--) // little endian 때문에 반대로 for 문을 돌린다
    {
        sprintf_s(strTemp, sizeof(strTemp), "%X", buffer[i]);
        id.append(strTemp);
    }
    return id;
}

void ControlBySimulator::DataParser(int id, char buffer[])
{
    switch (id) {
    case 710: // steering
        Parser710(buffer);
        break;
    case 711: // throttle
        Parser711(buffer);
        break;
    }
}

void ControlBySimulator::Parser710(char buffer[])
{
    int tmp1 = buffer[1];
    int tmp2 = buffer[2];
    int tmp3 = tmp2 * 256 + tmp1;
    
    tmp3 = (double)tmp3;
    if (tmp3 < 1000)
        tmpSteer = tmp3 / 10;
    else {
        tmp3 = tmp3 ^ 0b1111111111111111;
        tmpSteer = ~tmp3 / 10;
    }
    
    tmpSteer = (tmpSteer + 6222) / 1000; // 큰 수를 나눌수록 핸들 감도 낮아짐
    if (tmpSteer < 1 && tmpSteer > -1) {
        mSteering = -tmpSteer;
    }
}

void ControlBySimulator::Parser711(char buffer[])
{
    int tmp1 = buffer[5];
    int tmp2 = buffer[6];
    int tmp3 = tmp2 * 256 + tmp1;
    
    if (tmp3 > 610) {
        tmpThrottle = ((double)tmp3 - 610) / (3444 - 610);
        if (tmpThrottle < 1)
            mThrottle = tmpThrottle;
    }
}