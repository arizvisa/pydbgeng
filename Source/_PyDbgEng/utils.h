#pragma once

#include <boost/python.hpp>
#include <dbgeng.h>

using namespace boost::python;

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
		VECTOR128 = DEBUG_VALUE_VECTOR128,
	};

/** Python Helpers */
	static const object ToObject(const DEBUG_VALUE& value);

	static void RaiseException(const std::string& msg, PyObject *type = PyExc_RuntimeError) throw(...);
	static void RaiseException(const std::string& msg, std::string s, PyObject *type = PyExc_RuntimeError) throw(...);
	static void RaiseException(errno_t err, std::string s, PyObject *type = PyExc_RuntimeError) throw(...);

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

		for (size_t flag = static_cast<size_t>(min); flag <= static_cast<size_t>(max); flag <<= 1)
			if (static_cast<size_t>(flags) & flag == flag)
				result.append(static_cast<T>(flag));
		return result;
	}

	template <typename Tr, typename T>
	static const Tr FlagsFromList(list flags) {
		BOOST_STATIC_ASSERT((boost::is_enum<T>::value));

		Tr result;
		while (!utils::empty(flags))
			result |= extract<Tr>(flags.pop());
		return result;
	}
};

#define Check(x) do { utils::RealCheck(x, #x); } while (0)
