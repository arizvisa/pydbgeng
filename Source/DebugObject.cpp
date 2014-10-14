#include "StdAfx.h"
#include "DebugObject.h"

void CDebugHelper::Export(void)
{
  enum_<ValueType>("ValueType")
    .value("INVALID",   INVALID)
    .value("INT8",      INT8)
    .value("INT16",     INT16)
    .value("INT32",     INT32)
    .value("INT64",     INT64)
    .value("FLOAT32",   FLOAT32)
    .value("FLOAT64",   FLOAT64)
    .value("FLOAT80",   FLOAT80)
    .value("FLOAT82",   FLOAT82)
    .value("FLOAT128",  FLOAT128)
    .value("VECTOR64",  VECTOR64)
    .value("VECTOR128", VECTOR128)
    ;

  enum_<OutputMask>("OutputMask")
    .value("NORMAL",            MASK_NORMAL)
    .value("ERROR",             MASK_ERROR)
    .value("WARNING",           MASK_WARNING)
    .value("VERBOSE",           MASK_VERBOSE)
    .value("PROMPT",            MASK_PROMPT)
    .value("REGISTERS",         MASK_REGISTERS)
    .value("EXTENSION_WARNING", MASK_EXTENSION_WARNING)
    .value("DEBUGGEE",          MASK_DEBUGGEE)
    .value("DEBUGGEE_PROMPT",   MASK_DEBUGGEE_PROMPT)
    .value("SYMBOLS",           MASK_SYMBOLS)
    ;


  enum_<OutputTarget>("OutputTarget")
    .value("THIS_CLIENT",       TARGET_THIS_CLIENT)
    .value("ALL_CLIENTS",       TARGET_ALL_CLIENTS)
    .value("ALL_OTHER_CLIENTS", TARGET_ALL_OTHER_CLIENTS)
    .value("IGNORE",            TARGET_IGNORE)
    .value("LOG_ONLY",          TARGET_LOG_ONLY)
    ;

  enum_<KernelQualifier>("KernelQualifier")
    .value("CONNECTION",  KERNEL_CONNECTION)
    .value("LOCAL",       KERNEL_LOCAL)
    .value("EXDI_DRIVER", KERNEL_EXDI_DRIVER)
    .value("SMALL_DUMP",  KERNEL_SMALL_DUMP)
    .value("DUMP",        KERNEL_DUMP)
    .value("FULL_DUMP",   KERNEL_FULL_DUMP)
    .value("TRACE_LOG",   KERNEL_TRACE_LOG)
    ;

  enum_<UserQualifier>("UserQualifier")  
    .value("PROCESS",         USER_PROCESS)
    .value("PROCESS_SERVER",  USER_PROCESS_SERVER)
    .value("IDNA",            USER_IDNA)
    .value("SMALL_DUMP",      USER_SMALL_DUMP)
    .value("DUMP",            USER_DUMP)
    .value("DUMP_WINDOWS_CE", USER_DUMP_WINDOWS_CE)
    ;

}

size_t CDebugHelper::GetSize(const DEBUG_VALUE& value)
{
  static const size_t s_ValueSize[] = 
  { 
    0, 
    sizeof(value.I8), 
    sizeof(value.I16), 
    sizeof(value.I32), 
    sizeof(value.I64),
    sizeof(value.F32), 
    sizeof(value.F64), 
    _countof(value.F80Bytes), 
    _countof(value.F82Bytes),
    _countof(value.F128Bytes),
    _countof(value.VI8) / 2,
    _countof(value.VI8)
  };

  return value.Type < DEBUG_VALUE_TYPES ? s_ValueSize[value.Type] : 0;
}

const object CDebugHelper::ToObject(const DEBUG_VALUE& value)
{
  switch (value.Type)
  {
  case DEBUG_VALUE_INT8:    
    return object(value.I8);
  case DEBUG_VALUE_INT16:
    return object(value.I16);
  case DEBUG_VALUE_INT32:
    return object(value.I32);
  case DEBUG_VALUE_INT64:
    return object(value.I64);
  case DEBUG_VALUE_FLOAT32:
    return object(value.F32);
  case DEBUG_VALUE_FLOAT64:
    return object(value.F64);
  }

  size_t size = GetSize(value);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  memcpy(data, &value, size);

  return buffer;
}