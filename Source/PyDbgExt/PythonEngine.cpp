#include "StdAfx.h"
#include "PythonEngine.h"

#include <boost/python.hpp>

CPythonEngine::CPythonEngine(LPCSTR name)
{
  ::Py_SetProgramName(const_cast<LPSTR>(name));
  ::Py_Initialize();

  ::PyEval_InitThreads();
}

CPythonEngine::~CPythonEngine(void)
{
  (void)this->Acquire();
  if ( ::Py_IsInitialized() ) {
    ::Py_Finalize();
    return;
  }
  (void)this->Release();
}

void
CPythonEngine::Acquire(void)
{
    m_threadstate = ::PyGILState_Ensure();
}

void
CPythonEngine::Release(void)
{
    ::PyGILState_Release( m_threadstate );
}

LPCSTR CPythonEngine::GetName(void) const
{
  return ::Py_GetProgramName();
}
LPCSTR CPythonEngine::GetHome(void) const
{
  return ::Py_GetPythonHome();
}
LPCSTR CPythonEngine::GetPrefix(void) const
{
  return ::Py_GetPrefix();
}
LPCSTR CPythonEngine::GetExecPrefix(void) const
{
  return ::Py_GetExecPrefix();
}
LPCSTR CPythonEngine::GetProgramFullPath(void) const
{
  return ::Py_GetProgramFullPath();
}
LPCSTR CPythonEngine::GetPath(void) const
{
  return ::Py_GetPath();
}
LPCSTR CPythonEngine::GetVersion(void) const
{
  return ::Py_GetVersion();
}
LPCSTR CPythonEngine::GetPlatform(void) const
{
  return ::Py_GetPlatform();
}
LPCSTR CPythonEngine::GetCopyright(void) const
{
  return ::Py_GetCopyright();
}
LPCSTR CPythonEngine::GetCompiler(void) const
{
  return ::Py_GetCompiler();
}
LPCSTR CPythonEngine::GetBuildInfo(void) const
{
  return ::Py_GetBuildInfo();
}