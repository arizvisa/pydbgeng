// PyDbgEng.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PyDbgEng.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  dwReason, LPVOID lpReserved )
{
  UNREFERENCED_PARAMETER(hModule);
  UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    
  return TRUE;
}