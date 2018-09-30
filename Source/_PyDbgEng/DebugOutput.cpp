#include "StdAfx.h"
#include "utils.h"
#include "DebugObject.h"
#include "DebugOutput.h"

#include <boost/python.hpp>
using namespace boost::python;

PWINDBG_OUTPUT_ROUTINE CDebugOutput::s_callback = NULL;

void CDebugOutput::Export(void)
{
	class_<CDebugOutput>("DebugOutput")
		.def("write", &CDebugOutput::Write)
	;

	enum_<OutputMask>("OutputMask")
		.value("NORMAL",            OutputMask::MASK_NORMAL)            // Normal output.
		.value("ERROR",             OutputMask::MASK_ERROR)             // Error output.
		.value("WARNING",           OutputMask::MASK_WARNING)           // Warnings.
		.value("VERBOSE",           OutputMask::MASK_VERBOSE)           // Additional output.
		.value("PROMPT",            OutputMask::MASK_PROMPT)            // Prompt output.
		.value("REGISTERS",         OutputMask::MASK_REGISTERS)         // Register dump before prompt.
		.value("EXTENSION_WARNING", OutputMask::MASK_EXTENSION_WARNING) // Warnings specific to extension operation.
		.value("DEBUGGEE",          OutputMask::MASK_DEBUGGEE)          // Debuggee debug output, such as from OutputDebugString.
		.value("DEBUGGEE_PROMPT",   OutputMask::MASK_DEBUGGEE_PROMPT)   // Debuggee-generated prompt, such as from DbgPrompt.
		.value("SYMBOLS",           OutputMask::MASK_SYMBOLS)           // Symbol messages, such as for !sym noisy.

		.value("_KD_PROTOCOL",    OutputMask::MASK_KD_PROTOCOL)    // KD protocol output.
		.value("_REMOTING",       OutputMask::MASK_REMOTING)       // Remoting output.
		.value("_BREAKPOINT",     OutputMask::MASK_BREAKPOINT)     // Breakpoint output.
		.value("_EVENT",          OutputMask::MASK_EVENT)          // Event output.
		.value("_ADDR_TRANSLATE", OutputMask::MASK_ADDR_TRANSLATE) // Virtual/Physical address translation
	;

}

void CDebugOutput::Write(const char * text)
{
	s_callback("%s", text);
}

PWINDBG_OUTPUT_ROUTINE CDebugOutput::getCallback()
{
	return s_callback;
}

void CDebugOutput::setCallback(PWINDBG_OUTPUT_ROUTINE c)
{
	s_callback = c;
}

/* exported */
extern "C" {
	__declspec(dllexport) PWINDBG_OUTPUT_ROUTINE SetOutput(PWINDBG_OUTPUT_ROUTINE c)
	{
		PWINDBG_OUTPUT_ROUTINE result;
		result = CDebugOutput::getCallback();

		CDebugOutput::setCallback(c);
		return result;
	}

	__declspec(dllexport) PWINDBG_OUTPUT_ROUTINE GetOutput()
	{
		return CDebugOutput::getCallback();
	}
}
