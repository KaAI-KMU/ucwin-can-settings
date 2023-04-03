#include "stdafx.h"

class Client
{
private:
    WSADATA wsaData; // Winsock initialization data
    SOCKET sock;
    sockaddr_in server;
    const int port = 8888;
    const std::string ipAddress = "127.0.0.1";
    double mSteering = 0;
    double mThrottle = 0;
    double mBrake = 0;

    // std::vector<double> vecSteer;
    // std::vector<double> vecThrottle;

public:
    Client();
    ~Client();

    void ConnectToServer();
    void SendData();
    void ReceiveData();

private:
    std::string GetIDString(char buffer[]);
    std::string GetDataString(char buffer[]);
    void Parser710(char buffer[]);
    void Parser711(char buffer[]);
};

