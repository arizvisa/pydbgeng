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
};
#endif
