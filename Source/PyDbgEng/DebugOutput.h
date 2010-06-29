#pragma once

#include <string>

#define KDEXT_64BIT
#include <wdbgexts.h>

#include "PyDbgEng.h"

class PYDBGENG_API CDebugOutput
{
  static PWINDBG_OUTPUT_ROUTINE s_callback;
public:
  CDebugOutput(void)
  {
  }
  
  static void Export(void);

  void Write(const char *text);

  static PWINDBG_OUTPUT_ROUTINE GetCallback(void) { return s_callback; }
  static void SetCallback(PWINDBG_OUTPUT_ROUTINE callback) { s_callback = callback; }
};
