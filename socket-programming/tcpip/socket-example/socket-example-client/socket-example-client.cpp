#include "Client.h"

int main() {
    Client client;
    client.ConnectToServer();
    client.SendData();
    client.ReceiveData();
    return 0;
}