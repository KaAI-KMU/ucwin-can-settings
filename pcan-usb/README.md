# PCAN-USB
이제 PEAK 사가 제공하는 API를 활용해서 CAN data를 c++, python 등 여러 언어로 받을 수 있다.

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

## Step 3: Create new project for PCAN-Basic

샘플코드를 이용해서 새로운 프로젝트를 생성한다. 이를 위해서는 api를 프로젝트에 불러와야한다. 먼저 새로운 빈 프로젝트를 생성한다.

pcan-basic/x64/VC_LIB 폴더 복사 - 자신이 만든 visual studio 프로젝트 디렉토리에 붙여넣기 - visual studio - 솔루션 탐색기(왼쪽사이드) - 프로젝트 우클릭 - 추가 - 기존 항목 - VC_LIB/PCANBasic.lib 추가

## Step 4: Send CAN data

이렇게 새로만든 프로젝트에 위의 7번 예제를 그대로 사용해서 정상 작동하는 지 확인한다. 간단하게 코드를 복사 붙여넣기 해서 돌려보면 똑같이 실행된다.

~~k7 엑셀표를 보면서 parsing을 진행한다. 기존에 작성한 python 코드가 있어서 그것을 참고해 c++로 포팅하는 작업만 진행했다. 문제는 parsing data가 uc-winroad의 값 범위와 달라서 필터링을 해야한다. throttle 값은 0~1 사이, steering 값은 -1 ~ 1 사이인데 기존의 코드를 사용하면 이 값 안에 들어가지 않는다. 추가 parsing은 [Windows Socket Programming Example](../socket-programming/) 에서 진행했다.~~

기존 선배님들이 짠 코드들을 그대로 사용한 결과 이상한 값이 나와서 새로 다시 짰다. parser는 이 다음인 [Windows Socket Programming Example](../socket-programming/) 에서 진행한다. 이곳에서는 간단하게 데이터를 제대로 받는 지 확인만 한다.