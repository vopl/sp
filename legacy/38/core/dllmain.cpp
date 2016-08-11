// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
#include "hel/core.h"

namespace hel
{
	__declspec(dllexport) ICore *createICore()
	{
		return new Core;
	}
}


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//extern "C" BOOL WINAPI hoardDllMain (HANDLE hinstDLL, DWORD fdwReason, LPVOID lpreserved);
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	//BOOL result = hoardDllMain(hModule, ul_reason_for_call, lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


