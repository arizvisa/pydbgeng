#include "StdAfx.h"
#include "PythonContext.h"

CPythonContext::CPythonContext(void)
{
    m_objMainModule = import("__main__");
    m_objMainNamespace = extract<dict>(m_objMainModule.attr("__dict__"));
}

bool CPythonContext::Import(const std::string& name, object& mod)
{
  handle<> result(allow_null(::PyImport_ImportModule(name.c_str())));

  if (result) mod = object(result);

  return result;
}

object CPythonContext::Import(const std::string& name)
{
  PyObject* module = ::PyImport_ImportModule( name.c_str() );
  return object(handle<>(module));
}

void CPythonContext::AddSymbol(const std::string& name, object obj)
{
  dict symbols;

  symbols[name] = obj;

  AddSymbols(symbols);
}

void CPythonContext::AddSymbols(const dict& symbols)
{
  m_objMainNamespace.update(symbols);
}
