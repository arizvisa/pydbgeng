#ifndef __DebugAdvanced_h
#define __DebugAdvanced_h
#pragma once

#include "DebugObject.h"

class CDebugAdvanced : public CDebugObject<IDebugAdvanced>
{
public:
  CDebugAdvanced(IUnknown *intf) : DebugInterface(intf)
  { }

  static void Export(void);
};

#endif
