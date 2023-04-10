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
    ribbonButton1->SetCaption(L"Get CAN Data");
    ribbonButton1->SetWidth(120);
    Cb_RibbonMenuItemOnClick callback1 = 
        std::bind(&ControlBySimulator::OnButtonGetCANDataClick, this);
    if (ribbonButton1->IsSetCallbackOnClick())
        return;
    p_cbHandleButtonClick1 = ribbonButton1->SetCallbackOnClick(callback1);

    ribbonButton2 = ribbonGroup->CreateButton(L"ButtonControlBySimulator2");
    ribbonButton2->SetCaption(L"Control Current Vehicle");
    ribbonButton2->SetWidth(120);
    Cb_RibbonMenuItemOnClick callback2 = 
        std::bind(&ControlBySimulator::OnButtonControlCarClick, this);
    if (ribbonButton2->IsSetCallbackOnClick())
        return;
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
    F8TrafficSimulationProxy traffic = g_applicationServices->GetSimulationCore()->GetTrafficSimulation();
    traffic->UnregisterEventTransientObjectDeleted(p_cbHandleTransientDeleted);

    ribbonButton1->UnsetCallbackOnClick(p_cbHandleButtonClick1);
    ribbonButton2->UnsetCallbackOnClick(p_cbHandleButtonClick2);
    ribbonGroup->DeleteControl(ribbonButton1);
    ribbonGroup->DeleteControl(ribbonButton2);
    ribbonTab->DeleteGroup(ribbonGroup);
    if (ribbonTab->GetRibbonGroupsCount() == 0)
        g_applicationServices->GetMainForm()->GetMainRibbonMenu()->DeleteTab(ribbonTab);

    // Close the socket
    closesocket(m_socket);
    WSACleanup();

    // Stop the thread
    StopReceiveCANDataThread();
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

void ControlBySimulator::StopReceiveCANDataThread()
{
    if (receiveThread.joinable())
    {
        receiveThread.join();
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

    Cb_TransientOnBeforeCalculateMovement callback = 
        std::bind(&ControlBySimulator::OnVehicleBeforeCalculateMovement, this, std::placeholders::_1, std::placeholders::_2);
    data.cbHandleOnBeforeCalculateMovement = 
        inst->RegisterCallbackOnBeforeCalculateMovement(callback);

    vehicleDataDict.insert(std::make_pair(inst->GetID(), data));
}

void ControlBySimulator::OnVehicleBeforeCalculateMovement(double dTime, F8TransientInstanceProxy instance)
{
    if (instance->GetTransientType() == _TransientCar) {
        F8TransientCarInstanceProxy proxyCar = 
            std::static_pointer_cast<F8TransientCarInstanceProxy_Class>(instance);
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
}

void ControlBySimulator::OnButtonGetCANDataClick()
{
    InitializeSock();
    StartReceiveCANDataThread();
}

void ControlBySimulator::InitializeSock()
{
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup failed" << std::endl;
    }

    // Create a socket
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET)
    {
        std::cout << "Error creating socket" << std::endl;
    }

    // Bind the socket to any address and the specified port.
    ZeroMemory(&receiverAddr, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(LOCALPORT);
    receiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retVal = bind(m_socket, (SOCKADDR*)&receiverAddr, sizeof(receiverAddr));
    if (retVal == SOCKET_ERROR)
    {
        std::cout << "Error binding socket" << std::endl;
    }
}

void ControlBySimulator::StartReceiveCANDataThread()
{
    receiveThread = std::thread([&]() {
        while (true)
        {
            ReceiveCANData();
        }
        });
}

void ControlBySimulator::ReceiveCANData()
{
    // Receive data until the peer closes the connection
    char buffer[1024] = { 0 };
    sockaddr_in peerAddr;
    int peerAddrLen = sizeof(peerAddr);
    retVal = recvfrom(m_socket, buffer, BUFSIZE, 0, (SOCKADDR*)&peerAddr, &peerAddrLen);
    if (retVal == SOCKET_ERROR)
    {
        std::cout << "Error receiving data" << std::endl;
    }
    else
    {
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
    case 111: // brake
        Parser111(buffer);
		break;
    case 710: // steering
        Parser710(buffer);
        break;
    case 711: // throttle
        Parser711(buffer);
        break;
    }
}

void ControlBySimulator::Parser111(char buffer[])
{
    unsigned char byte1 = static_cast<unsigned char>(buffer[0] & 0xFF);
    double decimalValue = double(byte1) / 255.0;
    mBrake = decimalValue;
}

void ControlBySimulator::Parser710(char buffer[])
{
    // 0 ~ 255
    unsigned char byte1 = static_cast<unsigned char>(buffer[1] & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>(buffer[2] & 0xFF);

    // Steering값 하나로 합치기
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    //std::cout << "value: " << value << std::endl;

    /// byte1이 0~255까지 돌면 byte2가 1 증가
    /// +-90도 기준으로 2700 ~ 840
    /// 중간값은 1770
    /// 값은 실험할때마다 달라지므로 사용시기에 따라 달라질 수 있음 (2023.04.05 기준 정문규)
    /// uc-win에서 steering은 -1 에서 1 사이의 값만 받기 때문에 변경
    /// 840 ~ 2700 -> -1 ~ 1
    /// 왼쪽을 -1, 오른쪽을 1로 설정하기 때문에 마지막 mSteering에 -1 추가
    const unsigned short middle = 1770;
    const unsigned short max = 2700;
    const unsigned short diff = max - middle;
    if (value > 2700) {
        mSteering = -1.0;
    }
    else if (value < 840) {
        mSteering = 1.0;
    }
    else {
        mSteering = -(static_cast<double>(value) - middle) / diff;
    }
    // std::cout << "steering: " << mSteering << std::endl;
}

void ControlBySimulator::Parser711(char buffer[])
{
    unsigned char byte1 = static_cast<unsigned char>(buffer[5] & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>(buffer[6] & 0xFF);

    // Throttle값 하나로 합치기
    // unsigned short range: 0 ~ 65535
    unsigned short value = (byte2 << 8) | byte1;
    // std::cout << "value: " << value << std::endl;

    /// Throttle 범위가 들쭉날쭉하기 때문에 현재 기준으로 작성 (2023.04.03 기준 정문규)
    /// 620 ~ 3480 이 범위 밖은 0 또는 1 처리
    /// steering과 마찬가지로 0 ~ 1로 변경
    const unsigned short min = 620;
    const unsigned short max = 3480;
    const unsigned short diff = max - min;
    if (value < 620)
    {
        mThrottle = 0;
    }
    else if (value > 3480)
    {
        mThrottle = 1;
    }
    else {
        mThrottle = (static_cast<double>(value) - min) / diff;
    }
    // std::cout << "mThrottle: " << mThrottle << std::endl;
}