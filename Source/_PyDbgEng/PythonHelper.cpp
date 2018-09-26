#include "StdAfx.h"
#include "PythonHelper.h"

size_t CPythonHelper::len(const dict& d)
{
	return d ? ::PyDict_Size(d.ptr()) : 0;
}
size_t CPythonHelper::len(const list& l)
{
	return l ? ::PyList_Size(l.ptr()) : 0;
}

bool CPythonHelper::empty(const dict& d)
{
	return !d || ::PyDict_Size(d.ptr()) == 0;
}
bool CPythonHelper::empty(const list& l)
{
	return !l || ::PyList_Size(l.ptr()) == 0;
}

const std::string CPythonHelper::str(const object& obj)
{
	return std::string(extract<char *>(object(handle<>(allow_null(::PyObject_Str(obj.ptr()))))));
}

const std::string CPythonHelper::repr(const object& obj)
{
	return std::string(extract<char *>(object(handle<>(allow_null(::PyObject_Repr(obj.ptr()))))));
}

void CPythonHelper::RaiseException(errno_t err, std::string s, PyObject *type) throw(...)
{
	_set_errno(err);

	::PyErr_SetFromErrno(type);

	throw_error_already_set();
}

void CPythonHelper::RaiseException(const std::string& msg, std::string s, PyObject *type) throw(...)
{
	::PyErr_SetString(type, msg.c_str());

	throw_error_already_set();
}

void CPythonHelper::RaiseException(const std::string& msg, PyObject *type) throw(...)
{
	::PyErr_SetString(type, msg.c_str());

	throw_error_already_set();
}

void CPythonHelper::RealCheck(errno_t err, std::string s) throw(...)
{
	if (err != 0) RaiseException(err, s);
}

void CPythonHelper::RealCheck(HRESULT hr, std::string s) throw(...)
{
	if (SUCCEEDED(hr)) {
		::PyErr_Clear();
		return;
	}

	if (hr == E_OUTOFMEMORY) {
		::PyErr_NoMemory();
	} else if (hr == E_INVALIDARG) {
		::PyErr_BadArgument();
	} else {
		PyObject *type = PyExc_RuntimeError;
		const char *msg = NULL;

		if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, hr, 0, (LPTSTR) &msg, 0, NULL) > 0 && msg && strlen(msg) > 0) {
			::PyErr_SetString(type, msg);

			::LocalFree((HLOCAL) msg);
		} else {
			switch (hr)
			{
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
				::PyErr_SetFromWindowsErr(hr);
		}
	}

	throw_error_already_set();
}
