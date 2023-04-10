#include "ThreadRead.h"

ThreadRead::ThreadRead()
{ 
	/// Window socket initialization
	/// 서버를 시작하는 단계
	InitializeWinsock();
	CreateSock();
	SetServer();
	BindSock();
	AcceptConnection();

	ShowConfigurationHelp(); // Shows information about this sample
	ShowCurrentConfiguration(); // Shows the current parameters configuration

	TPCANStatus stsResult;
	// Initialization of the selected channel
	if (IsFD)
		stsResult = CAN_InitializeFD(PcanHandle, BitrateFD);
	else
		stsResult = CAN_Initialize(PcanHandle, Bitrate);

	if (stsResult != PCAN_ERROR_OK)
	{
		std::cout << "Can not initialize. Please check the defines in the code." << std::endl;
		ShowStatus(stsResult);
		std::cout << std::endl;
		std::cout << "Closing..." << std::endl;
		system("PAUSE");
		return;
	}

	// Reading messages...
	std::cout << "Successfully initialized." << std::endl;
	m_ThreadRun = true;
	m_ReadThread = new std::thread(&ThreadRead::ThreadExecute, this);
	std::cout << "Started reading messages..." << std::endl;
	std::cout << std::endl;
	std::cout << "Closing..." << std::endl;
	system("PAUSE");
}

ThreadRead::~ThreadRead()
{
	// Close sockets and clean up Winsock
	closesocket(newSock);
	closesocket(sock);
	WSACleanup();

	// Close thread
	m_ThreadRun = false;
	m_ReadThread->join();
	delete m_ReadThread;
	CAN_Uninitialize(PCAN_NONEBUS);
}

void ThreadRead::ThreadExecute()
{
	while (m_ThreadRun)
	{
		// Sleep(1); //Use Sleep to reduce the CPU load  
		ReadMessages();
	}
}

void ThreadRead::ReadMessages()
{
	TPCANStatus stsResult;

	// We read at least one time the queue looking for messages. If a message is found, we look again trying to 
	// find more. If the queue is empty or an error occurr, we get out from the dowhile statement.
	do
	{
		stsResult = IsFD ? ReadMessageFD() : ReadMessage();
		if (stsResult != PCAN_ERROR_OK && stsResult != PCAN_ERROR_QRCVEMPTY)
		{
			ShowStatus(stsResult);
			return;
		}
	} while (!(stsResult & PCAN_ERROR_QRCVEMPTY));
}

TPCANStatus ThreadRead::ReadMessageFD()
{
	TPCANMsgFD CANMsg;
	TPCANTimestampFD CANTimeStamp;

	// We execute the "Read" function of the PCANBasic    
	TPCANStatus stsResult = CAN_ReadFD(PcanHandle, &CANMsg, &CANTimeStamp);
	if (stsResult != PCAN_ERROR_QRCVEMPTY)
		// We process the received message
		ProcessMessageCanFD(CANMsg, CANTimeStamp);

	return stsResult;
}

TPCANStatus ThreadRead::ReadMessage()
{
	TPCANMsg CANMsg;
	TPCANTimestamp CANTimeStamp;

	// We execute the "Read" function of the PCANBasic   
	TPCANStatus stsResult = CAN_Read(PcanHandle, &CANMsg, &CANTimeStamp);
	if (stsResult != PCAN_ERROR_QRCVEMPTY) {
		// We process the received message
		ValidateId(CANMsg);
		//ProcessMessageCan(CANMsg, CANTimeStamp);
	}

	return stsResult;
}

void ThreadRead::ValidateId(TPCANMsg msg)
{
	const UINT32 id = msg.ID;
	if (id == 0x710 ||
		id == 0x711 ||
		id == 0x111) {
		SendData(msg);
	}
}

void ThreadRead::ProcessMessageCan(TPCANMsg msg, TPCANTimestamp itsTimeStamp)
{
	UINT64 microsTimestamp = ((UINT64)itsTimeStamp.micros + 1000 * (UINT64)itsTimeStamp.millis + 0x100000000 * 1000 * itsTimeStamp.millis_overflow);

	std::cout << "Type: " << GetMsgTypeString(msg.MSGTYPE) << std::endl;
	std::cout << "ID: " << GetIdString(msg.ID, msg.MSGTYPE) << std::endl;
	char result[MAX_PATH] = { 0 };
	sprintf_s(result, sizeof(result), "%i", msg.LEN);
	std::cout << "Length: " << result << std::endl;
	std::cout << "Time: " << GetTimeString(microsTimestamp) << std::endl;
	std::cout << "Data: " << GetDataString(msg.DATA, msg.MSGTYPE, msg.LEN) << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
}

void ThreadRead::ProcessMessageCanFD(TPCANMsgFD msg, TPCANTimestampFD itsTimeStamp)
{
	std::cout << "Type: " << GetMsgTypeString(msg.MSGTYPE) << std::endl;
	std::cout << "ID: " << GetIdString(msg.ID, msg.MSGTYPE) << std::endl;
	std::cout << "Length: " << GetLengthFromDLC(msg.DLC) << std::endl;
	std::cout << "Time: " << GetTimeString(itsTimeStamp) << std::endl;
	std::cout << "Data: " << GetDataString(msg.DATA, msg.MSGTYPE, GetLengthFromDLC(msg.DLC)) << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
}

void ThreadRead::ShowConfigurationHelp()
{
	std::cout << "=========================================================================================" << std::endl;
	std::cout << "|                           PCAN-Basic ThreadRead Example                                |" << std::endl;
	std::cout << "=========================================================================================" << std::endl;
	std::cout << "Following parameters are to be adjusted before launching, according to the hardware used |" << std::endl;
	std::cout << "                                                                                         |" << std::endl;
	std::cout << "* PcanHandle: Numeric value that represents the handle of the PCAN-Basic channel to use. |" << std::endl;
	std::cout << "              See 'PCAN-Handle Definitions' within the documentation                     |" << std::endl;
	std::cout << "* IsFD: Boolean value that indicates the communication mode, CAN (false) or CAN-FD (true)|" << std::endl;
	std::cout << "* Bitrate: Numeric value that represents the BTR0/BR1 bitrate value to be used for CAN   |" << std::endl;
	std::cout << "           communication                                                                 |" << std::endl;
	std::cout << "* BitrateFD: String value that represents the nominal/data bitrate value to be used for  |" << std::endl;
	std::cout << "             CAN-FD communication                                                        |" << std::endl;
	std::cout << "=========================================================================================" << std::endl;
	std::cout << std::endl;
}

void ThreadRead::ShowCurrentConfiguration()
{
	std::cout << "Parameter values used" << std::endl;
	std::cout << "----------------------" << std::endl;
	char buffer[MAX_PATH];
	FormatChannelName(PcanHandle, buffer, IsFD);
	std::cout << "* PCANHandle: " << buffer << std::endl;
	if (IsFD)
		std::cout << "* IsFD: True" << std::endl;
	else
		std::cout << "* IsFD: False" << std::endl;
	ConvertBitrateToString(Bitrate, buffer);
	std::cout << "* Bitrate: " << buffer << std::endl;
	std::cout << "* BitrateFD: " << BitrateFD << std::endl;
	std::cout << std::endl;
}

void ThreadRead::ShowStatus(TPCANStatus status)
{
	std::cout << "=========================================================================================" << std::endl;
	char buffer[MAX_PATH];
	GetFormattedError(status, buffer);
	std::cout << buffer << std::endl;
	std::cout << "=========================================================================================" << std::endl;
}

void ThreadRead::FormatChannelName(TPCANHandle handle, LPSTR buffer, bool isFD)
{
	TPCANDevice devDevice;
	BYTE byChannel;

	// Gets the owner device and channel for a PCAN-Basic handle
	if (handle < 0x100)
	{
		devDevice = (TPCANDevice)(handle >> 4);
		byChannel = (BYTE)(handle & 0xF);
	}
	else
	{
		devDevice = (TPCANDevice)(handle >> 8);
		byChannel = (BYTE)(handle & 0xFF);
	}

	// Constructs the PCAN-Basic Channel name and return it
	char handleBuffer[MAX_PATH];
	GetTPCANHandleName(handle, handleBuffer);
	if (isFD)
		sprintf_s(buffer, MAX_PATH, "%s:FD %d (%Xh)", handleBuffer, byChannel, handle);
	else
		sprintf_s(buffer, MAX_PATH, "%s %d (%Xh)", handleBuffer, byChannel, handle);
}

void ThreadRead::GetTPCANHandleName(TPCANHandle handle, LPSTR buffer)
{
	strcpy_s(buffer, MAX_PATH, "PCAN_NONE");
	switch (handle)
	{
	case PCAN_PCIBUS1:
	case PCAN_PCIBUS2:
	case PCAN_PCIBUS3:
	case PCAN_PCIBUS4:
	case PCAN_PCIBUS5:
	case PCAN_PCIBUS6:
	case PCAN_PCIBUS7:
	case PCAN_PCIBUS8:
	case PCAN_PCIBUS9:
	case PCAN_PCIBUS10:
	case PCAN_PCIBUS11:
	case PCAN_PCIBUS12:
	case PCAN_PCIBUS13:
	case PCAN_PCIBUS14:
	case PCAN_PCIBUS15:
	case PCAN_PCIBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_PCI");
		break;

	case PCAN_USBBUS1:
	case PCAN_USBBUS2:
	case PCAN_USBBUS3:
	case PCAN_USBBUS4:
	case PCAN_USBBUS5:
	case PCAN_USBBUS6:
	case PCAN_USBBUS7:
	case PCAN_USBBUS8:
	case PCAN_USBBUS9:
	case PCAN_USBBUS10:
	case PCAN_USBBUS11:
	case PCAN_USBBUS12:
	case PCAN_USBBUS13:
	case PCAN_USBBUS14:
	case PCAN_USBBUS15:
	case PCAN_USBBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_USB");
		break;

	case PCAN_LANBUS1:
	case PCAN_LANBUS2:
	case PCAN_LANBUS3:
	case PCAN_LANBUS4:
	case PCAN_LANBUS5:
	case PCAN_LANBUS6:
	case PCAN_LANBUS7:
	case PCAN_LANBUS8:
	case PCAN_LANBUS9:
	case PCAN_LANBUS10:
	case PCAN_LANBUS11:
	case PCAN_LANBUS12:
	case PCAN_LANBUS13:
	case PCAN_LANBUS14:
	case PCAN_LANBUS15:
	case PCAN_LANBUS16:
		strcpy_s(buffer, MAX_PATH, "PCAN_LAN");
		break;

	default:
		strcpy_s(buffer, MAX_PATH, "UNKNOWN");
		break;
	}
}

void ThreadRead::GetFormattedError(TPCANStatus error, LPSTR buffer)
{
	// Gets the text using the GetErrorText API function. If the function success, the translated error is returned. 
	// If it fails, a text describing the current error is returned.
	if (CAN_GetErrorText(error, 0x09, buffer) != PCAN_ERROR_OK)
		sprintf_s(buffer, MAX_PATH, "An error occurred. Error-code's text (%Xh) couldn't be retrieved", error);
}

void ThreadRead::ConvertBitrateToString(TPCANBaudrate bitrate, LPSTR buffer)
{
	switch (bitrate)
	{
	case PCAN_BAUD_1M:
		strcpy_s(buffer, MAX_PATH, "1 MBit/sec");
		break;
	case PCAN_BAUD_800K:
		strcpy_s(buffer, MAX_PATH, "800 kBit/sec");
		break;
	case PCAN_BAUD_500K:
		strcpy_s(buffer, MAX_PATH, "500 kBit/sec");
		break;
	case PCAN_BAUD_250K:
		strcpy_s(buffer, MAX_PATH, "250 kBit/sec");
		break;
	case PCAN_BAUD_125K:
		strcpy_s(buffer, MAX_PATH, "125 kBit/sec");
		break;
	case PCAN_BAUD_100K:
		strcpy_s(buffer, MAX_PATH, "100 kBit/sec");
		break;
	case PCAN_BAUD_95K:
		strcpy_s(buffer, MAX_PATH, "95,238 kBit/sec");
		break;
	case PCAN_BAUD_83K:
		strcpy_s(buffer, MAX_PATH, "83,333 kBit/sec");
		break;
	case PCAN_BAUD_50K:
		strcpy_s(buffer, MAX_PATH, "50 kBit/sec");
		break;
	case PCAN_BAUD_47K:
		strcpy_s(buffer, MAX_PATH, "47,619 kBit/sec");
		break;
	case PCAN_BAUD_33K:
		strcpy_s(buffer, MAX_PATH, "33,333 kBit/sec");
		break;
	case PCAN_BAUD_20K:
		strcpy_s(buffer, MAX_PATH, "20 kBit/sec");
		break;
	case PCAN_BAUD_10K:
		strcpy_s(buffer, MAX_PATH, "10 kBit/sec");
		break;
	case PCAN_BAUD_5K:
		strcpy_s(buffer, MAX_PATH, "5 kBit/sec");
		break;
	default:
		strcpy_s(buffer, MAX_PATH, "Unknown Bitrate");
		break;
	}
}

std::string ThreadRead::GetMsgTypeString(TPCANMessageType msgType)
{
	if ((msgType & PCAN_MESSAGE_STATUS) == PCAN_MESSAGE_STATUS)
		return "STATUS";

	if ((msgType & PCAN_MESSAGE_ERRFRAME) == PCAN_MESSAGE_ERRFRAME)
		return "ERROR";

	std::string strTemp;
	if ((msgType & PCAN_MESSAGE_EXTENDED) == PCAN_MESSAGE_EXTENDED)
		strTemp = "EXT";
	else
		strTemp = "STD";

	if ((msgType & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		strTemp = (strTemp + "/RTR");
	else
		if (msgType > PCAN_MESSAGE_EXTENDED)
		{
			strTemp = (strTemp + " [ ");
			if (msgType & PCAN_MESSAGE_FD)
				strTemp = (strTemp + " FD");
			if (msgType & PCAN_MESSAGE_BRS)
				strTemp = (strTemp + " BRS");
			if (msgType & PCAN_MESSAGE_ESI)
				strTemp = (strTemp + " ESI");
			strTemp = (strTemp + " ]");
		}

	return strTemp;
}

std::string ThreadRead::GetIdString(UINT32 id, TPCANMessageType msgType)
{
	char result[MAX_PATH] = { 0 };
	if ((msgType & PCAN_MESSAGE_EXTENDED) == PCAN_MESSAGE_EXTENDED)
	{
		sprintf_s(result, sizeof(result), "%08Xh", id);
		return result;
	}
	sprintf_s(result, sizeof(result), "%03X", id);
	return result;
}

int ThreadRead::GetLengthFromDLC(BYTE dlc)
{
	switch (dlc)
	{
	case 9: return 12;
	case 10: return 16;
	case 11: return 20;
	case 12: return 24;
	case 13: return 32;
	case 14: return 48;
	case 15: return 64;
	default: return dlc;
	}
}

std::string ThreadRead::GetTimeString(TPCANTimestampFD time)
{
	char result[MAX_PATH] = { 0 };
	double fTime = (time / 1000.0);
	sprintf_s(result, sizeof(result), "%.1f", fTime);
	return result;
}

std::string ThreadRead::GetDataString(BYTE data[], TPCANMessageType msgType, int dataLength)
{
	if ((msgType & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		return "Remote Request";
	else
	{
		char strTemp[MAX_PATH] = { 0 };
		std::string result = "";
		for (int i = 0; i < dataLength; i++)
		{
			sprintf_s(strTemp, sizeof(strTemp), "%02X ", data[i]);
			result.append(strTemp);
		}

		return result;
	}
}

void ThreadRead::InitializeWinsock()
{
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ThreadRead::CreateSock()
{
	// Create a socket
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ThreadRead::SetServer()
{
	// Set server information
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port); // Set to desired port number
}

void ThreadRead::BindSock()
{
	// Bind socket to IP address and port
	if (bind(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		std::cerr << "Binding failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	listen(sock, SOMAXCONN);
}


void ThreadRead::AcceptConnection()
{
	// Accept incoming connection
	c = sizeof(sockaddr_in);
	if ((newSock = accept(sock, (sockaddr*)&client, &c)) == INVALID_SOCKET) {
		std::cerr << "Accepting connection failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ThreadRead::ReceiveData()
{
	// Receive data from client
	char buffer[1024] = { 0 };
	if (recv(newSock, buffer, 1024, 0) < 0) {
		std::cerr << "Receiving data failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Client message: " << buffer << std::endl;
}

void ThreadRead::SendData(TPCANMsg msg)
{
	/// Send data to client
	/// buffer = Data + ID + Length
	const size_t bufferSize = sizeof(msg.DATA) + sizeof(msg.ID);
	char buffer[bufferSize];
	memcpy(buffer, &msg.DATA, sizeof(msg.DATA));
	memcpy(buffer + sizeof(msg.DATA), &msg.ID, sizeof(msg.ID));
	std::cout << "Server: " << GetIdString(msg.ID, msg.MSGTYPE) << " " << GetDataString(msg.DATA, msg.MSGTYPE, msg.LEN) << std::endl;
	if (send(newSock, buffer, bufferSize, 0) < 0) {
		std::cerr << "Sending data failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}