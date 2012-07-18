#include "StdAfx.h"
#include "DebugOutput.h"

#include <boost/python.hpp>
using namespace boost::python;

PWINDBG_OUTPUT_ROUTINE CDebugOutput::s_callback = NULL;

void CDebugOutput::Export(void)
{
  class_<CDebugOutput>("DebugOutput")
    .def("write", &CDebugOutput::Write);
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