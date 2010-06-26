#pragma once

#include <boost/python.hpp>
using namespace boost::python;

#include "PyDbgEng.h"

struct PYDBGENG_API CPythonHelper
{
  static void RaiseException(errno_t err, PyObject *type = PyExc_RuntimeError) throw(...);
  static void RaiseException(const std::string& msg, PyObject *type = PyExc_RuntimeError) throw(...);

  static void Check(HRESULT hr);
  static void Check(errno_t err);

  static size_t len(const dict& d);
  static size_t len(const list& l);

  static bool empty(const dict& d);
  static bool empty(const list& l);

  static const std::string str(const object& obj) throw(...);
  static const std::string repr(const object& obj) throw(...);
};