#include "stdafx.h"

#include "swalib/sys.h"
#include "swalib/core.h"
#include "swalib/font.h"

#include "ClientENet.h"
#include <Windows.h>

using namespace ENet;

void SetupEngine() {
	FONT_Init();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); // Sets up clipping
	glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCR_WIDTH, 0.0, SCR_HEIGHT, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int Main(void)
{
	SetupEngine();

    CClienteENet* pClient = new CClienteENet();
    pClient->Init();

    CPeerENet* pPeer = pClient->Connect("127.0.0.1", 1234, 2);

    std::vector<CPacketENet*>  incomingPackets;
    pClient->Service(incomingPackets, 0);
    Sleep(100);
    pClient->SendData(pPeer, "pepe", 4, 0, false);
    while (true)
    {
		glClear(GL_COLOR_BUFFER_BIT);

        std::vector<CPacketENet*>  incommingPackets;
        pClient->Service(incommingPackets, 0);
        //Sleep(100);

		SYS_Show();
		// Keep system running
		SYS_Pump();
		SYS_Sleep(17);
    }

    pClient->Disconnect(pPeer);

    return 0;
}

