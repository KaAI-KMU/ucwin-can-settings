#include "stdafx.h"
#include "PCANBasic.h"

class UDPCANSender
{
public:
	UDPCANSender();
	~UDPCANSender();

private:
	// PCANBasic func()
	void ThreadExecute();
	void ReadMessages();
	TPCANStatus ReadMessageFD();
	TPCANStatus ReadMessage();
	void ProcessMessageCan(TPCANMsg msg, TPCANTimestamp itsTimeStamp);
	void ProcessMessageCanFD(TPCANMsgFD msg, TPCANTimestampFD itsTimeStamp);
	void ShowConfigurationHelp();
	void ShowCurrentConfiguration();
	void ShowStatus(TPCANStatus status);
	void FormatChannelName(TPCANHandle handle, LPSTR buffer, bool isFD);
	void GetTPCANHandleName(TPCANHandle handle, LPSTR buffer);
	void GetFormattedError(TPCANStatus error, LPSTR buffer);
	void ConvertBitrateToString(TPCANBaudrate bitrate, LPSTR buffer);
	std::string GetMsgTypeString(TPCANMessageType msgType);
	std::string GetIdString(UINT32 id, TPCANMessageType msgType);
	int GetLengthFromDLC(BYTE dlc);
	std::string GetTimeString(TPCANTimestampFD time);
	std::string GetDataString(BYTE data[], TPCANMessageType msgType, int dataLength);

	// Parcing CAN data
	void ValidateId(TPCANMsg msg);

	// UDP func
	void Init();
	void Send(TPCANMsg msg);
	void Close();

	// PCANBasic var
	const TPCANHandle PcanHandle = PCAN_USBBUS1;
	const bool IsFD = false;
	const TPCANBaudrate Bitrate = PCAN_BAUD_500K;
	const TPCANBitrateFD BitrateFD = const_cast<LPSTR>("f_clock_mhz=20, nom_brp=5, nom_tseg1=2, nom_tseg2=1, nom_sjw=1, data_brp=2, data_tseg1=3, data_tseg2=1, data_sjw=1");

	std::thread* m_ReadThread;
	bool m_ThreadRun;

	// UDP var
	const std::string REMOTEIP = "127.0.0.1";
	const int REMOTEPORT = 9000;
	WSADATA wsaData;
	SOCKET m_socket;
	BOOL bEnableBroadcast = TRUE;
	int retVal;
	sockaddr_in senderAddr;
	int len;
};

