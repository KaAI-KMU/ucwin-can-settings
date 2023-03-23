# UC-win/Road C++

## Step 1: Download UC-win/Road x64 and SDK

사용버전

- UC-win/Road Data 16.0
- UC-win/Road SDK 16.0.3

KaAI 이메일로 forum8 사이트 접속 - 최신 버전 data 및 sdk 다운 - 각각 압축해제 - installer 실행

## Step 2: UC-win/Road C++ API setting

C:\UCwinRoad_SDK 16.0.3\CPP\Doc\en 에 있는 APIIntroduction을 참고해서 프로젝트 세팅을 진행한다. 영어로 적혀있지만 상세하게 적혀있고 Visual Studio 2019 버전 기준이기 때문에 2022 버전과 다른 부분이 없어 어렵지 않게 따라할 수 있다.

## Step 3: Link CAN Socket with UC-win/Road
비주얼 스튜디오 솔루션에 이전에 작성했던 can-socket-server를 연결해서 실행할 때 동시에 uc-win/road와 can server를 실행하도록 세팅한다.(일일이 실행하는 것보다 훨씬 간편함)
솔루션 우클릭 - 추가 - 기존 프로젝트 - can-socket-server 선택
솔루션 우클릭 - 속성 - 공용 속성 - 시작 프로젝트 - 여러 개의 시작 프로젝트

이후에 can-socket-client 코드를 uc-win/road c++ 코드에 포팅한다. 같은 c++이라 쉽게 넣을 수 있다. 

uc-win/road c++ api에서 내가 작성한 함수를 사용하는 방법 중 권장하는 방법은 callback에 등록하는 것이다. 버튼 클릭 콜백에 내가 만든 함수를 등록하면 여러 함수를 동시에 실행할 수 있는 기능을 제공하고 있다. 만약 callback에 등록하지 않고 바로 사용하면 프로그램이 새로운 thread를 열지 않고 while문을 돌기 때문에 귀찮더라도 callback을 사용해야한다.

## Step 4: Parsing CAN Data

CAN 데이터를 parsing해서 사람이 보기 쉽게 double값으로 변환해준다. [Windows Socket Programming Example](../socket-programming/can-socket/can-socket-client/Client.cpp) 에서 작성한 parsing code를 그대로 사용한다.

## Step 5: Contorl Car By CAN Data

바뀐 CAN값으로 uc-win/road 상의 차량을 구동한다. 아직 브레이크를 세팅하지 않아서 steering과 throttle만 사용가능하다. SampleOverrideCarInput 예제를 참조하면 쉽게 구현할 수 있다.