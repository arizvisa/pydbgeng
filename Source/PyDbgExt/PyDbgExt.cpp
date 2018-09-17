// PyDbgExt.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <memory>
#include <sstream>
#include <vector>

#include <atlbase.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

#define KDEXT_64BIT
#include <dbgeng.h>
#include <wdbgexts.h>

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

#include "PythonEngine.h"
#include "PythonContext.h"

#include "DebugClient.h"
#include "DebugOutput.h"

#include "FileVersion.h"

#define EXT_NAME "pydbgext"

HANDLE g_hModule = NULL;

WINDBG_EXTENSION_APIS ExtensionApis;

std::auto_ptr<CPythonEngine> g_engine;
std::auto_ptr<CPythonContext> g_global;

namespace DbgEng {
	PSetOutput SetOutput;
	PGetOutput GetOutput;
}

bool InitDbgEng()
{
	HMODULE m;
	m = ::GetModuleHandle("_PyDbgEng.pyd");
	DbgEng::GetOutput = (PGetOutput) ::GetProcAddress(m, "GetOutput");
	DbgEng::SetOutput = (PSetOutput) ::GetProcAddress(m, "SetOutput");
	return true;
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD dwReason, LPVOID lpReserved )
{
  UNREFERENCED_PARAMETER(lpReserved);
  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    g_hModule = hModule;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

const std::string repr(const object& obj)
{
  return std::string(extract<char *>(object(handle<>(allow_null(::PyObject_Repr(obj.ptr()))))));
}

/* for executing an rc file, and outputting a friendly error message */
void
PyExecuteFile( std::string path )
{
  g_engine->Acquire();
  try {
    object result = g_global->ExecuteFile(path);
    if (result)
      dprintf("%s\n", repr(result).c_str());
  } catch(error_already_set) {
    PyErr_Print();
  } catch (...) {
    handle_exception();
  }
  g_engine->Release();
}

extern "C" __declspec(dllexport) HRESULT CALLBACK DebugExtensionInitialize(PULONG Version, PULONG Flags)
{
  /* XXX: already initialized? */
  if (g_engine.get() != 0)
    return 0;

  *Version = DEBUG_EXTENSION_VERSION(1, 0);
  *Flags = 0;

  CComPtr<IDebugClient> client;

  HRESULT hr = ::DebugCreate(__uuidof(IDebugClient), (PVOID*) &client);
  if (FAILED(hr))
	  return hr;
  CComQIPtr<IDebugControl> control = client;

  ExtensionApis.nSize = sizeof(ExtensionApis);
  hr = control->GetWindbgExtensionApis64(&ExtensionApis);
  if (FAILED(hr))
	  return hr;

  g_engine.reset(new CPythonEngine(EXT_NAME));
  g_engine->Acquire();
  g_global.reset(new CPythonContext());

  /*
		FIXME: if the following import fails, then output an error message
		saying that we were unable to locate the _PyDbgEng module instead of
		just dying stupidly like we do..
  */

  dict lookup = dict(g_global->Import("_PyDbgEng").attr("__dict__"));
  InitDbgEng();
  DbgEng::SetOutput(dprintf);

  object objDebugOutput = lookup.get("DebugOutput")();
  import("sys").attr("stdout") = objDebugOutput;
  import("sys").attr("stderr") = objDebugOutput;

  /* execute the user's rc file now that our namespace is prepared */
  PyExecuteFile(std::string(getenv("USERPROFILE")) + "\\pydbgextrc.py");
  g_engine->Release();
  return hr;
}

extern "C" __declspec(dllexport) void CALLBACK DebugExtensionUninitialize(void)
{
  //CDebugOutput::controller.operator=( CDebugControl
  g_global.reset();
  g_engine.reset();
  memset(&ExtensionApis, 0, sizeof(ExtensionApis));
}

extern "C" __declspec(dllexport) void CALLBACK DebugExtensionNotify(ULONG Notify, ULONG64 Argument)
{
  UNREFERENCED_PARAMETER(Argument);

  /* XXX: should we save our python state somewhere?? */
  switch (Notify) {
  case DEBUG_NOTIFY_SESSION_ACTIVE:
    /* locate and execute the rc script in our homedirectory */
    PyExecuteFile(std::string(getenv("USERPROFILE")) + "\\pydbgextsess.py");
    break;

  case DEBUG_NOTIFY_SESSION_INACTIVE:
    break;

  case DEBUG_NOTIFY_SESSION_ACCESSIBLE:
    break;

  case DEBUG_NOTIFY_SESSION_INACCESSIBLE:
    break;
  }
}

/*
A built-in help for the extension dll
*/
extern "C" __declspec(dllexport) HRESULT CALLBACK help(PDEBUG_CLIENT Client, PCSTR args)
{
  UNREFERENCED_PARAMETER(Client);
  UNREFERENCED_PARAMETER(args);

  char szExtPath[MAX_PATH], szExtDate[MAX_PATH], szExtTime[MAX_PATH];
  WIN32_FILE_ATTRIBUTE_DATA wfad;

  szExtPath[::GetModuleFileName((HMODULE) g_hModule, szExtPath, _countof(szExtPath))] = 0;
  ::GetFileAttributesEx(szExtPath, GetFileExInfoStandard, &wfad);

  FILETIME ftCreate; SYSTEMTIME stCreate;
  ::FileTimeToLocalFileTime(&wfad.ftCreationTime, &ftCreate);
  ::FileTimeToSystemTime(&ftCreate, &stCreate);

  szExtDate[::GetDateFormat(LOCALE_USER_DEFAULT, 0, &stCreate, NULL, szExtDate, _countof(szExtDate))] = 0;
  szExtTime[::GetTimeFormat(LOCALE_USER_DEFAULT, 0, &stCreate, NULL, szExtTime, _countof(szExtTime))] = 0;

  util::CFileVersion ver((HMODULE) g_hModule);

  g_engine->Acquire();  //not sure if we need this, but i'm putting it here anyways
  dprintf("Help for " EXT_NAME ".dll\n\n"
          "[Envionment]\n\n"
          "\tExtention Image\t: %s built %s %s @ %s, %d\n"
          "\tPython Version\t: %s @ %s\n\n"
          "[Usage]\n\n"
          "\teval\t\t\t: Evaluate a python expression\n"
          "\timport\t\t\t: Import a python module\n"
          "\tfrom\t\t\t: Import a list of symbols from a python module\n"
          "\tprint\t\t\t: Evaluate and then print a python expression\n"
          "\texec\t\t\t: Execute a python script\n"
          "\thelp\t\t\t: Shows this help\n"
          "\tpy\t\t\t: Evaluate a python expression\n",
          ver.GetFileVersionString().c_str(), szExtDate, szExtTime, szExtPath, wfad.nFileSizeLow,
          g_engine->GetVersion(), g_engine->GetPlatform());
  g_engine->Release();
  return S_OK;
}

std::ostream& operator << (std::ostream& os, const DEBUG_VALUE& value)
{
  switch (value.Type) {
  case DEBUG_VALUE_INT8:
    os << value.I8;
    break;
  case DEBUG_VALUE_INT16:
    os << value.I16;
    break;
  case DEBUG_VALUE_INT32:
    os << value.I32;
    break;
  case DEBUG_VALUE_INT64:
    os << value.I64;
    break;
  case DEBUG_VALUE_FLOAT32:
    os << value.F32;
    break;
  case DEBUG_VALUE_FLOAT64:
    os << value.F64;
    break;
  }
  return os;
}

HRESULT evaluate(PDEBUG_CLIENT Client, PCSTR args)
{
  HRESULT result(E_FAIL);

  g_engine->Acquire();
  try {
    object r = g_global->Execute(args);
    if (r)
      dprintf("%s\n", repr(r).c_str());
    result = S_OK;
  } catch(error_already_set) {
    PyErr_Print();
  } catch (...) {
    handle_exception();
  }
  g_engine->Release();
  return result;
}

extern "C" __declspec(dllexport) HRESULT CALLBACK eval(PDEBUG_CLIENT Client, PCSTR args)
{
    return evaluate(Client, args);
}

extern "C" __declspec(dllexport) HRESULT CALLBACK py(PDEBUG_CLIENT Client, PCSTR args)
{
    return evaluate(Client, args);
}

extern "C" __declspec(dllexport) HRESULT CALLBACK exec(PDEBUG_CLIENT Client, PCSTR args)
{
  //CDebugClient::Scope use(Client);
  HRESULT result = E_FAIL;
  g_engine->Acquire();

  try {
    object r = g_global->ExecuteFile(args);
    if (r) {
      dprintf("%s\n", repr(r).c_str());
      g_global->AddSymbol("_", r);   /* FIXME: this doesn't seem to work... */

    }
    result = S_OK;
  } catch(error_already_set) {
    PyErr_Print();
  } catch (...) {
    handle_exception();
  }
  g_engine->Release();
  return result;
}

extern "C" __declspec(dllexport) HRESULT CALLBACK import(PDEBUG_CLIENT Client, PCSTR args)
{
  HRESULT result = E_FAIL;

  g_engine->Acquire();
  try {
    std::vector<std::string> modules;
    split(modules, std::string(args), is_any_of(", "));

    for (std::vector<std::string>::const_iterator it = modules.begin(); it != modules.end(); it++) {
      const std::string name = *it;
      if (name.empty())
        continue;
      object mod;
      if (g_global->Import(name, mod)) {
        g_global->AddSymbol(name, mod);
        dprintf("Import %s succeeded.\n", repr(mod).c_str());
      } else
        dprintf("No module named %s.\n", name.c_str());
    }
    result = S_OK;
  } catch(error_already_set) {
    PyErr_Print();
  } catch (...) {
    handle_exception();
  }

  g_engine->Release();
  return result;
}

#if 0
extern "C" __declspec(dllexport) HRESULT CALLBACK from(PDEBUG_CLIENT Client, PCSTR args)
{
  //CDebugClient::Scope use(Client);
  HRESULT result = E_FAIL;

  const char *pszImport = strstr(args, " import ");
  if (!pszImport) {
    dprintf("invalid syntax, 'from' command must has a import section");
    return result;
  }

  std::string modName = std::string(args, pszImport-args);
  trim(modName);

  std::vector<std::string> symbols;
  split(symbols, std::string(pszImport+strlen(" import ")), is_any_of(", "));

  if (symbols.empty()) {
    dprintf("invalid syntax, 'from' command must follow a imported symbol list");
    return result;
  }

  g_engine->Acquire();
  try  {
    object mod = g_global->Import(modName);

    if (mod) {
      dict modSymbols(mod.attr("__dict__"));
      std::string symbolList, missingList;

      if (symbols.end() != std::find(symbols.begin(), symbols.end(), "*")) {
        g_global->AddSymbols(modSymbols);
        symbolList = "all symbols";
      } else {
        std::ostringstream oss, mss;

        for (size_t i=0; i<symbols.size(); i++) {
          const std::string symbolName = symbols[i];

          if (symbolName.empty())
			continue;

          if (::PyObject_HasAttr(mod.ptr(), str(symbolName.c_str()).ptr())) {
            g_global->AddSymbol(symbolName, modSymbols[symbolName]);
            oss << (oss.str().size() == 0 ? "" : ", ") << symbolName;
          } else {
            mss << (mss.str().size() == 0 ? "" : ", ") << symbolName;
          }
        }
        symbolList = oss.str();
        missingList = mss.str();
      }

      dprintf("Import %s from %s succeeded.\n", symbolList.c_str(), repr(mod).c_str());
      if (!missingList.empty())
        dprintf("Symbols %s are not exist in the module.\n", missingList.c_str());
    }
    result = S_OK;
  }
  catch(error_already_set)
  {
    PyErr_Print();
  }
  g_engine->Release();
  return result;
}
#endif

extern "C" __declspec(dllexport) HRESULT CALLBACK print(PDEBUG_CLIENT Client, PCSTR args)
{
  /* FIXME: This function probably doesn't work as intended... */
  g_engine->Acquire();
  HRESULT result = E_FAIL;
  try {
    object r = g_global->Execute(args);
    dprintf("%s\n", repr(r).c_str());
    result = S_OK;
  } catch(error_already_set) {
    PyErr_Print();
  } catch (...) {
    handle_exception();
  }
  g_engine->Release();
  return result;
}
