// ICG_P1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "MainEngine.h"

int _tmain(int argc, _TCHAR * argv[])
{
    MainEngine::_game = new Game();
	MainEngine::_game->Start();
    return 0;
}

