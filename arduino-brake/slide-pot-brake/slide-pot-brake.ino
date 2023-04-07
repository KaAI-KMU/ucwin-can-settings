#include <mcp_can.h>
#include <SPI.h>

const int potmeterPin = A0;
double potmeterVal = 0;

MCP_CAN CAN0(10);     // Set CS to pin 10
byte data[1] = {0x00};

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

  // send data:  ID = 0x100, Standard CAN Frame, Data length = 1 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 1, data);

  if (sndStat == CAN_OK) {
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  // delay(1000);   // send data per 1000ms
}
