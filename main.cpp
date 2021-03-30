#include <iostream>
#include "PeerToPeerNetwork.h"




int main()
{
    std::cout << "Hello World!\n";
    p2p::Sender sender;
    p2p::Reciever reciever;

    std::thread senderThread = std::thread([&]() { sender.SendPacket("127.0.0.1"); });
    std::thread recieverThread = std::thread([&]() {reciever.asyncWait(); });

    senderThread.join();
    recieverThread.join();

    return 0;
}