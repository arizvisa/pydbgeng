#include "StdAfx.h"
#include "DebugEngine.h"

#include <DbgEng.h>

#include <boost/python.hpp>
using namespace boost::python;

#include "DebugOutput.h"

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
    return new CDebugClient();    // XXX: i umm hope boost & python manage these references
}

/** Creates a new IDebugClient interface via calling ::DebugConnect */
CDebugClient*
Connect(const std::string& remoteOptions)
{
    CComPtr<IDebugClient> intf;
    HRESULT r;
    
    r = ::DebugConnect( remoteOptions.c_str(), __uuidof(IDebugClient), (PVOID*)&intf);
    if (r != S_OK) {
        ::PyErr_SetFromWindowsErr(r);
        throw_error_already_set();  
        assert(false == true);
    }
    assert(intf);

    return new CDebugClient(intf);
}

/** Create a new IDebugClient, and attach it to the kernel specified in the connectOptions */
CDebugClient*
ConnectKernel(const std::string& connectOptions)
{
    CDebugClient::AttachKernelFlag flags;
    CDebugClient* p;

    // if no string is specified, assume that the user specified local
    flags = ( connectOptions.empty())? CDebugClient::ATTACH_LOCAL_KERNEL : CDebugClient::ATTACH_KERNEL_CONNECTION;
    p = new CDebugClient();
    p->AttachKernel(flags, connectOptions);
    return p;
}

BOOST_PYTHON_MODULE(_PyDbgEng)
{
  CDebugOutput::Export();
  CDebugHelper::Export();
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
