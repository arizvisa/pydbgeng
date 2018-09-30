#ifndef __DebugObject_h
#define __DebugObject_h
#pragma once

#include <iostream>
#include <atlbase.h>

#include <dbgeng.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
using namespace boost::python;

/** Forward declarations **/
class CDebugControl;

/** Class declarations **/
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
	I *GetInterface(void) const { return m_intf; }

	operator I*(void) const {
		return GetInterface();
	}

	static inline HRESULT Create(I** intf) {
		return ::DebugCreate(__uuidof(I), (PVOID*)intf);
	}
};

template <typename I>
class CDebugObjectOwned : public CDebugObject<I> {
protected:
	typedef CDebugObject<I> DefaultInterface;
	typedef CDebugObjectOwned<I> DebugInterface;
	CComQIPtr<IDebugControl> m_owner;

public:
	DebugInterface(const CDebugControl* owner) :
		m_owner(owner->GetInterface())
	{}

	DebugInterface(const CDebugControl* owner, I* intf) :
		m_owner(owner->GetInterface()), DefaultInterface(intf)
	{}

	DebugInterface(CDebugControl* const owner, I* intf) :
		DefaultInterface(intf)
	{}
};
#endif
