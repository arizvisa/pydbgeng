#pragma once

#include <string>

#include <boost/noncopyable.hpp>

#include <boost/python.hpp>
using namespace boost::python;

class CPythonEngine : public boost::noncopyable
{
  PyGILState_STATE m_threadstate;

public:
  CPythonEngine(LPCSTR name);
  ~CPythonEngine(void);

  void CPythonEngine::Acquire(void);
  void CPythonEngine::Release(void);

  LPCSTR GetName(void) const;
  LPCSTR GetHome(void) const;

  LPCSTR GetPrefix(void) const;
  LPCSTR GetExecPrefix(void) const;
  LPCSTR GetProgramFullPath(void) const;
  LPCSTR GetPath(void) const;
  LPCSTR GetVersion(void) const;
  LPCSTR GetPlatform(void) const;
  LPCSTR GetCopyright(void) const;
  LPCSTR GetCompiler(void) const;
  LPCSTR GetBuildInfo(void) const;
};
