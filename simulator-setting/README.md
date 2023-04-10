# Simulator Setting
시뮬레이터 k7을 켜는 방법과 PCAN-USB 값을 GUI로 확인한다.

- 고다현 학우가 만든 [Simulator User Manual (고다현)](https://www.notion.so/Simulator-User-Manual-c31ad156434e4074bc4b9c05aa45604b)

CAN 업체는 NI, kvaser, PEAK 등 다양하지만 이번 프로젝트에서는 PEAK 사의 PCAN-USB를 사용하기로 했다. PCAN-USB는 다음과 같이 생겼고 중간에 빨간색 LED가 있는데 이것이 깜빡거리면 데이터가 송수신 중이라는 뜻이다.



<table>
  <tr>
    <td valign="top"><img src="https://user-images.githubusercontent.com/111988634/230911049-4aa538a8-aa6c-44fa-9542-610e35985504.png"/></td>
    <td valign="top"><img alt="스크린샷 2023-04-08 오전 11 45 33" src="https://user-images.githubusercontent.com/111988634/230911063-13f28ac1-a18f-4d6e-9b33-32afea586e6c.png"></td>
  </tr>
</table>

## Step 1: Download Driver

[PCAN-USB 사이트](https://www.peak-system.com/PCAN-USB.199.0.html?&L=1) - Downloads - Device driver setup for Windows -  PEAK-System_Driver-Setup 압축 해제 - PeakOemDrv 실행

## Step 2: PCAN-VIEW

[PCAN-VIEW 사이트](https://www.peak-system.com/PCAN-View.242.0.html?&L=1) - Downloads - PCAN-VIEW - pcanview 압축 해제 - PcanView 실행

## Step 3: Check Availablity

다음 유튜브를 참고해서 CAN 값이 들어오는 지 확인한다.

[PCAN-View 5 Youtube](https://www.youtube.com/playlist?list=PL1QtxAvQuMOhffi7ouhzOufqseRBHN-Vr)

![Untitled](https://user-images.githubusercontent.com/111988634/227128170-c71c075b-47dd-4ae5-b346-bdfa9958f651.png)

Status BUSHEAVY는 오류 메시지가 누적되면 뜬다고 하는데 시뮬레이터에서 어떻게 해결하는 지는 모르겠다.