# Arduino Brake
아두이노를 활용해 브레이크 값을 받아온다. CAN shield를 아두이노와 연결하고 시뮬레이터 허브에 연결한다. 그러면 그 값을 PCAN이 읽고 우리에게 CAN data를 한번에 보내주게된다.

## Step 1: Get Brake value using slide sensor Example

아두이노 우노와 slide sensor를 활용해 브레이크 값을 받아온다. 회로도는 다음과 같다.

| Slide pot | Arduino |
| --- | --- |
| VCC | 5V |
| GND | GND |
| DTB | A0 |

```arduino
const int potmeterPin = A0; 
double potmeterVal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  potmeterVal = analogRead(potmeterPin); // range 350 ~ 975
  potmeterVal = abs((potmeterVal - 350) / (975 - 350) - 1);

  if (potmeterVal < 0) {
    potmeterVal = 0;
  } else if (potmeterVal > 1) {
    potmeterVal = 1;
  }
  Serial.println(potmeterVal);
  delay(300);
}
```

## Step 2:  Arduino CAN Shield Settings

지금까지 간단하게 센서를 통해 값을 받아와봤다. 이제 [CAN Shield](http://www.compuzone.co.kr/product/product_detail.htm?ProductNo=480814&go_target=product_list) 세팅을 하고 PCAN에 CAN data를 받는다. 일단 CAN shield와 시뮬레이터를 연결해야한다. 그러기 위해선 2가지 방법이 있는데 

1. CAN H, CAN L 선 이용하기
2. DB9 to DB9 케이블 이용하기

<table>
  <tr>
    <td valign="top"><img src="https://user-images.githubusercontent.com/111988634/230912124-a5a5fd46-6f48-434e-9ace-00bf2ae7b8b9.png"/></td>
    <td valign="top"><img alt="스크린샷 2023-04-08 오전 11 45 33" src="https://user-images.githubusercontent.com/111988634/230912378-8d744adf-dd87-46cb-8227-0cf4abfce6b8.png"></td>
  </tr>
</table>

보다시피 구멍 규격은 DB9으로 똑같지만 CAN-H, CAN-L 위치가 다르기 때문에 그냥 사용하면 데이터 송수신이 불가능하다. 따라서 기존의 DB9 to DB9 케이블의 내부 배선을 바꿨다.

<table>
  <tr>
    <td valign="top"><img src="https://user-images.githubusercontent.com/111988634/230912130-263a446b-d78c-40ef-817d-01899b88dd8c.jpg"/></td>
    <td valign="top"><img alt="스크린샷 2023-04-08 오전 11 45 33" src="https://user-images.githubusercontent.com/111988634/230912722-a9ddb943-b5ea-4914-9729-0660b11565fc.jpg"></td>
  </tr>
</table>

![nse-3828080385220569742-1000000139](https://user-images.githubusercontent.com/111988634/230912819-f5098e95-b364-4320-8645-5dee018696ad.jpg)

이렇게 연결함으로써 이제 PCAN-USB만 컴퓨터에 연결하면 기존의 CAN 값 뿐만아니라 아두이노 값도 받을 수 있게 된다.

위의 코드를 CAN shield에 맞게 수정해준다. 이 코드를 사용하기 위해서는 [mcp_can library](https://github.com/coryjfowler/MCP_CAN_lib)를 다운받아야한다. 아두이노 IDE에서 Tools → Mange Libraries → **mcp_can by coryjfowler** 다운로드

```cpp
#include <mcp_can.h>
#include <SPI.h>

const int potmeterPin = A0;
double potmeterVal = 0;

MCP_CAN CAN0(10);     // Set CS to pin 10
byte data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

void loop() {
  // put your main code here, to run repeatedly:

  potmeterVal = analogRead(potmeterPin); // range 350 ~ 975
  potmeterVal = abs((potmeterVal - 350) / (975 - 350) - 1);

  if (potmeterVal < 0) {
    potmeterVal = 0;
  } else if (potmeterVal > 1) {
    potmeterVal = 1;
  }

  int hexValue = int(potmeterVal * 255);
  String hexString = String(hexValue, HEX);
  if (hexString.length() == 1) {
    hexString = "0" + hexString;
  }
  data[0] = strtol(hexString.c_str(), NULL, 16);

  /*
  // hex to decimal
  int a = data[0];
  double b = double(a) / 255.0;
  Serial.print("data: ");
  Serial.println(b); 
  */

  // send data:  ID = 0x111, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x111, 0, 8, data);

  if (sndStat == CAN_OK) {
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(20);   // send data per 20ms
}
```

<img width="491" alt="캡처 PNG" src="https://user-images.githubusercontent.com/111988634/230912143-ec74bbd0-d01d-40cc-81ce-260f38dee00c.png">
위 코드를 실행하면 111h란 id로 PCAN-View에서 확인할 수 있다