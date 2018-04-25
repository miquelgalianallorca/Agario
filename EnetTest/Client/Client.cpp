#include "stdafx.h"

#include "swalib/sys.h"
#include "swalib/core.h"
#include "swalib/font.h"

#include "ClientENet.h"
#include <Windows.h>

#include "Game.h"

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

	Game game;
    while (!SYS_GottaQuit()) {
		glClear(GL_COLOR_BUFFER_BIT);

		game.Update();		
		game.Render();
		
		SYS_Show();
		SYS_Pump();
		SYS_Sleep(17);
    }

	FONT_End();
    return 0;
}

