#pragma once

#include "DebugObject.h"

class PYDBGENG_API CDebugAdvanced
  : public CDebugObject<IDebugAdvanced>
{
  typedef CDebugObject<IDebugAdvanced> __inherited;
public:
  CDebugAdvanced(IUnknown *intf) : __inherited(intf)
  {

  }

  static void Export(void);
};

template <>
CDebugObject<IDebugAdvanced>::operator IDebugAdvanced*(void) const { return m_intf; }