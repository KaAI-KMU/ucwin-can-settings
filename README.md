# ucwin-can-settings
UC-win/Road 상에서 운전 시뮬레이터를 사용하기 위한 CAN 세팅

<br>

## 프로젝트 개요

![ezgif com-optimize](https://user-images.githubusercontent.com/111988634/230921855-0621f4a7-a0b4-483a-8c69-8fb91a1ea1e8.gif)

기존의 Delphi로 작성된 UC-win/Road 코드를 C++로 변경하는 첫 프로젝트이다. 또한 기존의 CAN 코드들이 인수인계가 제대로 되지 않고 유지보수가 되지 않아 github를 활용해 새롭게 정리하려고 한다.

PCAN-USB를 사용해서 CAN data를 받는다. 그 이후 UDP/IP Socket 프로그래밍을 이용해 데이터를 UC-win/Road 어플리케이션으로 보낸다. UC-win/Road에서는 그 값들을 이용해 차량을 제어하는 코드를 짠다.

이번 프로젝트를 배우면 다음과 같은 기술을 배울 수 있을 것으로 예상한다.

1. **C++을 공부할 수 있다**: 현재 KaAI에서 C++로 진행하는 프로젝트가 하나도 없다. 현업에서는 어디에서도 많이 사용되므로 꼭 알아야한다.
2. **Socket Programming**: UDP/IP Socket Programming을 C++로 구현한다.
3. **CAN 통신**: 차량 내 장치들이 서로 통신할 수 있게 만든 통신 규격이다.
4. **UC-win/Road C++ SDK**: 기존의 Delphi 코드로 작성된 기능을 C++로 전환하면서 새로운 언어를 배워야 한다는 부담을 줄일 수 있다.
5. **Github 사용법**: 이후 누군가 이 코드를 사용 시 오류가 있을때 issue를 생성해 도움을 받을 수 있을 것이다.
6. **Arduino**: 간단한 아두이노 사용법

<br>

## 개발환경
- Windows
- Visual Studio IDE
- C++
- Arduino

<br>

## 프로젝트 개요
1. [Simualtor setting](/simulator-setting/) 
3. [Arduino CAN Shield를 통한 브레이크값 받기](/arduino-brake/)
2. [PCAN-USB API C++](/pcan-usb/)
4. [Windows UDP socket programming](/socket-programming/)
5. [UC-win/Road C++ API](/ucwin-cpp/)