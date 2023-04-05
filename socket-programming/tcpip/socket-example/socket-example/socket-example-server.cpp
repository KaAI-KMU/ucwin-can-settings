#include "Server.h"

int main() {
    Server server;
    server.AcceptConnection();
    server.ReceiveData();
    server.SendData();
    return 0;
}