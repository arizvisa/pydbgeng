#ifndef __DebugObject_h
#define __DebugObject_h
#pragma once

#include <iostream>
#include <atlbase.h>

#include <dbgeng.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
using namespace boost::python;

#define Check(x) do { utils::RealCheck(x, #x); } while (0)

struct utils
{
  static void Export(void);

/** DbgEng Helpers */
  enum class ValueType {
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

/** Python Helpers */
  static const object ToObject(const DEBUG_VALUE& value);

  static void RaiseException(const std::string& msg               , PyObject *type = PyExc_RuntimeError) throw(...);
  static void RaiseException(const std::string& msg, std::string s, PyObject *type = PyExc_RuntimeError) throw(...);
  static void RaiseException(errno_t err,            std::string s, PyObject *type = PyExc_RuntimeError) throw(...);

  template <typename T>
  static void RealCheck(T res, std::string s);

  template <class T>
  static size_t len(const T& obj);

  template <class T>
  static bool empty(const T& obj);

  static const std::string str(const object& obj) throw(...);
  static const std::string repr(const object& obj) throw(...);

  template <class T>
  static void warning(T message);

  template <class T>
  static void fatal(T message);

  template <typename T>
  static const list FlagsToList(T flags, T min, T max) {
    BOOST_STATIC_ASSERT((boost::is_enum<T>::value));
    list result;

    for (ULONG flag = static_cast<ULONG>(min); 0 < flag <= static_cast<ULONG>(max); flag <<= 1)
      if (flag == static_cast<ULONG>(flags) & flag)
        result.append(static_cast<T>(flag));
    return result;
  }

  template <typename T>
  static const T FlagsFromList(list flags) {
    BOOST_STATIC_ASSERT((boost::is_enum<T>::value));

    ULONG result;
    while (!utils::empty(flags))
      result |= (ULONG) extract<T>(flags.pop());
    return static_cast<T>(result);
  }
};

class CDebugControl;

template <typename I>
class CDebugObject {
protected:
  typedef I DefaultInterface;
  typedef CDebugObject<I> DebugInterface;
  CComQIPtr<I> m_intf;

/** scope */
  CDebugObject()
  {
    HRESULT hr;
    hr = DebugInterface::Create(&m_intf);
    if (hr != S_OK) {
        ::PyErr_SetFromWindowsErr(hr);
        throw_error_already_set();
    }
  }
  CDebugObject(IUnknown *intf) : m_intf(intf)
  { }

public:
  virtual ~CDebugObject(void)
  { }

/** interface scope */
  operator I*(void) const { return m_intf; }
  I *GetInterface(void) const { return m_intf; }

  static inline HRESULT Create(I** intf) {
    return ::DebugCreate(__uuidof(I), (PVOID*)intf);
  }
};

template <typename I>
class CDebugObjectOwned : public CDebugObject<I> {
protected:
  typedef CDebugObjectOwned<I> DebugInterface;
  CComQIPtr<IDebugControl> m_owner;

public:
  DebugInterface(const CDebugControl* owner) :
    m_owner(owner->GetInterface())
  {}

  DebugInterface(const CDebugControl* owner, DefaultInterface* intf) :
    m_owner(owner->GetInterface()), DebugInterface(intf)
  {}

  DebugInterface(CDebugControl* const owner, DefaultInterface* intf) :
    m_owner(owner->GetInterface()), DebugInterface(intf)
  {}
};
#endif
