#include "StdAfx.h"
#include "DebugClient.h"

#include <sstream>

CComQIPtr<CDebugClient::DefaultInterface> CDebugClient::s_current;

void CDebugClient::Export(void)
{
  enum_<CreateFlags>("CreateFlags")
    .value("DEFAULT",                   CREATE_DEFAULT)
    .value("ATTACH_PROCESS",            CREATE_ATTACH_PROCESS)
    .value("ATTACH_ONLY_THIS_PROCESS",  CREATE_ATTACH_ONLY_THIS_PROCESS)
    .value("SUSPENDED",                 CREATE_SUSPENDED_)
    .value("NO_DEBUG_HEAP",             CREATE_NO_DEBUG_HEAP)
    .value("THROUGH_RTL",               CREATE_THROUGH_RTL)
    ;

  enum_<AttachUserFlags>("AttachUserFlags")
    .value("DEFAULT",                   ATTACH_DEFAULT)
    .value("NONINVASIVE",               ATTACH_NONINVASIVE)
    .value("EXISTING",                  ATTACH_EXISTING)
    .value("NONINVASIVE_NO_SUSPEND",    ATTACH_NONINVASIVE_NO_SUSPEND)
    .value("INVASIVE_NO_INITIAL_BREAK", ATTACH_INVASIVE_NO_INITIAL_BREAK)
    .value("INVASIVE_RESUME_PROCESS",   ATTACH_INVASIVE_RESUME_PROCESS)
    .value("NONINVASIVE_ALLOW_PARTIAL", ATTACH_NONINVASIVE_ALLOW_PARTIAL)
    ;

  enum_<AttachKernelFlag>("AttachKernelFlag")
    .value("KERNEL_CONNECTION", ATTACH_KERNEL_CONNECTION)
    .value("LOCAL_KERNEL",      ATTACH_LOCAL_KERNEL)
    .value("EXDI_DRIVER",       ATTACH_EXDI_DRIVER)
    ;

  enum_<ChangeStateFlags>("StateFlags")
    .value("ALL",       CDS_ALL)
    .value("REGISTERS", CDS_REGISTERS)
    .value("DATA",      CDS_DATA)
    ;

  enum_<DebugSessionStatus>("SessionStatus")
    .value("ACTIVE",                        SESSION_ACTIVE)
    .value("END_SESSION_ACTIVE_TERMINATE",  SESSION_END_SESSION_ACTIVE_TERMINATE)
    .value("END_SESSION_ACTIVE_DETACH",     SESSION_END_SESSION_ACTIVE_DETACH)
    .value("END_SESSION_PASSIVE",           SESSION_END_SESSION_PASSIVE)
    .value("END",                           SESSION_END)
    .value("REBOOT",                        SESSION_REBOOT)
    .value("HIBERNATE",                     SESSION_HIBERNATE)
    .value("FAILURE",                       SESSION_FAILURE)                     
    ;

  enum_<DebugEvent>("DebugEvent")
    .value("BREAKPOINT",            EVENT_BREAKPOINT)
    .value("EXCEPTION",             EVENT_EXCEPTION)
    .value("CREATE_THREAD",         EVENT_CREATE_THREAD)
    .value("EXIT_THREAD",           EVENT_EXIT_THREAD)
    .value("CREATE_PROCESS",        EVENT_CREATE_PROCESS)
    .value("EXIT_PROCESS",          EVENT_EXIT_PROCESS)
    .value("LOAD_MODULE",           EVENT_LOAD_MODULE)
    .value("UNLOAD_MODULE",         EVENT_UNLOAD_MODULE)
    .value("SYSTEM_ERROR",          EVENT_SYSTEM_ERROR)
    .value("SESSION_STATUS",        EVENT_SESSION_STATUS)
    .value("CHANGE_DEBUGGEE_STATE", EVENT_CHANGE_DEBUGGEE_STATE)
    .value("CHANGE_ENGINE_STATE",   EVENT_CHANGE_ENGINE_STATE)
    .value("CHANGE_SYMBOL_STATE",   EVENT_CHANGE_SYMBOL_STATE)
    ;

  enum_<DumpFormat>("DumpFormat")
    .value("DEFAULT",                               FORMAT_DEFAULT)
    .value("CAB_SECONDARY_ALL_IMAGES",              FORMAT_CAB_SECONDARY_ALL_IMAGES)
    .value("WRITE_CAB",                             FORMAT_WRITE_CAB)
    .value("CAB_SECONDARY_FILES",                   FORMAT_CAB_SECONDARY_FILES)
    .value("NO_OVERWRITE",                          FORMAT_NO_OVERWRITE)
    .value("USER_SMALL_FULL_MEMORY",                FORMAT_USER_SMALL_FULL_MEMORY)
    .value("USER_SMALL_HANDLE_DATA",                FORMAT_USER_SMALL_HANDLE_DATA)
    .value("USER_SMALL_UNLOADED_MODULES",           FORMAT_USER_SMALL_UNLOADED_MODULES)
    .value("USER_SMALL_INDIRECT_MEMORY",            FORMAT_USER_SMALL_INDIRECT_MEMORY)
    .value("USER_SMALL_DATA_SEGMENTS",              FORMAT_USER_SMALL_DATA_SEGMENTS)
    .value("USER_SMALL_FILTER_MEMORY",              FORMAT_USER_SMALL_FILTER_MEMORY)
    .value("USER_SMALL_FILTER_PATHS",               FORMAT_USER_SMALL_FILTER_PATHS)
    .value("USER_SMALL_PROCESS_THREAD_DATA",        FORMAT_USER_SMALL_PROCESS_THREAD_DATA)
    .value("USER_SMALL_PRIVATE_READ_WRITE_MEMORY",  FORMAT_USER_SMALL_PRIVATE_READ_WRITE_MEMORY)
    .value("USER_SMALL_NO_OPTIONAL_DATA",           FORMAT_USER_SMALL_NO_OPTIONAL_DATA)
    .value("USER_SMALL_FULL_MEMORY_INFO",           FORMAT_USER_SMALL_FULL_MEMORY_INFO)
    .value("USER_SMALL_THREAD_INFO",                FORMAT_USER_SMALL_THREAD_INFO)
    .value("USER_SMALL_CODE_SEGMENTS",              FORMAT_USER_SMALL_CODE_SEGMENTS)
    .value("USER_SMALL_NO_AUXILIARY_STATE",         FORMAT_USER_SMALL_NO_AUXILIARY_STATE)
    .value("USER_SMALL_FULL_AUXILIARY_STATE",       FORMAT_USER_SMALL_FULL_AUXILIARY_STATE)  
    ;

  scope DebugClient = class_<CDebugClient>("DebugClient", init<>())
    .add_static_property("Current", &CDebugClient::GetCurrent)

    .add_property("Control", &CDebugClient::GetControl)
    .add_property("Registers", &CDebugClient::GetRegisters)
    .add_property("Symbols", &CDebugClient::GetSymbols)
    .add_property("DataSpaces", &CDebugClient::GetDataSpaces)
    .add_property("SystemObjects", &CDebugClient::GetSystemObjects)

    .def_readonly("EventCallbacks", &CDebugClient::m_eventCallbacks)
    .def_readonly("InputCallbacks", &CDebugClient::m_inputCallbacks)
    .def_readonly("OutputCallbacks", &CDebugClient::m_outputCallbacks)

    .add_property("NumberOfEventCallbacks", &CDebugClient::GetNumberOfEventCallbacks)
    .add_property("NumberOfInputCallbacks", &CDebugClient::GetNumberOfInputCallbacks)
    .add_property("NumberOfOutputCallbacks", &CDebugClient::GetNumberOfOutputCallbacks)

    .add_property("ExitCode", &CDebugClient::GetExitCode,
      "returns the exit code of the current process if that process has already run through to completion.")
    
    .add_property("Identity", &CDebugClient::GetIdentity,
      "return a string describing the computer and user this client represents.")
    .def("OutputIdentity", &CDebugClient::OutputIdentity,
      (arg("format")="%s", arg("target")=TARGET_ALL_CLIENTS), 
      "format and output a string describing the computer and user this client represents.")

    .add_property("OutputMask", &CDebugClient::GetOutputMask, &CDebugClient::SetOutputMask,
      "the output mask currently set for the client.")
    .add_property("OutputWidth", &CDebugClient::GetOutputWidth, &CDebugClient::SetOutputWidth,
      "the width of the output display for this client object.")
    .add_property("QuitLockString", &CDebugClient::GetQuitLockString, &CDebugClient::SetQuitLockString,
      "the string used to prevent the user from accidentally ending the debugging session.")

    .def("CreateProcess", &CDebugClient::CreateProcess,
      ("commandLine", arg("initialDirectory")=std::string(), arg("environmentVariables")=dict(),
       arg("createFlags")=CREATE_DEFAULT, arg("attachFlags")=ATTACH_DEFAULT, 
       arg("attachProcessId")=0, arg("server")=DEFAULT_PROCESS_SERVER),
      "create a process from the specified command line.")
    .def("AttachProcess", &CDebugClient::AttachProcess, 
      ("id", arg("flags")=ATTACH_DEFAULT, arg("server")=DEFAULT_PROCESS_SERVER),
      "connects the debugger engine to a user-mode process")
    .def("DetachProcesses", &CDebugClient::DetachProcesses,
      "detaches the debugger engine from all processes in all targets, resuming all their threads.")
    .def("TerminateProcesses", &CDebugClient::TerminateProcesses,
      "attempts to terminate all processes in all targets.")
    .def("AbandonCurrentProcess", &CDebugClient::AbandonCurrentProcess,
      "removes the current process from the debugger engine's process list without detaching or terminating the process.")
    .def("DetachCurrentProcess", &CDebugClient::DetachCurrentProcess,
      "detaches the debugger engine from the current process, resuming all its threads.")
    .def("TerminateCurrentProcess", &CDebugClient::TerminateCurrentProcess,
      "attempts to terminate the current process.")

    .def("AttachKernel", &CDebugClient::AttachKernel, 
      (arg("flags")=ATTACH_LOCAL_KERNEL, arg("connectOptions")=std::string()),
      "connect the debugger engine to a kernel target.")
    .add_property("IsKernelDebuggerEnabled", &CDebugClient::IsKernelDebuggerEnabled,
      "whether kernel debugging is enabled for the local kernel.")
    .add_property("KernelConnectionOptions", &CDebugClient::GetKernelConnectionOptions, &CDebugClient::SetKernelConnectionOptions,
      "the connection options for the current kernel target.")

    .def("OpenDumpFile", &CDebugClient::OpenDumpFile, "opens a dump file as a debugger target.")
    .def("AddDumpInformationFile", &CDebugClient::AddDumpInformationFile, 
      "registers additional files containing supporting information that will be used when opening a dump file.")
    .add_property("GetDumpFiles", &CDebugClient::GetDumpFiles,
      "describe the files containing supporting information that were used when opening the current dump target.")
    .def("WriteDumpFile", &CDebugClient::WriteUserDumpFile, 
      ("filename", arg("qualifier")=USER_SMALL_DUMP, arg("format")=FORMAT_DEFAULT, arg("comment")=std::string()),
      "creates a user-mode crash dump file.")
    .def("WriteDumpFile", &CDebugClient::WriteKernelDumpFile, 
      ("filename", arg("qualifier")=KERNEL_SMALL_DUMP, arg("format")=FORMAT_DEFAULT, arg("comment")=std::string()),
      "creates a kernel-mode crash dump file.")
    ;

  class_<CDebugClient::CDebugCallbacksBase, boost::noncopyable>("DebugCallbacksBase", no_init)
    .def("Attach", &CDebugClient::CDebugCallbacksBase::Attach)
    .def("Detach", &CDebugClient::CDebugCallbacksBase::Detach)
    ;

  #define DEF_EVENT(name, doc) \
    .add_property(#name, \
                  &CDebugClient::CDebugEventCallbacks::Get##name, \
                  &CDebugClient::CDebugEventCallbacks::Set##name, \
                  doc)

  class_<CComObjectStackEx<CDebugClient::CDebugEventCallbacks>, bases<CDebugClient::CDebugCallbacksBase> >("DebugEventCallbacks", no_init)
    DEF_EVENT(Breakpoint, "the target issues a breakpoint exception.")
    DEF_EVENT(Exception, "an exception debugging event occurs in the target.")
    DEF_EVENT(CreateThread, "a create-thread debugging event occurs in the target.")
    DEF_EVENT(ExitThread, "an exit-thread debugging event occurs in the target.")
    DEF_EVENT(CreateProcess, "a create-process debugging event occurs in the target.")
    DEF_EVENT(ExitProcess, "an exit-process debugging event occurs in the target.")
    DEF_EVENT(LoadModule, "a module-load debugging event occurs in the target.")
    DEF_EVENT(UnloadModule, "a module-unload debugging event occurs in the target.")
    DEF_EVENT(SystemError, "a system error occurs in the target.")
    DEF_EVENT(SessionStatus, "a change occurs in the debugger session.")
    DEF_EVENT(ChangeDebuggeeState, "the engine makes or detects changes to the target.")
    DEF_EVENT(ChangeEngineState, "the engine state has changed.")
    DEF_EVENT(ChangeSymbolState, "the symbol state changes.")
    ;

  class_<CComObjectStackEx<CDebugClient::CDebugInputCallbacks>, bases<CDebugClient::CDebugCallbacksBase> >("DebugInputCallbacks", no_init)
    .add_property("Input", &CDebugClient::CDebugInputCallbacks::GetInput, &CDebugClient::CDebugInputCallbacks::SetInput)
    ;

  class_<CComObjectStackEx<CDebugClient::CDebugOutputCallbacks>, bases<CDebugClient::CDebugCallbacksBase> >("DebugOutputCallbacks", no_init)
    .add_property("Output", &CDebugClient::CDebugOutputCallbacks::GetOutput, &CDebugClient::CDebugOutputCallbacks::SetOutput)
    ;

  class_<CDebugClient::CEventArgs>("EventArgs", no_init)
    .add_property("Source", &CDebugClient::CEventArgs::GetOwner)
    ;

  class_< CDebugClient::CBreakpointEventArgs, bases<CDebugClient::CEventArgs> >("BreakpointEventArgs", no_init)
    .add_property("Breakpoint", &CDebugClient::CBreakpointEventArgs::GetBreakpoint)
    ;

  class_< CDebugClient::CExceptionEventArgs, bases<CDebugClient::CEventArgs> >("ExceptionEventArgs", no_init)
    .add_property("IsFirstChance", &CDebugClient::CExceptionEventArgs::IsFirstChance)
    .add_property("Code", &CDebugClient::CExceptionEventArgs::GetExceptionCode)
    .add_property("Flags", &CDebugClient::CExceptionEventArgs::GetExceptionFlags)
    .add_property("Address", &CDebugClient::CExceptionEventArgs::GetExceptionAddress)
    .add_property("Parameters", &CDebugClient::CExceptionEventArgs::GetParameters)
    ;

  class_< CDebugClient::CCreateThreadEventArgs, bases<CDebugClient::CEventArgs> >("CreateThreadEventArgs", no_init)
    .add_property("Handle", &CDebugClient::CCreateThreadEventArgs::GetHandle)
    .add_property("DataOffset", &CDebugClient::CCreateThreadEventArgs::GetDataOffset)
    .add_property("StartOffset", &CDebugClient::CCreateThreadEventArgs::GetStartOffset)
    ;

  class_< CDebugClient::CExitThreadEventArgs, bases<CDebugClient::CEventArgs> >("ExitThreadEventArgs", no_init)
    .add_property("ExitCode", &CDebugClient::CExitThreadEventArgs::GetExitCode)
    ;

  class_< CDebugClient::CCreateProcessEventArgs, bases<CDebugClient::CEventArgs> >("CreateProcessEventArgs", no_init)
    .add_property("ImageFileHandle", &CDebugClient::CCreateProcessEventArgs::GetImageFileHandle)
    .add_property("Handle", &CDebugClient::CCreateProcessEventArgs::GetHandle)
    .add_property("BaseOffset", &CDebugClient::CCreateProcessEventArgs::GetBaseOffset)
    .add_property("ModuleSize", &CDebugClient::CCreateProcessEventArgs::GetModuleSize)
    .add_property("ModuleName", &CDebugClient::CCreateProcessEventArgs::GetModuleName)
    .add_property("ImageName", &CDebugClient::CCreateProcessEventArgs::GetImageName)
    .add_property("CheckSum", &CDebugClient::CCreateProcessEventArgs::GetCheckSum)
    .add_property("TimeDateStamp", &CDebugClient::CCreateProcessEventArgs::GetTimeDateStamp)
    .add_property("InitialThreadHandle", &CDebugClient::CCreateProcessEventArgs::GetInitialThreadHandle)
    .add_property("ThreadDataOffset", &CDebugClient::CCreateProcessEventArgs::GetThreadDataOffset)
    .add_property("StartOffset", &CDebugClient::CCreateProcessEventArgs::GetStartOffset)
    ;

  class_< CDebugClient::CExitProcessEventArgs, bases<CDebugClient::CEventArgs> >("ExitProcessEventArgs", no_init)
    .add_property("ExitCode", &CDebugClient::CExitProcessEventArgs::GetExitCode)
    ;

  class_< CDebugClient::CLoadModuleEventArgs, bases<CDebugClient::CEventArgs> >("LoadModuleEventArgs", no_init)
    .add_property("ImageFileHandle", &CDebugClient::CLoadModuleEventArgs::GetImageFileHandle)
    .add_property("BaseOffset", &CDebugClient::CLoadModuleEventArgs::GetBaseOffset)
    .add_property("ModuleSize", &CDebugClient::CLoadModuleEventArgs::GetModuleSize)
    .add_property("ModuleName", &CDebugClient::CLoadModuleEventArgs::GetModuleName)
    .add_property("ImageName", &CDebugClient::CLoadModuleEventArgs::GetImageName)
    .add_property("CheckSum", &CDebugClient::CLoadModuleEventArgs::GetCheckSum)
    .add_property("TimeDateStamp", &CDebugClient::CLoadModuleEventArgs::GetTimeDateStamp)
    ;

  class_< CDebugClient::CUnloadModuleEventArgs, bases<CDebugClient::CEventArgs> >("UnloadModuleEventArgs", no_init)
    .add_property("ImageBaseName", &CDebugClient::CUnloadModuleEventArgs::GetImageBaseName)
    .add_property("BaseOffset", &CDebugClient::CUnloadModuleEventArgs::GetBaseOffset)
    ;

  class_< CDebugClient::CSystemErrorEventArgs, bases<CDebugClient::CEventArgs> >("SystemErrorEventArgs", no_init)
    .add_property("Error", &CDebugClient::CSystemErrorEventArgs::GetError)
    .add_property("Level", &CDebugClient::CSystemErrorEventArgs::GetLevel)
    ;

  class_< CDebugClient::CSessionStatusEventArgs, bases<CDebugClient::CEventArgs> >("SessionStatusEventArgs", no_init)
    .add_property("Status", &CDebugClient::CSessionStatusEventArgs::GetStatus)
    ;

  class_< CDebugClient::CStateChangeEventArgs, bases<CDebugClient::CEventArgs> >("StateChangeEventArgs", no_init)
    .add_property("Flags", &CDebugClient::CStateChangeEventArgs::GetFlags)
    .add_property("Argument", &CDebugClient::CStateChangeEventArgs::GetArgument)
    ;

  class_< CDebugClient::CInputEventArgs, bases<CDebugClient::CEventArgs> >("InputEventArgs", no_init)
    .add_property("BufferSize", &CDebugClient::CInputEventArgs::GetBufferSize)
    ;

  class_< CDebugClient::COutputEventArgs, bases<CDebugClient::CEventArgs> >("OutputEventArgs", no_init)
    .add_property("Mask", &CDebugClient::COutputEventArgs::GetMask)
    .add_property("Text", &CDebugClient::COutputEventArgs::GetText)
    ;
}

const list CDebugClient::CExceptionEventArgs::GetParameters(void) const
{
  list params;

  for (DWORD i=0; i<m_pRecord->NumberParameters; i++)
  {
    params.append(m_pRecord->ExceptionInformation[i]);
  }

  return params;
}

// IDebugEventCallbacks
STDMETHODIMP CDebugClient::CDebugEventCallbacks::GetInterestMask(__out PULONG Mask)
{
  *Mask = 0;

  for(CallbackMap::const_iterator it = m_eventCallbacks.begin(); it != m_eventCallbacks.end(); it++)
  {
    *Mask |= it->first;
  }

  return S_OK;
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::Breakpoint(__in PDEBUG_BREAKPOINT Bp)
{
  return CallEventCallback(DEBUG_EVENT_BREAKPOINT, CBreakpointEventArgs(m_owner, Bp));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::Exception(__in PEXCEPTION_RECORD64 Exception, __in ULONG FirstChance)
{
  return CallEventCallback(DEBUG_EVENT_BREAKPOINT, CExceptionEventArgs(m_owner, Exception, FirstChance));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::CreateThread(__in ULONG64 Handle, __in ULONG64 DataOffset, __in ULONG64 StartOffset)
{
  return CallEventCallback(DEBUG_EVENT_CREATE_THREAD, CCreateThreadEventArgs(m_owner, Handle, DataOffset, StartOffset));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ExitThread(__in ULONG ExitCode)
{
  return CallEventCallback(DEBUG_EVENT_EXIT_THREAD, CExitThreadEventArgs(m_owner, ExitCode));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::CreateProcess(__in ULONG64 ImageFileHandle, __in ULONG64 Handle, 
                                                               __in ULONG64 BaseOffset, __in ULONG ModuleSize, 
                                                               __in_opt PCSTR ModuleName, __in_opt PCSTR ImageName, 
                                                               __in ULONG CheckSum, __in ULONG TimeDateStamp, 
                                                               __in ULONG64 InitialThreadHandle, __in ULONG64 ThreadDataOffset, 
                                                               __in ULONG64 StartOffset)
{
  return CallEventCallback(DEBUG_EVENT_CREATE_PROCESS, CCreateProcessEventArgs(m_owner, 
    ImageFileHandle, Handle, BaseOffset, ModuleSize, ModuleName, ImageName, 
    CheckSum, TimeDateStamp, InitialThreadHandle, ThreadDataOffset, StartOffset));  
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ExitProcess(__in ULONG ExitCode)
{
  return CallEventCallback(DEBUG_EVENT_EXIT_PROCESS, CExitProcessEventArgs(m_owner, ExitCode));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::LoadModule(__in ULONG64 ImageFileHandle, __in ULONG64 BaseOffset, 
                                                            __in ULONG ModuleSize, __in_opt PCSTR ModuleName, 
                                                            __in_opt PCSTR ImageName, __in ULONG CheckSum, 
                                                            __in ULONG TimeDateStamp)
{
  return CallEventCallback(DEBUG_EVENT_LOAD_MODULE, CLoadModuleEventArgs(m_owner, 
    ImageFileHandle, BaseOffset, ModuleSize, ModuleName, ImageName, CheckSum, TimeDateStamp));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::UnloadModule(__in_opt PCSTR ImageBaseName, __in ULONG64 BaseOffset)
{
  return CallEventCallback(DEBUG_EVENT_UNLOAD_MODULE, CUnloadModuleEventArgs(m_owner, ImageBaseName, BaseOffset));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::SystemError(__in ULONG Error, __in ULONG Level)
{
  return CallEventCallback(DEBUG_EVENT_SYSTEM_ERROR, CSystemErrorEventArgs(m_owner, Error, Level));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::SessionStatus(__in ULONG Status)
{
  return CallEventCallback(DEBUG_EVENT_SESSION_STATUS, CSessionStatusEventArgs(m_owner, Status));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeDebuggeeState(__in ULONG Flags, __in ULONG64 Argument)
{
  return CallEventCallback(DEBUG_EVENT_CHANGE_DEBUGGEE_STATE, CStateChangeEventArgs(m_owner, Flags, Argument));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeEngineState(__in ULONG Flags, __in ULONG64 Argument)
{
  return CallEventCallback(DEBUG_EVENT_CHANGE_ENGINE_STATE, CStateChangeEventArgs(m_owner, Flags, Argument));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeSymbolState(__in ULONG Flags, __in ULONG64 Argument)
{
  return CallEventCallback(DEBUG_EVENT_CHANGE_SYMBOL_STATE, CStateChangeEventArgs(m_owner, Flags, Argument));
}

object CDebugClient::CDebugEventCallbacks::GetEvent(ULONG id) const
{
  CallbackMap::const_iterator it = m_eventCallbacks.find(id);

  return it == m_eventCallbacks.end() ? object(borrowed(Py_None)) : it->second;
}
void CDebugClient::CDebugEventCallbacks::SetEvent(ULONG id, object event) 
{
  if (event.ptr() == Py_None)
    m_eventCallbacks.erase(id);
  else
    m_eventCallbacks.insert(std::make_pair(id, event));
}

void CDebugClient::CDebugEventCallbacks::UpdateEventCallbacks(IDebugEventCallbacks *callbacks) 
{
  if (callbacks && m_callback != callbacks)
  {
    Check(m_owner->GetInterface()->GetEventCallbacks(&m_callback));
    Check(m_owner->GetInterface()->SetEventCallbacks(callbacks));      
  }
  else
  {
    Check(m_owner->GetInterface()->SetEventCallbacks(m_callback));
  }

  m_owner->GetInterface()->FlushCallbacks();
}

void CDebugClient::CDebugInputCallbacks::UpdateIntputCallbacks(IDebugInputCallbacks *callbacks)
{
  if (callbacks && m_callback != callbacks)
  {
    Check(m_owner->GetInterface()->GetInputCallbacks(&m_callback));
    Check(m_owner->GetInterface()->SetInputCallbacks(callbacks));      
  }
  else
  {
    Check(m_owner->GetInterface()->SetInputCallbacks(m_callback));
  }

  m_owner->GetInterface()->FlushCallbacks();
}

void CDebugClient::CDebugOutputCallbacks::UpdateOutputCallbacks(IDebugOutputCallbacks *callbacks)
{
  if (callbacks && m_callback != callbacks)
  {
    Check(m_owner->GetInterface()->GetOutputCallbacks(&m_callback));
    Check(m_owner->GetInterface()->SetOutputCallbacks(callbacks));      
  }
  else
  {
    Check(m_owner->GetInterface()->SetOutputCallbacks(m_callback));
  }

  m_owner->GetInterface()->FlushCallbacks();
}

STDMETHODIMP CDebugClient::CDebugInputCallbacks::StartInput(__in ULONG BufferSize)
{
  if (!m_input) return E_NOTIMPL;

  try
  {
    m_input(CInputEventArgs(m_owner, BufferSize));

    return S_OK;
  }
  catch (error_already_set)
  {
    PyErr_Print();

    return E_FAIL;
  }    
}
STDMETHODIMP CDebugClient::CDebugInputCallbacks::EndInput(void)
{
  return S_OK;
}

STDMETHODIMP CDebugClient::CDebugOutputCallbacks::Output(__in ULONG Mask, __in PCSTR Text)
{
  if (!m_output) return E_NOTIMPL;

  try
  {
    m_output(COutputEventArgs(m_owner, Mask, Text));

    return S_OK;
  }
  catch (error_already_set)
  {
    PyErr_Print();

    return E_FAIL;
  }    
}

ULONG CDebugClient::GetExitCode(void) const
{
  ULONG code;

  Check(m_intf->GetExitCode(&code));

  return code;
}

const std::string CDebugClient::GetIdentity(void) const
{
  char buf[1024];
  ULONG size = _countof(buf);

  Check(m_intf->GetIdentity(buf, size, &size));

  return std::string(buf, size);
}
void CDebugClient::OutputIdentity(const std::string& format, OutputTarget target) const
{
  Check(m_intf->OutputIdentity((ULONG) target, 0, format.c_str()));
}

void CDebugClient::CreateProcess(const std::string& commandLine, const std::string& initialDirectory, 
                                 dict environmentVariables, CDebugClient::CreateFlags createFlags,
                                 AttachUserFlags attachFlags, ULONG attachProcessId, ULONG server) const
{
  bool attach = attachProcessId != 0 || 
                (((ULONG) createFlags) & (DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS)) != 0;

  if (initialDirectory.empty() && empty(environmentVariables))
  {
    if (attach)
    {
      Check(m_intf->CreateProcessAndAttach(server, const_cast<PSTR>(commandLine.c_str()), 
        (ULONG) createFlags, attachProcessId, (ULONG)attachFlags));
    }
    else
    {
      Check(m_intf->CreateProcess(server, const_cast<PSTR>(commandLine.c_str()), (ULONG) createFlags));
    }    
  }
  else
  {   
    list items = environmentVariables.items();
    std::ostringstream oss;

    while (!empty(items))
    {
      tuple item = tuple(items.pop());

      oss << str(item[0]) << "=" << str(item[1]) << std::endl;
    }

    oss << std::endl;

    std::string env = oss.str();

    std::replace(env.begin(), env.end(), '\n', '\0');

    CComQIPtr<IDebugClient5> intf(m_intf);

    if (!intf) RaiseException("The debug engine doesn't support IDebugClient5 interface", PyExc_NotImplementedError);

    DEBUG_CREATE_PROCESS_OPTIONS options = { (ULONG) createFlags, 0, 0, 0 };

    if (attach)
    {
      Check(intf->CreateProcessAndAttach2(server, const_cast<PSTR>(commandLine.c_str()), 
        &options, sizeof(options), const_cast<PSTR>(initialDirectory.c_str()), env.c_str(), 
        attachProcessId, (ULONG)attachFlags));
    }
    else
    {
      Check(intf->CreateProcess2(server, const_cast<PSTR>(commandLine.c_str()), 
        &options, sizeof(options), const_cast<PSTR>(initialDirectory.c_str()), env.c_str()));
    }
  }
}

void CDebugClient::AttachProcess(ULONG id, AttachUserFlags flags, ULONG server) const
{
  Check(m_intf->AttachProcess(server, id, flags));
}
void CDebugClient::DetachProcesses(void) const
{
  Check(m_intf->DetachProcesses());
}
void CDebugClient::TerminateProcesses(void) const
{
  Check(m_intf->TerminateProcesses());
}
void CDebugClient::AbandonCurrentProcess(void) const
{
  CComQIPtr<IDebugClient2> intf(m_intf);

  Check(intf->AbandonCurrentProcess());
}
void CDebugClient::DetachCurrentProcess(void) const
{
  CComQIPtr<IDebugClient2> intf(m_intf);

  Check(intf->DetachCurrentProcess());
}
void CDebugClient::TerminateCurrentProcess(void) const
{
  CComQIPtr<IDebugClient2> intf(m_intf);

  Check(intf->TerminateCurrentProcess());
}

void CDebugClient::AttachKernel(AttachKernelFlag flags, const std::string& connectOptions) const
{
  Check(m_intf->AttachKernel((ULONG) flags, connectOptions.empty() ? NULL : connectOptions.c_str()));
}

bool CDebugClient::IsKernelDebuggerEnabled(void) const
{
  HRESULT hr = CComQIPtr<IDebugClient2>(m_intf)->IsKernelDebuggerEnabled();

  Check(hr);

  return S_OK == hr;
}

const std::string CDebugClient::GetKernelConnectionOptions(void) const
{
  char options[1024];
  ULONG size;

  Check(m_intf->GetKernelConnectionOptions(options, _countof(options), &size));

  return std::string(options, size-1);
}
void CDebugClient::SetKernelConnectionOptions(const std::string& options) const
{
  Check(m_intf->SetKernelConnectionOptions(options.c_str()));
}

bool CDebugClient::DispatchCallbacks(ULONG timeout) const
{
  HRESULT hr = m_intf->DispatchCallbacks(timeout);

  Check(hr);

  return hr == S_OK;
}
void CDebugClient::ExitDispatch(void) const
{
  Check(m_intf->ExitDispatch(m_intf));
}

ULONG CDebugClient::GetNumberOfEventCallbacks(DebugEvent event) const
{
  ULONG count;

  Check(CComQIPtr<IDebugClient5>(m_intf)->GetNumberEventCallbacks((ULONG) event, &count));

  return count;
}
ULONG CDebugClient::GetNumberOfInputCallbacks(void) const
{
  ULONG count;

  Check(CComQIPtr<IDebugClient5>(m_intf)->GetNumberInputCallbacks(&count));

  return count;
}
ULONG CDebugClient::GetNumberOfOutputCallbacks(void) const
{
  ULONG count;

  Check(CComQIPtr<IDebugClient5>(m_intf)->GetNumberOutputCallbacks(&count));

  return count;
}

CDebugClient::OutputMask CDebugClient::GetOutputMask(void) const
{
  ULONG mask;

  Check(m_intf->GetOutputMask(&mask));

  return (OutputMask) mask;
}
void CDebugClient::SetOutputMask(CDebugClient::OutputMask mask) const
{
  Check(m_intf->SetOutputMask((ULONG) mask));
}
ULONG CDebugClient::GetOutputWidth(void) const
{
  ULONG width;

  Check(m_intf->GetOutputMask(&width));

  return width;
}
void CDebugClient::SetOutputWidth(ULONG width) const
{
  Check(m_intf->SetOutputMask((ULONG) width));
}

const std::string CDebugClient::GetQuitLockString(void) const
{
  char szMsg[1024];
  ULONG nMsg = _countof(szMsg);

  Check(CComQIPtr<IDebugClient5>(m_intf)->GetQuitLockString(szMsg, nMsg, &nMsg));

  return std::string(szMsg, nMsg-1);
}
void CDebugClient::SetQuitLockString(const std::string str) const
{
  Check(CComQIPtr<IDebugClient5>(m_intf)->SetQuitLockString(str.c_str())); 
}

void CDebugClient::OpenDumpFile(const std::string& filename) const
{
  Check(m_intf->OpenDumpFile(filename.c_str()));
}
void CDebugClient::AddDumpInformationFile(const std::string& filename) const
{
  Check(CComQIPtr<IDebugClient2>(m_intf)->AddDumpInformationFile(filename.c_str(), DEBUG_DUMP_FILE_PAGE_FILE_DUMP));
}
const dict CDebugClient::GetDumpFiles(void) const
{
  CComQIPtr<IDebugClient4> intf(m_intf);

  ULONG number;

  Check(intf->GetNumberDumpFiles(&number));

  char szFile[MAX_PATH];
  ULONG nFile, nType;
  ULONG64 handle;

  dict files;

  for (ULONG i=0; i<number; i++)
  {
    Check(intf->GetDumpFile(i, szFile, _countof(szFile), &nFile, &handle, &nType));

    files[std::string(szFile, nFile-1)] = make_tuple(handle, nType);
  }

  return files;
}

void CDebugClient::WriteUserDumpFile(const std::string& filename, UserQualifier qualifier, DumpFormat format, const std::string& comment) const
{
  if (FORMAT_DEFAULT == format && comment.empty())
    Check(m_intf->WriteDumpFile(filename.c_str(), static_cast<ULONG>(qualifier)));
  else
    Check(CComQIPtr<IDebugClient2>(m_intf)->WriteDumpFile2(
      filename.c_str(), static_cast<ULONG>(qualifier), (ULONG) format, comment.empty() ? comment.c_str() : NULL));
}

void CDebugClient::WriteKernelDumpFile(const std::string& filename, KernelQualifier qualifier, DumpFormat format, const std::string& comment) const
{
  if (FORMAT_DEFAULT == format && comment.empty())
    Check(m_intf->WriteDumpFile(filename.c_str(), static_cast<ULONG>(qualifier)));
  else
    Check(CComQIPtr<IDebugClient2>(m_intf)->WriteDumpFile2(
    filename.c_str(), static_cast<ULONG>(qualifier), (ULONG) format, comment.empty() ? comment.c_str() : NULL));

}