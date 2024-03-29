#include "ThreadRead.h"

ThreadRead::ThreadRead()
{
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
		//ValidateId(CANMsg);
		ProcessMessageCan(CANMsg, CANTimeStamp);
	}

	return stsResult;
}

void ThreadRead::ValidateId(TPCANMsg msg)
{
	std::cout << "==================" << std::endl;
	//if (msg.ID == ClusterID) Parcing50(msg);
	//else if (msg.ID == AGMID) Parcing631(msg);
	if (msg.ID == APMID) Parcing710(msg);
	else if (msg.ID == ASMID) Parcing711(msg);
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

void ThreadRead::Parcing50(TPCANMsg msg)
{
	const UINT8 lamp = msg.DATA[1] & 0x0F;
	const UINT8 wiper = msg.DATA[1] & 0xF0;
	const UINT8 wiperOn = msg.DATA[2] & 0x0F;
	const UINT8 lampOn = msg.DATA[2] & 0xF0;

	std::cout << "lamp: " << lamp << std::endl;
	switch (lamp)
	{
	case 0x00:
		std::cout << "Tail lamp off" << std::endl;
		break;
	case 0x01:
		std::cout << "Tail lamp on" << std::endl;
		break;
	case 0x02:
		std::cout << "High beam on" << std::endl;
		break;
	default:
		std::cout << "None" << std::endl;
	}

	std::cout << "wiper: " << wiper << std::endl;
	switch (wiper) 
	{
	case 0x20:
		std::cout << "Wiper off" << std::endl;
		break;
	case 0x40:
		std::cout << "Wipe Auto" << std::endl;
		break;
	case 0x60:
		std::cout << "Wiper Low" << std::endl;
		break;
	case 0x80:
		std::cout << "Wiper High" << std::endl;
		break;
	default:
		std::cout << "None" << std::endl;
	}

	std::cout << "wiper_on: " << wiperOn << std::endl;
	switch (wiperOn)
	{
	case 0x01:
		std::cout << "Wiper Trigger" << std::endl;
		break;
	case 0x08:
		std::cout << "Wiper Mist" << std::endl;
		break;
	default:
		std::cout << "None" << std::endl;
	}

	std::cout << "lamp_on: " << lampOn << std::endl;
	switch (lampOn)
	{
	case 0x10:
		std::cout << "Turn right lamp on" << std::endl;
		break;
	case 0x20:
		std::cout << "Turn left lamp on" << std::endl;
		break;
	case 0x40:
		std::cout << "High beam Hold" << std::endl;
		break;
	default:
		std::cout << "None" << std::endl;
	}
}

void ThreadRead::Parcing631(TPCANMsg msg)
{
	const UINT8 status = msg.DATA[0] & 0x0F;
	const UINT8 reserved = msg.DATA[0] & 0xF0;
	const UINT8 TM_G_SEL_DISP = msg.DATA[1] & 0x0F;

	switch (TM_G_SEL_DISP)
	{
	case 0x00:
		std::cout << "Parking" << std::endl;
		break;
	case 0x05:
		std::cout << "Drive" << std::endl;
		break;
	case 0x06:
		std::cout << "Rear" << std::endl;
		break;
	case 0x07:
		std::cout << "Nutral" << std::endl;
		break;
	default:
		std::cout << "None 631" << std::endl;
	}
}

void ThreadRead::Parcing710(TPCANMsg msg)
{
	/*
	const UINT8 mode = msg.DATA[0] & 0x01;
	const UINT8 modeFeedback = msg.DATA[0] & 0x70;
	const UINT8 errorStatus = msg.DATA[0] & 0xF0;
	switch (mode)
	{
	case 0x00:
		std::cout << "Manual 710" << std::endl;
		break;
	case 0x01:
		std::cout << "Auto 710" << std::endl;
		break;
	default:
		std::cout << "None mode 710" << std::endl;
	}

	switch (modeFeedback)
	{
	case 0x00:
		std::cout << "Abnormal state 710" << std::endl;
		break;
	case 0x02:
		std::cout << "Initial state 710" << std::endl;
		break;
	case 0x04:
		std::cout << "Operating state 710" << std::endl;
		break;
	case 0x08:
		std::cout << "Overriding error 710" << std::endl;
		break;
	default:
		std::cout << "None modeFeedback 710" << std::endl;
	}

	switch (errorStatus)
	{
	case 0x00: 
		std::cout << "None errorStatus 710" << std::endl;
		break;
	case 0x10:
		std::cout << "Command Controller Coummunication Error 710" << std::endl;
		break;
	case 0x20:
		std::cout << "MDPS Error 710" << std::endl;
		break;
	case 0x40:
		std::cout << "SAPS Error 710" << std::endl;
		break;
	default:
		std::cout << "error errorStatus 710" << std::endl;
	}
	*/
	int data = msg.DATA[1] + msg.DATA[2] * 256;
	double steeringAngleFeedback = 0;
	if (data < 1000) 
		steeringAngleFeedback = data / 10;
	else {
		data = data ^ 0b1111111111111111;
		steeringAngleFeedback = ~data / 10;
	}
	std::cout << "steering angle feedback 710: " << steeringAngleFeedback << std::endl;

	/*
	int desiredData = msg.DATA[3] + msg.DATA[4] * 256;
	double desiredSteeringAngle = 0;
	if (desiredData < 1000)
		desiredSteeringAngle = desiredData / 10;
	else {
		desiredData = data ^ 0b1111111111111111;
		desiredSteeringAngle = ~desiredData / 10;
	}
	std::cout << "desired steering angle 710: " << desiredSteeringAngle << std::endl;
	*/
}

void ThreadRead::Parcing711(TPCANMsg msg)
{
	const UINT8 accel = msg.DATA[5] + msg.DATA[6] * 256;
	std::cout << "accel: " << (int)accel << std::endl;

	/*
	const UINT8 mode = msg.DATA[0] & 0x01;
	const UINT8 modeFeedback = msg.DATA[0] & 0x0E;
	const UINT8 errorStatus = msg.DATA[0] & 0xF0;

	switch (mode)
	{
	case 0x00:
		std::cout << "Manual 711" << std::endl;
		break;
	case 0x01:
		std::cout << "Auto 711" << std::endl;
		break;
	default:
		std::cout << "None 711" << std::endl;
	}

	switch (modeFeedback)
	{
	case 0x00:
		std::cout << "Abnormal state 711" << std::endl;
		break;
	case 0x02:
		std::cout << "Initial state 711" << std::endl;
		break;
	case 0x04:
		std::cout << "Operating state 711" << std::endl;
		break;
	default:
		std::cout << "None 711" << std::endl;
	}

	switch (errorStatus)
	{
	case 0x00:
		std::cout << "None errorStatus 711" << std::endl;
		break;
	case 0x10:
		std::cout << "Command Controller Coummunication Error 711" << std::endl;
		break;
	case 0x20:
		std::cout << "Brake actuator Error 711" << std::endl;
		break;
	case 0x40:
		std::cout << "APS Error 711" << std::endl;
		break;
	case 0x80:
		std::cout << "OBD-II or GW Error 711" << std::endl;
		break;
	default:
		std::cout << "error errorStatus 711" << std::endl;
	}

	const UINT8 brakePedal = msg.DATA[4];
	switch (brakePedal)
	{
	case 0x00:
		std::cout << "Brake pedal: off 711" << std::endl;
		break;
	case 0xFF:
		std::cout << "Brake pedal: on 711" << std::endl;
		break;
	default:
		std::cout << "error brakePedal 711" << std::endl;
	}
	*/
}