#include <iostream>
#include <conio.h>
#include "PeerToPeerNetwork.h"




int main()
{
    std::cout << "Hello World!\n";
    char command;
    std::cin >> command;
    if (command == 's')
    {
        p2p::Reciever r;
        r.asyncWait();
    }
    else
    {
        p2p::Sender s;
        s.SendPacket("127.0.0.1");
    }
    while (!_kbhit());

    return 0;
}