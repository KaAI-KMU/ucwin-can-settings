#include "Client.h"

int main() {
    Client client;
    client.ConnectToServer();
    client.SendData();
    while(1)
        client.ReceiveData();
    return 0;
}