#include "stdafx.h"
#include "ServerENet.h"
#include <Windows.h>

#include "Server.h"

using namespace ENet;

int _tmain(int argc, _TCHAR* argv[]) {
	Server server;
	if (server.Init()) {
		while (true) {
			server.Update();
		}
	}
	else fprintf(stdout, "Server could not be initialized.\n");

    return 0;
}

