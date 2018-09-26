#include "StdAfx.h"

#include "PythonHelper.h"
#include "DebugObject.h"

using namespace boost::python;

void utils::Export(void)
{
	enum_<ValueType>("ValueType")
		.value("INVALID",   ValueType::INVALID)
		.value("INT8",      ValueType::INT8)
		.value("INT16",     ValueType::INT16)
		.value("INT32",     ValueType::INT32)
		.value("INT64",     ValueType::INT64)
		.value("FLOAT32",   ValueType::FLOAT32)
		.value("FLOAT64",   ValueType::FLOAT64)
		.value("FLOAT80",   ValueType::FLOAT80)
		.value("FLOAT82",   ValueType::FLOAT82)
		.value("FLOAT128",  ValueType::FLOAT128)
		.value("VECTOR64",  ValueType::VECTOR64)
		.value("VECTOR128", ValueType::VECTOR128)
	;
}

/** Python helpers */
// utils::len
template<>
size_t utils::len(const dict& obj)
{
	return obj? ::PyDict_Size(obj.ptr()) : 0;
}
template<>
size_t utils::len(const list& obj)
{
	return obj? ::PyList_Size(obj.ptr()) : 0;
}

// utils::empty
template <>
bool utils::empty(const dict& obj)
{
	return !obj || ::PyDict_Size(obj.ptr()) == 0;
}
template <>
bool utils::empty(const list& obj)
{
	return !obj || ::PyList_Size(obj.ptr()) == 0;
}

const std::string utils::str(const object& obj)
{
	return std::string(extract<char *>(object(handle<>(allow_null(::PyObject_Str(obj.ptr()))))));
}

const std::string utils::repr(const object& obj)
{
	return std::string(extract<char *>(object(handle<>(allow_null(::PyObject_Repr(obj.ptr()))))));
}

// utils::RaiseException
void utils::RaiseException(errno_t err, std::string s, PyObject *type) throw(...)
{
	_set_errno(err);
	::PyErr_SetFromErrno(type);
	throw_error_already_set();
}

void utils::RaiseException(const std::string& msg, std::string s, PyObject *type) throw(...)
{
	::PyErr_SetString(type, msg.c_str());
	throw_error_already_set();
}

void utils::RaiseException(const std::string& msg, PyObject *type) throw(...)
{
	::PyErr_SetString(type, msg.c_str());
	throw_error_already_set();
}

// utils::RealCheck
template <>
void utils::RealCheck(errno_t res, std::string s) throw(...)
{
	if (res != 0)
		RaiseException(res, s);
}

template <>
void utils::RealCheck(HRESULT res, std::string s) throw(...)
{
	if (SUCCEEDED(res)) {
		::PyErr_Clear();
		return;
	}
	if (res == E_OUTOFMEMORY) {
		::PyErr_NoMemory();
	} else if (res == E_INVALIDARG) {
		::PyErr_BadArgument();
	} else {
		PyObject *type = PyExc_RuntimeError;
		const char *msg = NULL;

		if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, res, 0, (LPTSTR) &msg, 0, NULL) > 0 &&
			msg && strlen(msg) > 0) {
			::PyErr_SetString(type, msg);
			::LocalFree((HLOCAL) msg);
		} else {
			switch (res) {
				case E_FAIL:
					msg = "The operation could not be performed.";
					break;
				case E_NOINTERFACE:
					msg = "The object searched for was not found.";
					break;
				case E_UNEXPECTED:
					type = PyExc_SystemError;
					msg = "The target was not accessible, or the engine was not in a state where the function or method could be processed.";
					break;
				case E_NOTIMPL:
					type = PyExc_NotImplementedError;
					msg = "Not implemented.";
					break;
			}
			if (msg)
				::PyErr_SetString(type, msg);
			else
				::PyErr_SetFromWindowsErr(res);
		}
	}
	throw_error_already_set();
}

/** DbgEng Helpers */
const object utils::ToObject(const DEBUG_VALUE& value)
{
	size_t size(0);
	switch (value.Type) {
		case DEBUG_VALUE_INVALID:
			size = 0;
			return object();
		case DEBUG_VALUE_INT8:
			size = sizeof(value.I8);
			return object(value.I8);
		case DEBUG_VALUE_INT16:
			size = sizeof(value.I16);
			return object(value.I16);
		case DEBUG_VALUE_INT32:
			size = sizeof(value.I32);
			return object(value.I32);
		case DEBUG_VALUE_INT64:
			size = sizeof(value.I64);
			return object(value.I64);
		case DEBUG_VALUE_FLOAT32:
			size = sizeof(value.F32);
			return object(value.F32);
		case DEBUG_VALUE_FLOAT64:
			size = sizeof(value.F64);
			return object(value.F64);

		case DEBUG_VALUE_FLOAT80:
			size = sizeof(value.F80Bytes);
			break;
		case DEBUG_VALUE_FLOAT82:
			size = sizeof(value.F82Bytes);
			break;
		case DEBUG_VALUE_FLOAT128:
			size = sizeof(value.F128Bytes);
			break;
		case DEBUG_VALUE_VECTOR64:
			size = sizeof(value.VI8) / 2;
			break;
		case DEBUG_VALUE_VECTOR128:
			size = sizeof(value.VI8);
			break;
	}

	LPVOID data(NULL); Py_ssize_t len(0);
	object buffer(handle<>(::PyBuffer_New(size)));

	if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
		throw_error_already_set();
	memcpy(data, &value, size);
	return buffer;
}

template<>
static void utils::warning(boost::python::str message)
{
	object m;
	m = import("logging");
	m.attr("warn")(message);
}
template<>
static void utils::fatal(boost::python::str message)
{
	object m;
	m = import("logging");
	m.attr("fatal")(message);
	if (PyErr_Occurred()) {
		PyErr_Print();
		throw_error_already_set();
	}
}

template<>
static void utils::warning(const std::string& message)
{ return utils::warning(boost::python::str(message.c_str())); }
template<>
static void utils::fatal(const std::string& message)
{ return utils::fatal(boost::python::str(message.c_str())); }

template<>
static void utils::warning(const char* message)
{ return utils::warning(boost::python::str(message)); }
template<>
static void utils::fatal(const char* message)
{ return utils::fatal(boost::python::str(message)); }
