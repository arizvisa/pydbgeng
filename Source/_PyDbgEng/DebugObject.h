#pragma once
#include <iostream>

#include <atlbase.h>

#include <DbgEng.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include "PythonHelper.h"

struct CDebugHelper
{
  enum ValueType
  {
    INVALID   = DEBUG_VALUE_INVALID,
    INT8      = DEBUG_VALUE_INT8,     
    INT16     = DEBUG_VALUE_INT16,        
    INT32     = DEBUG_VALUE_INT32,        
    INT64     = DEBUG_VALUE_INT64,        
    FLOAT32   = DEBUG_VALUE_FLOAT32,      
    FLOAT64   = DEBUG_VALUE_FLOAT64,      
    FLOAT80   = DEBUG_VALUE_FLOAT80,      
    FLOAT82   = DEBUG_VALUE_FLOAT82,      
    FLOAT128  = DEBUG_VALUE_FLOAT128,     
    VECTOR64  = DEBUG_VALUE_VECTOR64,     
    VECTOR128 = DEBUG_VALUE_VECTOR128    
  };

  enum OutputMask
  {
    MASK_NORMAL = DEBUG_OUTPUT_NORMAL,                         // Normal output.
    MASK_ERROR = DEBUG_OUTPUT_ERROR,                           // Error output.
    MASK_WARNING = DEBUG_OUTPUT_WARNING,                       // Warnings.
    MASK_VERBOSE = DEBUG_OUTPUT_VERBOSE,                       // Additional output.
    MASK_PROMPT = DEBUG_OUTPUT_PROMPT,                         // Prompt output.
    MASK_REGISTERS = DEBUG_OUTPUT_PROMPT_REGISTERS,            // Register dump before prompt.
    MASK_EXTENSION_WARNING = DEBUG_OUTPUT_EXTENSION_WARNING,   // Warnings specific to extension operation.
    MASK_DEBUGGEE = DEBUG_OUTPUT_DEBUGGEE,                     // Debuggee debug output, such as from OutputDebugString.
    MASK_DEBUGGEE_PROMPT = DEBUG_OUTPUT_DEBUGGEE_PROMPT,       // Debuggee-generated prompt, such as from DbgPrompt.
    MASK_SYMBOLS = DEBUG_OUTPUT_SYMBOLS                        // Symbol messages, such as for !sym noisy.
  };

  enum OutputTarget
  {
    TARGET_THIS_CLIENT = DEBUG_OUTCTL_THIS_CLIENT,
    TARGET_ALL_CLIENTS = DEBUG_OUTCTL_ALL_CLIENTS,
    TARGET_ALL_OTHER_CLIENTS = DEBUG_OUTCTL_ALL_OTHER_CLIENTS,
    TARGET_IGNORE = DEBUG_OUTCTL_IGNORE,
    TARGET_LOG_ONLY = DEBUG_OUTCTL_LOG_ONLY,
  };

  enum KernelQualifier
  {
    KERNEL_CONNECTION = DEBUG_KERNEL_CONNECTION,
    KERNEL_LOCAL = DEBUG_KERNEL_LOCAL,
    KERNEL_EXDI_DRIVER = DEBUG_KERNEL_EXDI_DRIVER,

    KERNEL_SMALL_DUMP = DEBUG_KERNEL_SMALL_DUMP,
    KERNEL_DUMP = DEBUG_KERNEL_DUMP,
    KERNEL_FULL_DUMP = DEBUG_KERNEL_FULL_DUMP,

    KERNEL_TRACE_LOG = DEBUG_KERNEL_TRACE_LOG
  };

  enum UserQualifier
  {
    USER_PROCESS = DEBUG_USER_WINDOWS_PROCESS,
    USER_PROCESS_SERVER = DEBUG_USER_WINDOWS_PROCESS_SERVER,
    USER_IDNA = DEBUG_USER_WINDOWS_IDNA,
    USER_SMALL_DUMP = DEBUG_USER_WINDOWS_SMALL_DUMP,
    USER_DUMP = DEBUG_USER_WINDOWS_DUMP,
    USER_DUMP_WINDOWS_CE = DEBUG_USER_WINDOWS_DUMP_WINDOWS_CE
  };

  static void Export(void);

  static size_t CDebugHelper::GetSize(const DEBUG_VALUE& value);
  static const object ToObject(const DEBUG_VALUE& value);
};

template <typename I>
class CDebugObject : public CDebugHelper, public CPythonHelper
{
protected:
  typedef I DefaultInterface;

  template <typename T>
  static const list FlagsToList(T flags, T min, T max) 
  {
    BOOST_STATIC_ASSERT((boost::is_enum<T>::value));

    list result;

    for (ULONG flag = static_cast<ULONG>(min); 0 < flag && flag <= static_cast<ULONG>(max); flag <<= 1)
      if (flag == (static_cast<ULONG>(flags) & flag))
        result.append(static_cast<T>(flag));

    return result;
  }

  template <typename T>
  static const T FlagsFromList(list flags) 
  {
    BOOST_STATIC_ASSERT((boost::is_enum<T>::value));

    ULONG value;

    while (!empty(flags))
    {
      value |= (ULONG) extract<T>(flags.pop());
    }

    return static_cast<T>(value);
  }

  CComQIPtr<I> m_intf;

  CDebugObject(IUnknown *intf) : m_intf(intf)
  {}  
public:
  virtual ~CDebugObject(void)
  {

  }

  operator I*(void) const { return m_intf; }

  I *GetInterface(void) const { return m_intf; }
};

#define Check(x) { RealCheck(x, #x); }