#ifndef __DebugOutput_h
#define __DebugOutput_h
#pragma once

#include <atlbase.h>
#include <dbgeng.h>
#include <string>

#define KDEXT_64BIT
#include <wdbgexts.h>

typedef void (*PSetOutput)(PWINDBG_OUTPUT_ROUTINE);
typedef PWINDBG_OUTPUT_ROUTINE (*PGetOutput)();

class CDebugOutput
{
	static PWINDBG_OUTPUT_ROUTINE s_callback;

public:
	CDebugOutput(void) {};

	static PWINDBG_OUTPUT_ROUTINE getCallback();
	static void setCallback(PWINDBG_OUTPUT_ROUTINE c);

	static void Export(void);
	static void Write(const char *text);

	enum class OutputMask : ULONG {
		MASK_NORMAL            = DEBUG_OUTPUT_NORMAL,
		MASK_ERROR             = DEBUG_OUTPUT_ERROR,
		MASK_WARNING           = DEBUG_OUTPUT_WARNING,
		MASK_VERBOSE           = DEBUG_OUTPUT_VERBOSE,
		MASK_PROMPT            = DEBUG_OUTPUT_PROMPT,
		MASK_REGISTERS         = DEBUG_OUTPUT_PROMPT_REGISTERS,
		MASK_EXTENSION_WARNING = DEBUG_OUTPUT_EXTENSION_WARNING,
		MASK_DEBUGGEE          = DEBUG_OUTPUT_DEBUGGEE,
		MASK_DEBUGGEE_PROMPT   = DEBUG_OUTPUT_DEBUGGEE_PROMPT,
		MASK_SYMBOLS           = DEBUG_OUTPUT_SYMBOLS,

		MASK_KD_PROTOCOL    = DEBUG_IOUTPUT_KD_PROTOCOL,
		MASK_REMOTING       = DEBUG_IOUTPUT_REMOTING,
		MASK_BREAKPOINT     = DEBUG_IOUTPUT_BREAKPOINT,
		MASK_EVENT          = DEBUG_IOUTPUT_EVENT,
		MASK_ADDR_TRANSLATE = DEBUG_IOUTPUT_ADDR_TRANSLATE,
	};
};
#endif
