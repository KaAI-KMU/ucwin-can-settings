# Arduino Brake
아두이노에서 Serial Communication을 통해 C++에서 값을 읽어온다.

아두이노를 활용해 브레이크 값을 받아온다.

아두이노에서 받은 브레이크 값(0~1)을 바로 UC-win/Road로 보낸다.

## Step 1: Get Brake value using slide sensor

아두이노 우노와 slide sensor를 활용해 브레이크 값을 받아온다. 회로도는 다음과 같다.

| Slide pot | Arduino |
| --- | --- |
| VCC | 5V |
| GND | GND |
| DTB | A0 |

## Step 2: Link with C++

[다음 사이트](https://playground.arduino.cc/Interfacing/CPPWindows/)를 참고해서 코드를 짜면 아두이노에서 출력한 데이터를 받을 수 있다.

위 코드를 참고해서 brake 값을 c++에서 받는 코드를 작성해본다.

- [아두이노 코드](/arduino-brake/slide-pot-brake/slide-pot-brake.ino)
- [Serial.cpp](/arduino-brake/arduino-cpp-example/Serial.cpp)
- [Serial.h](/arduino-brake/arduino-cpp-example/Serial.h)
- [main.cpp](/arduino-brake/arduino-cpp-example/main.cpp)

![Untitled (2)](https://user-images.githubusercontent.com/111988634/227460412-9e682109-7998-47e6-9eb5-95b517981ad5.png)


## Step 3: Integrate with UC-win/Road C++

저번에 작성했던 uc-win/road 프로젝트에 Serial.cpp, Serial.h를 추가해주고 그대로 사용하면 된다.

기존 ControlBySimulator에 2개 함수를 추가해준다.

```cpp
void ControlBySimulator::InitializeSerial()
{
    SP = new Serial("\\\\.\\COM3");

    /*if (SP->IsConnected())
        std::cout << "Arduino connected" << std::endl;*/
}

void ControlBySimulator::ReceiveBrakeData()
{
    readResult = SP->ReadData(brakeData, dataLength);
    brakeData[readResult] = 0;
    double brake = std::atof(brakeData);
    if (brake < 1 && brake > 0)
        mBrake = brake;
}
```

버튼을 누르면 ReceiveBrakeData 함수를 호출한다. 콜백 등록

```cpp
Cb_TransientOnBeforeCalculateMovement callback3 =
        std::bind(&ControlBySimulator::ReceiveBrakeData, this);
    data.cbReceiveBrakeData =
        inst->RegisterCallbackOnBeforeCalculateMovement(callback3);
```

자세한 코드는 [ControlBySimulator.cpp](../ucwin-cpp/ControlBySimulator/ControlBySimulator.cpp)를 참고.