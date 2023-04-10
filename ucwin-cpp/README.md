# UC-win/Road C++

## Step 1: Download UC-win/Road x64 and SDK

사용버전

- UC-win/Road Data 16.0
- UC-win/Road SDK 16.0.3

KaAI 이메일로 forum8 사이트 접속 - 최신 버전 data 및 sdk 다운 - 각각 압축해제 - installer 실행

## Step 2: UC-win/Road C++ API setting

C:\UCwinRoad_SDK 16.0.3\CPP\Doc\en 에 있는 APIIntroduction을 참고해서 프로젝트 세팅을 진행한다. 영어로 적혀있지만 상세하게 적혀있고 Visual Studio 2019 버전 기준이기 때문에 2022 버전과 다른 부분이 없어 어렵지 않게 따라할 수 있다.

## Step 3: Run UC-win/Road Sample C++ codes

다행히 APIIntroduction에 기본적인 API 사용법, 샘플 코드의 기능을 자세하게 서술해놨다. 나중에 어느 곳에서 문제가 생길지 모르니 꼼꼼히 읽고 샘플 코드를 실행해보면서 공부한다.

## Step 4: UC-win/Road Callback

예제를 보면서 callback이란 부분이 있다.

uc-win/road c++ api에서 내가 작성한 함수를 사용하는 방법 중 권장하는 방법은 callback에 등록하는 것이다. 버튼 클릭 콜백에 내가 만든 함수를 등록하면 여러 함수를 동시에 실행할 수 있는 기능을 제공하고 있다.

콜백 예시로 아래 코드를 보면 RibbonMenu를 클릭할 때 callback OnButtonControlCarClick이라는 함수를 실행한다.

```cpp
Cb_RibbonMenuItemOnClick callback2 = 
        std::bind(&ControlBySimulator::OnButtonControlCarClick, this);
```

다음 함수를 보자. 이전과 달리 placeholder가 있는데 이는 함수에 필요한 파라미터 개수이다.

```cpp
Cb_TransientOnBeforeCalculateMovement callback1 = 
        std::bind(&ControlBySimulator::OnVehicleBeforeCalculateMovement, this, std::placeholders::_1, std::placeholders::_2);
```

## Step 5: Thread for CAN

처음 CAN 데이터를 받기 위해 UDPReceive 함수를 차량 구조체의 콜백에 등록했지만 딜레이가 생겼었다. 이 문제를 해결하기 위해 콜백이 아닌 별개의 thread를 열어 독립적으로 CAN data를 얻을 수 있게 설계했다. 다음 코드를 보자.

uc-win 상에서 버튼을 누르면 소켓을 초기화하고 CAN thread를 새롭게 열어주는 함수를 실행한다. 

```cpp
void ControlBySimulator::OnButtonGetCANDataClick()
{
    InitializeSock();
    StartReceiveCANDataThread();
}
```

함수가 시작되면 람다 함수로 CAN data를 받는 함수를 while문으로 호출한다. 새로운 thread로 열었기 때문에 ucwin 어플리케이션이 멈추지 않고 함께 작동한다.

```cpp
void ControlBySimulator::StartReceiveCANDataThread()
{
    receiveThread = std::thread([&]() {
        while (true)
        {
            ReceiveCANData();
        }
        });
}
```

어플리케이션을 종료할 때는 join을 통해 thread를 닫아준다.

```cpp
void ControlBySimulator::StopReceiveCANDataThread()
{
    if (receiveThread.joinable())
    {
        receiveThread.join();
    }
}
```

이렇게 CAN 값을 받아 parsing까지 thread를 통해 완료한다. 

## Step 6: Contorl Car By CAN Data

thread에서 바뀐 CAN값으로 uc-win/road 상의 차량을 구동한다. SampleOverrideCarInput 예제를 참조하면 쉽게 구현할 수 있다.

먼저 버튼을 누르면 콜백 함수를 실행한다.

현재 자동차 드라이버를 받아서 이전에 있던 인스턴스를 삭제하는 함수(이전 것들 비우기)와 현재 차량의 인스턴스를 추가(현재 드라이버 추가하기)하는 함수를 호출한다.

```cpp
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
```

iterator를 사용해서 vehicleDataDict에 저장된 인스턴스들의 콜백함수를 지워준다.

```cpp
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
```

현재 VehicleData의 콜백 함수를 등록하고 dict에 id와 인스턴스 구조체를 함께 저장한다.

```cpp
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
```

인스턴스가 생성되는 순간 이 함수가 무한루프로 돌아간다. (while문이 없어도 무한으로 돈다) 따라서 프로그램이 실행하는 중에는 이 함수가 계속 돌아가면서 현재 차량의 steering, throttle, brake 값을 CAN 값에 따라 setting 해준다.

```cpp
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
```