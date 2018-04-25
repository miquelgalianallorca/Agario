#include "stdafx.h"
#include "ClientENet.h"
#include <Windows.h>

using namespace ENet;

int _tmain(int argc, _TCHAR* argv[])
{
    CClienteENet* pClient = new CClienteENet();
    pClient->Init();

    CPeerENet* pPeer = pClient->Connect("127.0.0.1", 1234, 2);

    std::vector<CPacketENet*>  incomingPackets;
    pClient->Service(incomingPackets, 0);
    Sleep(100);
    pClient->SendData(pPeer, "pepe", 4, 0, false);
    while (true)
    {
        std::vector<CPacketENet*>  incommingPackets;
        pClient->Service(incommingPackets, 0);
        Sleep(100);

    }

    pClient->Disconnect(pPeer);

    return 0;
}

