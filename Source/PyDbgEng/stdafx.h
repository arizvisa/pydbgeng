// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#undef CreateProcess

#define _countof(array) (sizeof(array)/sizeof(array[0]))

#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#pragma warning( disable : 4121 ) // 'symbol' : alignment of a member was sensitive to packing
#pragma warning( disable : 4244 ) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning( disable : 4251 ) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning( disable : 4819 ) // The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.

#ifndef __field_ecount_opt
  #define __field_ecount_opt(x) 
#endif
