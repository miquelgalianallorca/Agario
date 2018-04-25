#include "stdafx.h"
#include "ServerENet.h"
#include <Windows.h>

using namespace ENet;

int _tmain(int argc, _TCHAR* argv[])
{
    CServerENet* pServer = new CServerENet();
    if (pServer->Init(1234, 5))
    {
        while (true)
        {
            std::vector<CPacketENet*>  incomingPackets;
            pServer->Service(incomingPackets, 0);
            Sleep(100);
        }
    }
    else
    {
        fprintf(stdout, "Server could not be initialized.\n");
    }

    return 0;
}

