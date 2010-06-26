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
  if (s_callback) s_callback(text);
}