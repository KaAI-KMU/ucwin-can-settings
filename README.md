# ucwin-can-settings
UC-win/Road 상에서 운전 시뮬레이터 사용하기 위한 CAN 세팅

### 프로젝트 개요
기존의 Delphi로 작성된 코드를 C++로 변경하는 첫 프로젝트이다. 또한 기존의 CAN 코드들이 인수인계가 제대로 되지 않고 유지보수가 되지 않아 github를 활용해 새롭게 정리하려고 한다. 이번 프로젝트를 배우면 다음과 같은 기술을 배울 수 있을 것으로 예상한다.

1. C++을 공부할 수 있다. ( 현재 KaAI에서 C++로 진행하는 프로젝트가 하나도 없다. 현업에서는 어디에서도 많이 사용되므로 꼭 알아야한다. )
2. CMake 작성법 및 사용방법 ( Visual Studio의 복잡한 빌드 시스템을 공부하기 보다는 요즘 모든 C++ 프로젝트에 사용되는 CMake를 사용한다. )
3. Qt Creator 사용법 ( Visual Studio는 Windows에서만 사용 가능해서 모든 플랫폼에서 사용가능한 Qt Creator를 선호한다. )

<br>

### 개발환경
- Windows
- Qt Creator IDE
- C++
- AnyDesk Workstation:

<br>

### 참고 사이트
- [https://www.peak-system.com/PCAN-USB.199.0.html?&L=1](https://www.peak-system.com/PCAN-USB.199.0.html?&L=1)
- [https://forum.peak-system.com/viewtopic.php?t=1931](https://forum.peak-system.com/viewtopic.php?t=1931)

<br>

### 프로젝트 계획
1. PCAN-USB로 k7 simulator CAN data 받기
    1. pcan-usb c++ code 작성 [#2](../../issues/2)
    2. 브레이크 기능 추가 [#6](../../issues/6)
2. UC-win/Road에 CAN data 전달 (CommonAPI vSomeIP) 
    1. Windows에서 CommonAPI 세팅 [#3](../../issues/3)
    2. UC-win/Road 와 C++ 코드 간 데이터 송수신 ( CommonAPI vSomeIP ) [#4](../../issues/4)
    3. logging으로 같은 값인지 확인 [#7](../../issues/7)
3. UC-win/Road에서 CAN data로 제어 
    1. C++로 새로 개발 [#5](../../issues/5)
