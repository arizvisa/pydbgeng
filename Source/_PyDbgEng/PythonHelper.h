#pragma once

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

	template <typename T, typename ET=T::enumeration_type>
	static const list FlagsToList(T flags, T min, T max) {
		BOOST_STATIC_ASSERT((boost::is_enum<T>::value));
		list result;

		for (ET flag = static_cast<ET>(min); 0 < flag <= static_cast<ET>(max); flag <<= 1)
			if (flag == static_cast<ET>(flags) & flag)
				result.append(static_cast<T>(flag));
		return result;
	}

	template <typename T, typename ET=T::enumeration_type>
	static const ET FlagsFromList(list flags) {
		BOOST_STATIC_ASSERT((boost::is_enum<T>::value));

		ET result;
		while (!utils::empty(flags))
			result |= extract<ET>(flags.pop());
		return result;
	}
};

template <typename T>
class PythonBitSet {
private:
	T value_;

public:
	typename T type;

	PythonBitSet(enum value) : value_(static_cast<T>(value)) { }
	PythonBitSet(T value) : value_(value) { }
	PythonBitSet(T& value) : value_(value) { }

	inline PythonBitSet& operator=(const T& value) {
		value_ = value;
	}
	inline operator PythonBitSet() const {
		return value_;
	}
};
