#pragma once

#include <boost/python.hpp>
using namespace boost::python;

class CPythonContext
{
  object m_objMainModule;
  dict m_objMainNamespace, m_objLocalNamespace;
public:
  CPythonContext(void);
  ~CPythonContext(void)
  {
  }

  bool Import(const std::string& name, object& mod);
  object Import(const std::string& name);

  object ExecuteFile( const std::string& filename)
  {
    return exec_file( str(filename), m_objMainNamespace, m_objLocalNamespace );
  }

  object Execute(const std::string& command)
  {
    return exec( str(command), m_objMainNamespace, m_objLocalNamespace );    
  }

  template <typename T>
  bool Execute(const std::string& command, T& value)
  {
    try
    {
      object result( exec( str(command.cstr()), m_objMainNamespace, m_objLocalNamespace ) );

      if (result)
        value = extract<T>(result);

      return true;
    }
    catch(error_already_set)
    {
      return false;
    }
  }

  void AddSymbol(const std::string& name, object obj);
  void AddSymbols(const dict& symbols);
};
