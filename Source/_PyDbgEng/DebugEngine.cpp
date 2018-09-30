#include "StdAfx.h"
#include "utils.h"

#include "DebugObject.h"
#include "DebugOutput.h"
#include "DebugRegisters.h"
#include "DebugSymbols.h"
#include "DebugDataSpaces.h"
#include "DebugSystemObjects.h"
#include "DebugAdvanced.h"
#include "DebugControl.h"
#include "DebugClient.h"

#include "DebugEngine.h"

#include <dbgeng.h>
#include <boost/python.hpp>
using namespace boost::python;

/** returns a new IDebugClient interface */
CDebugClient*
Create()
{
	return new CDebugClient();    // XXX: i umm hope boost & python manage these references
}

/** Creates a new IDebugClient interface via calling ::DebugConnect */
CDebugClient*
Connect(const std::string& remoteOptions)
{
	CComPtr<IDebugClient> intf;
	HRESULT hr;

	// FIXME: there's probably a safer way to do this rather than reinterpret_cast
	hr = ::DebugConnect(remoteOptions.c_str(), __uuidof(IDebugClient), reinterpret_cast<PVOID*>(&intf));
	if (hr != S_OK) {
		::PyErr_SetFromWindowsErr(hr);
		throw_error_already_set();
		assert(false == true);
	}
	// FIXME: there's a better way to assert via BOOST
	assert(intf);

	return new CDebugClient(intf);
}

/** Create a new IDebugClient, and attach it to the kernel specified in the connectOptions */
CDebugClient*
ConnectKernel(const std::string& connectOptions)
{
	CDebugClient::AttachKernelFlags flags;
	CDebugClient* p;

	// if no string is specified, assume that the user specified local
	flags = ( connectOptions.empty())? CDebugClient::AttachKernelFlags::ATTACH_LOCAL_KERNEL : CDebugClient::AttachKernelFlags::ATTACH_KERNEL_CONNECTION;
	p = new CDebugClient();
	p->AttachKernel(flags, connectOptions);
	return p;
}

BOOST_PYTHON_MODULE(_PyDbgEng)
{
	CDebugOutput::Export();
	//CDebugHelper::Export();
	CDebugClient::Export();
	CDebugControl::Export();
	CDebugRegisters::Export();
	CDebugSymbols::Export();
	CDebugDataSpaces::Export();
	CDebugSystemObjects::Export();
	CDebugAdvanced::Export();

	def("Create", Create, return_value_policy<manage_new_object>());
	def("Connect", Connect, args("remoteOptions"), return_value_policy<manage_new_object>());
	def("ConnectKernel", ConnectKernel, arg("connectOptions")=std::string(), return_value_policy<manage_new_object>());
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
