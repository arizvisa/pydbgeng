// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// windows header files
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef CreateProcess
#undef STATUS_TIMEOUT

// dbgeng header files
#include <DbgEng.h>
#include <DbgHelp.h>

// boost-python header files
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>

// utilities
#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
	#ifndef __field_ecount_opt
		#define __field_ecount_opt(x)
	#endif
#endif

// disabling specific warnings
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#pragma warning( disable : 4121 ) // 'symbol' : alignment of a member was sensitive to packing
#pragma warning( disable : 4244 ) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning( disable : 4251 ) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning( disable : 4819 ) // The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.
