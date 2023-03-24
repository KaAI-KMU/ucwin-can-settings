# PCAN-USB
PCAN-USB의 C++ 예제를 실행해보고 시뮬레이터 CAN 데이터를 parsing 한다.

## Step 1: Download PCAN-Basic API

PCAN-USB 사이트 - Downloads - PCAN-Basic API 다운로드 - pcan-basic 압축 해제

## Step 2: Execute 07_ThreadRead Example

pcan-basic - Samples - Samples - Console - NativeC++ - 07 프로젝트 실행해보기

```bash
=========================================================================================
|                           PCAN-Basic ThreadRead Example                                |
=========================================================================================
Following parameters are to be adjusted before launching, according to the hardware used |
                                                                                         |
* PcanHandle: Numeric value that represents the handle of the PCAN-Basic channel to use. |
              See 'PCAN-Handle Definitions' within the documentation                     |
* IsFD: Boolean value that indicates the communication mode, CAN (false) or CAN-FD (true)|
* Bitrate: Numeric value that represents the BTR0/BR1 bitrate value to be used for CAN   |
           communication                                                                 |
* BitrateFD: String value that represents the nominal/data bitrate value to be used for  |
             CAN-FD communication                                                        |
=========================================================================================

Parameter values used
----------------------
* PCANHandle: PCAN_USB 1 (51h)
* IsFD: False
* Bitrate: 500 kBit/sec
* BitrateFD: f_clock_mhz=20, nom_brp=5, nom_tseg1=2, nom_tseg2=1, nom_sjw=1, data_brp=2, data_tseg1=3, data_tseg2=1, data_sjw=1

Successfully initialized.
Started reading messages...

Closing...
Type: STATUS
ID: 001h
Length: 4
Time: 40183384.2
Data: 00 00 00 04
----------------------------------------------------------
Type: STATUS
ID: 001h
Length: 4
Time: 40183385.5
Data: 00 00 00 08
----------------------------------------------------------
=========================================================================================
Bus error: an error counter reached the 'heavy'/'warning' limit
=========================================================================================
계속하려면 아무 키나 누르십시오 . . . Type: STD
ID: 631h
Length: 8
Time: 40183393.4
Data: 02 00 B1 00 00 00 00 B3
----------------------------------------------------------
Type: STD
ID: 710h
Length: 8
Time: 40183394.2
Data: 12 0B 0E 4F 0B FB 07 27
----------------------------------------------------------
```

## Step 3: Include DLL in Visual Studio

pcan-basic/x64/VC_LIB 폴더 복사 - 자신이 만든 visual studio 프로젝트 디렉토리에 붙여넣기 - visual studio - 프로젝트 우클릭 - 추가 - 기존 항목 - VC_LIB/PCANBasic.lib 추가

[PCANBasic.dll 과 PCANBasic.lib를 불러오기](https://biji-jjigae.tistory.com/48) 위해 프로젝트에 불러온다.

## Step 4: Parcing CAN data

7번 코드는 CAN 데이터를 계속 받아주는 코드이다. 하지만 우리에게 필요없는 데이터도 계속 출력해주기 때문에 parser를 만들어 필요한 데이터만 뽑아온다.

1기 선배님들의 코드를 참고해서 필요한 CAN ID가 0x50(cluster), 0x710(APM_Feedback), 0x631(AGM_Feedback), 0x711(ASM_Feedback)으로 총 4가지 경우가 있는 것을 확인했다. 일단 받아올 수 있는 모든 값들을 코드로 작성[ThreadRead.cpp](/pcan-usb/pcan-basic-examaple/pcan-basic-examaple/ThreadRead.cpp)했다.

k7 엑셀표를 보면서 parsing을 진행한다. 기존에 작성한 python 코드가 있어서 그것을 참고해 c++로 포팅하는 작업만 진행했다. 문제는 parsing data가 uc-win/road의 값 범위와 달라서 필터링을 해야한다. throttle 값은 0~1 사이, steering 값은 -1 ~ 1 사이인데 기존의 코드를 사용하면 이 값 안에 들어가지 않는다. 추가 parsing은 [Windows Socket Programming Example](../socket-programming/) 에서 진행했다.