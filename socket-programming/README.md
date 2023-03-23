# Socket Programming

## Step 1: Write Client/Server Class Example

[위 사이트](https://kevinthegrey.tistory.com/26)를 참고해서 Client/Server class 를 c++ 스타일로 작성했다. 프로젝트를 생성한 뒤 [inet_addr 에러](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=luckywjd7&logNo=220872794096)를 방지하기 위해 설정을 해준다.

클라이언트 우클릭 - 속성 - 구성 속성 - C/C++ - 일반 - SDL 검사 - ‘아니오’ 설정

실행하면 다음과 같은 콘솔창이 나온다.

![Untitled (1)](https://user-images.githubusercontent.com/111988634/227132740-5998e6f0-e07b-4a3a-9c75-c5b83686cf4c.png)

(현재 코드와 다름)

## Step 2: Integrate PCAN-Basic API code and Server

데이터를 받는 pcan-basic 코드가 server가 되서 여러 client 어플리케이션에 데이터를 보내주는 역할을 한다. 따라서 기존의 ThreadRead 코드 안에 Server 클래스 기능을 넣어서 사용한다.

Server에서 CAN data를 받은 뒤 Client가 Server에 data를 요청한다.
Server는 CAN data를 Client에 보내고 (원본) Client에서 parser를 통해 data를 해석한다.

코드를 짤 때 기존의 코드 구조를 바꿨다

| 기존 | 현재 |
| --- | --- |
| CAN 데이터를 string으로 변환 후 client(ucwin)에 전달 | CAN 데이터, id 자체(raw data)를 전달 |
| steering, throttle, brake 등 데이터 각각 socket을 생성 | CAN 데이터 자체를 보내기 때문에 socket 1개만 생성 |
| Delphi - python 간의 socket 통신 | C++ - C++ 간의 socket 통신 |

또한 uc-win/road 상에 socket을 열어서 받는 것이 아닌 class 내에 thread을 열어서 직접 CAN 데이터를 받게 할 수 있지만 그렇게 하면 하나의 class의 너무 많은 기능을 넣는 것 같다. 그래서 하나의 class에 socket 통신 기능을 넣고 받은 데이터를 parsing하는 기능만 넣었다. 

[PCAN-Basic API](../pcan-usb/) 에서 작성한 parsing code를 수정해서 사용했다. 여기에 추가로 uc-win/road에서 사용할 수 있도록 범위를 조정했다.