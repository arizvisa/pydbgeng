#include "StdAfx.h"
#include "DebugAdvanced.h"

void CDebugAdvanced::Export(void)
{
  scope DebugAdvanced = class_<CDebugAdvanced>("DebugAdvanced", no_init)
    ;
}