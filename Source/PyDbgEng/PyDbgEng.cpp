#include "StdAfx.h"
#include "DebugOutput.h"
using namespace boost::python;

#include "DebugClient.h"
#include "DebugControl.h"
#include "DebugRegisters.h"
#include "DebugSymbols.h"
#include "DebugDataSpaces.h"
#include "DebugSystemObjects.h"
#include "DebugAdvanced.h"

/** returns a new IDebugClient interface */
CDebugClient*
Create()
{
    return new CDebugClient();
}

/** Creates a new IDebugClient interface via calling ::DebugConnect */
CDebugClient*
Connect(const std::string& remoteOptions)
{
    CComPtr<IDebugClient> intf;
    HRESULT hr;

    hr = ::DebugConnect(remoteOptions.c_str(), __uuidof(IDebugClient), (PVOID*)&intf);
    if (hr != S_OK) {
        ::PyErr_SetFromWindowsErr(hr);
        throw_error_already_set();
        assert(false == true);
    }
    assert(intf);

    return new CDebugClient(intf);
}

BOOST_PYTHON_MODULE(_PyDbgEng)
{
  utils::Export();
  CDebugOutput::Export();
  CDebugClient::Export();
  CDebugControl::Export();
  CDebugRegisters::Export();
  CDebugSymbols::Export();
  CDebugDataSpaces::Export();
  CDebugSystemObjects::Export();
  CDebugAdvanced::Export();

  def("Create", Create, return_value_policy<manage_new_object>());
  def("Connect", Connect, args("remoteOptions"), return_value_policy<manage_new_object>());
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  dwReason, LPVOID lpReserved )
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
