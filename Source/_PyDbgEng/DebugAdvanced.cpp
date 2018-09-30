#include "StdAfx.h"
#include "utils.h"
#include "DebugObject.h"
#include "DebugAdvanced.h"

#include <boost/python.hpp>
using namespace boost::python;

void CDebugAdvanced::Export(void)
{
	scope DebugAdvanced = class_<CDebugAdvanced>("DebugAdvanced", no_init)
	;
}
