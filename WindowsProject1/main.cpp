// WindowsProject1.cpp : Defines the entry point for the application.
//

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass* System;
	bool result;

	System = new SystemClass;

	result = System->Init();
	if (result) {
		System->Run();
	}

	System->Shutdown();
	delete System;
	System = 0;
	
	return 0;
}