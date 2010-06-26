#include "StdAfx.h"
#include "DebugEngine.h"

#include <DbgEng.h>

#include <boost/python.hpp>
using namespace boost::python;

#include "DebugOutput.h"

#include "DebugClient.h"
#include "DebugControl.h"
#include "DebugRegisters.h"
#include "DebugSymbols.h"
#include "DebugDataSpaces.h"
#include "DebugSystemObjects.h"
#include "DebugAdvanced.h"

BOOST_PYTHON_MODULE(PyDbgEng)
{
  CDebugOutput::Export();
  CDebugHelper::Export();
  CDebugClient::Export();
  CDebugControl::Export();
  CDebugRegisters::Export();
  CDebugSymbols::Export();
  CDebugDataSpaces::Export();
  CDebugSystemObjects::Export();
  CDebugAdvanced::Export();
}