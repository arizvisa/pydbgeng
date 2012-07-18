#pragma once

#include <cassert>

#include <atlbase.h>

class CDebugEngine
{
public:
  CDebugEngine(void);
  ~CDebugEngine(void);

  template <typename I>
  static CComPtr<I> CreateInstance()
  {
    CComPtr<I> instance;

    HRESULT hr = ::DebugCreate(__uuidof(I), (PVOID*) &instance);

    if (SUCCEEDED(hr) && instance)
      return instance;

    ::PyErr_SetFromWindowsErr(hr);
    throw_error_already_set();  

    assert(false); // never run to here, throw exception on previous line

    return instance; 
  }
};
