#include "StdAfx.h"
#include "PythonEngine.h"

#include <boost/python.hpp>

CPythonEngine::CPythonEngine(LPCSTR name)
{
  PyThreadState* threadstate;

  ::Py_SetProgramName(const_cast<LPSTR>(name));
  ::Py_Initialize();

  ::PyEval_InitThreads();
  threadstate = ::PyThreadState_Get();
  m_interpreter = threadstate->interp;
  (void)::PyEval_ReleaseThread(threadstate);
}

CPythonEngine::~CPythonEngine(void)
{
  (void)this->Acquire();
  if ( ::Py_IsInitialized() ) {
    try {
      ::Py_Finalize();  /* XXX: for some reason this crashes occasionally */
    } catch (...) {
        /*
            XXX: FIXME: WTF: this isn't right, but I acquired the lock..
                 what else do i need to do?
        */
    }
    m_threadstate = static_cast<PyGILState_STATE>(0);
    return;
  }
  (void)this->Release();
  m_threadstate = static_cast<PyGILState_STATE>(0);
}

void
CPythonEngine::Acquire(void)
{
     m_threadstate = ::PyGILState_Ensure();
//    (void)::PyEval_AcquireThread(m_threadstate);
//    (void)::PyEval_AcquireLock();
}

void
CPythonEngine::Release(void)
{
//    (void)::PyEval_RestoreThread(m_threadstate);
    (void)::PyGILState_Release(m_threadstate);
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