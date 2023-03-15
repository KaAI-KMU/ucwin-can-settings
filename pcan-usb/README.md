# PCAN-USB
PCAN-USB의 C++ 예제를 실행해보고 시뮬레이터 CAN 데이터를 parsing 한다.

## Step 1: Download PCAN-Basic API

PCAN-USB 사이트 - Downloads - PCAN-Basic API 다운로드 - pcan-basic 압축 해제

## Step 2: Execute Example Codes

pcan-basic - Samples - Samples - Console - NativeC++ - 01 ~ 09 프로젝트 실행해보기

## Step 3: Include DLL in Visual Studio

pcan-basic/x64/VC_LIB 폴더 복사 - 자신이 만든 visual studio 프로젝트 디렉토리에 붙여넣기 - visual studio - 프로젝트 우클릭 - 추가 - 기존 항목 - VC_LIB/PCANBasic.lib 추가

[PCANBasic.dll 과 PCANBasic.lib를 불러오기](https://biji-jjigae.tistory.com/48) 위해 프로젝트에 불러온다.

## Step 4: Get CAN data using 07_ThreadRead Example

7번 예제를 조금씩 변형시켜서 코드를 작성한다.
기존의 코드와 다르게 데이터를 계속 받을 수 있게 작성해준다. 이 코드에 socket 기능도 넣을 예정이다. 다른 어플리케이션에 CAN 데이터를 보내주므로 service의 역할을 하고 UC-win/Road는 Client 역할을 하게 될 것이다.