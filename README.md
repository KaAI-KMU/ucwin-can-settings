# ucwin-can-settings
UC-win/Road 상에서 운전 시뮬레이터 사용하기 위한 CAN 세팅

기존의 Delphi로 작성된 코드를 C++로 변경하는 첫 프로젝트이다. 
동시에 기존의 파편화된 CAN 프로그램을 C++로 작성해서 통일성을 높이고 재사용성을 높일 수 있다.

<br>

### 개발환경

- Windows
- Visual Studio IDE
- C++17
- AnyDesk Workstation:

<br>

### 참고 사이트

- [https://www.peak-system.com/PCAN-USB.199.0.html?&L=1](https://www.peak-system.com/PCAN-USB.199.0.html?&L=1)
- [https://forum.peak-system.com/viewtopic.php?t=1931](https://forum.peak-system.com/viewtopic.php?t=1931)
- [https://biji-jjigae.tistory.com/48](https://biji-jjigae.tistory.com/48)

<br>

### 프로젝트 계획

1. PCAN-USB로 k7 simulator CAN data 받기
    1. pcan-usb c++ code 작성 [#1](../../issues/1)
    2. 브레이크 기능 추가
2. UC-win/Road에 CAN data 전달 (CommonAPI vSomeIP) 
    1. Windows에서 CommonAPI 세팅 [#2](../../issues/2)
    2. UC-win/Road 와 C++ 코드 간 데이터 송수신 ( CommonAPI vSomeIP ) [#3](../../issues/3)
    3. logging으로 같은 값인지 확인
3. UC-win/Road에서 CAN data로 제어 
    1. C++로 새로 개발 [#4](../../issues/4)