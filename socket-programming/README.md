# Socket Programming
기존에 TCP로 작성했지만 uc-win/road와 연동하면서 문제점을 발견했다. 따라서 TCP/IP가 아닌 UDP를 사용해서 CAN data를 받아오기로 했다. TCP와 UDP의 차이점을 공부하면 왜 UDP를 사용한 지 알 수 있을 것이다.
<details>
<summary>deprecated TCP/IP</summary>

## Step 1: Write Client/Server Class Example

[위 사이트](https://kevinthegrey.tistory.com/26)를 참고해서 Client/Server class 를 c++ 스타일로 작성했다. 프로젝트를 생성한 뒤 [inet_addr 에러](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=luckywjd7&logNo=220872794096)를 방지하기 위해 설정을 해준다.

클라이언트 우클릭 - 속성 - 구성 속성 - C/C++ - 일반 - SDL 검사 - ‘아니오’ 설정

실행하면 다음과 같은 콘솔창이 나온다.

- [Server.cpp](/socket-programming/tcpip/socket-example/socket-example/Server.cpp)
- [Client.cpp](/socket-programming/tcpip/socket-example/socket-example-client/Client.cpp)

![Untitled (1)](https://user-images.githubusercontent.com/111988634/227132740-5998e6f0-e07b-4a3a-9c75-c5b83686cf4c.png)

(현재 코드와 다름)

## Step 2: Integrate PCAN-Basic API code and Server

데이터를 받는 pcan-basic 코드가 server가 되서 여러 client 어플리케이션에 데이터를 보내주는 역할을 한다. 따라서 기존의 ThreadRead 코드 안에 Server 클래스 기능을 넣어서 사용한다.

Server에서 CAN data를 받은 뒤 Client가 Server에 data를 요청한다.
Server는 CAN data를 Client에 보내고 (원본) Client에서 parser를 통해 data를 해석한다.
- [ThreadRead.cpp](/socket-programming/tcpip/can-socket/can-socket-server/ThreadRead.cpp)
- [ThreadRead.h](/socket-programming/tcpip/can-socket/can-socket-server/ThreadRead.h)
- [Client.cpp](/socket-programming/tcpip/can-socket/can-socket-client/Client.cpp)
- [Client.h](/socket-programming/tcpip/can-socket/can-socket-client/Client.h)
</details>

</br>

코드를 짤 때 기존의 코드 구조를 바꿨다

| 기존 | 현재 |
| --- | --- |
| CAN 데이터를 string으로 변환 후 client(ucwin)에 전달 | CAN 데이터, id 자체(raw data)를 전달 |
| steering, throttle, brake 등 데이터 각각 socket을 생성 | CAN 데이터 자체를 보내기 때문에 socket 1개만 생성 |
| Delphi - python 간의 socket 통신 | C++ - C++ 간의 socket 통신 |

또한 uc-win/road 상에 socket을 열어서 받는 것이 아닌 class 내에 thread을 열어서 직접 CAN 데이터를 받게 할 수 있지만 그렇게 하면 하나의 class의 너무 많은 기능을 넣는 것 같다. 그래서 하나의 class에 socket 통신 기능을 넣고 받은 데이터를 parsing하는 기능만 넣었다. 

[PCAN-Basic API](../pcan-usb/) 에서 작성한 parsing code를 수정해서 사용했다. 여기에 추가로 uc-win/road에서 사용할 수 있도록 범위를 조정했다.

</br>

## Step 3: Parsing CAN data

[기존의 parser 코드](https://www.notion.so/k7-add-ca6c153c40bb41b1a035b40ef61c2d07)가 ucwin/road와는 fit하지 않아 처음부터 다시 짰다. 

일단 이 프로젝트의 목적은 steering, throttle, brake 이기 때문에 나머지 기능들은 parsing 하지 않았다. 하지만 엑셀표와 다음 코드를 참고하면 다른 것도 충분히 구현가능하다.

값들이 들쭉날쭉하기 때문에 코드를 돌려보면 다를 수 있다.

- [UDPCANReceiver.cpp](/socket-programming/udpip/udp-pcan/UDPCANReceiver/UDPCANReceiver.cpp)

```cpp
void UDPCANReceiver::Parser111(char buffer[])
{
	unsigned char byte1 = static_cast<unsigned char>(buffer[0] & 0xFF);
	double decimalValue = double(byte1) / 255.0;
	mBrake = decimalValue;
	//std::cout << "mBrake: " << decimalValue << std::endl;
}

void UDPCANReceiver::Parser710(char buffer[])
{
	// 0 ~ 255
	unsigned char byte1 = static_cast<unsigned char>(buffer[1] & 0xFF);
	unsigned char byte2 = static_cast<unsigned char>(buffer[2] & 0xFF);

	// Steering값 하나로 합치기
	// unsigned short range: 0 ~ 65535
	unsigned short value = (byte2 << 8) | byte1;
	//std::cout << "value: " << value << std::endl;

	/// byte1이 0~255까지 돌면 byte2가 1 증가
	/// +-90도 기준으로 2700 ~ 840
	/// 중간값은 1770
	/// 값은 실험할때마다 달라지므로 사용시기에 따라 달라질 수 있음 (2023.04.05 기준 정문규)
	/// uc-win에서 steering은 -1 에서 1 사이의 값만 받기 때문에 변경
	/// 840 ~ 2700 -> -1 ~ 1
	/// 왼쪽을 -1, 오른쪽을 1로 설정하기 때문에 마지막 mSteering에 -1 추가
	const unsigned short middle = 1770;
	const unsigned short max = 2700;
	const unsigned short diff = max - middle;
	if (value > 2700) {
		mSteering = -1.0;
	}
	else if (value < 840) {
		mSteering = 1.0;
	}
	else {
		mSteering = -(static_cast<double>(value) - middle) / diff;
	}
	// std::cout << "steering: " << mSteering << std::endl;
}

void UDPCANReceiver::Parser711(char buffer[])
{
	unsigned char byte1 = static_cast<unsigned char>(buffer[5] & 0xFF);
	unsigned char byte2 = static_cast<unsigned char>(buffer[6] & 0xFF);

	// Throttle값 하나로 합치기
	// unsigned short range: 0 ~ 65535
	unsigned short value = (byte2 << 8) | byte1;
	// std::cout << "value: " << value << std::endl;

	/// Throttle 범위가 들쭉날쭉하기 때문에 현재 기준으로 작성 (2023.04.03 기준 정문규)
	/// 620 ~ 3480 이 범위 밖은 0 또는 1 처리
	/// steering과 마찬가지로 0 ~ 1로 변경
	const unsigned short min = 620;
	const unsigned short max = 3480;
	const unsigned short diff = max - min;
	if (value < 620)
	{
		mThrottle = 0;
	}
	else if (value > 3480)
	{
		mThrottle = 1;
	}
	else {
		mThrottle = (static_cast<double>(value) - min) / diff;
	}
	//std::cout << "mThrottle: " << mThrottle << std::endl;
}
```