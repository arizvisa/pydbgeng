#pragma once

#include <boost/python.hpp>
using namespace boost::python;

struct CPythonHelper
{
  static void RaiseException(const std::string& msg, PyObject *type = PyExc_RuntimeError) throw(...);
  static void RaiseException(errno_t err, std::string s, PyObject *type = PyExc_RuntimeError) throw(...);
  static void RaiseException(const std::string& msg, std::string s, PyObject *type = PyExc_RuntimeError) throw(...);

  static void RealCheck(HRESULT hr, std::string s);
  static void RealCheck(errno_t err, std::string s);

  static size_t len(const dict& d);
  static size_t len(const list& l);

  static bool empty(const dict& d);
  static bool empty(const list& l);

  static const std::string str(const object& obj) throw(...);
  static const std::string repr(const object& obj) throw(...);
};