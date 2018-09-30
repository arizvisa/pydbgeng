#include "StdAfx.h"
#include "utils.h"

#include "DebugObject.h"
#include "DebugOutput.h"
#include "DebugBreakpoint.h"
#include "DebugRegisters.h"
#include "DebugSymbols.h"
#include "DebugDataSpaces.h"
#include "DebugSystemObjects.h"
#include "DebugAdvanced.h"
#include "DebugControl.h"

#include "DebugClient.h"

#include <sstream>
#include <list>
#include <queue>

#include <boost/python.hpp>
using namespace boost::python;

void CDebugClient::Export(void)
{
	// CDebugClient enums
	enum_<AttachKernelFlags>("AttachKernelFlags")
		.value("KERNEL_CONNECTION", AttachKernelFlags::ATTACH_KERNEL_CONNECTION)
		.value("LOCAL_KERNEL",			AttachKernelFlags::ATTACH_LOCAL_KERNEL)		 // Attach to the local machine.	If this flag is not set a connection is made to a separate target machine using the given connection options.
		.value("EXDI_DRIVER",				AttachKernelFlags::ATTACH_EXDI_DRIVER)		 // Attach to an eXDI driver.
		.value("INSTALL_DRIVER",		AttachKernelFlags::ATTACH_INSTALL_DRIVER)  // Just install client-side transpotr drivers
	;

	enum_<RunningProcessSystemIdFlags>("RunningProcessSystemIdFlags")
		.value("DEFAULT",			 RunningProcessSystemIdFlags::GET_PROC_DEFAULT)			 // By default the match allows a tail match on just the filename.	The match returns the first hit even if multiple matches exist.
		.value("FULL_MATCH",	 RunningProcessSystemIdFlags::GET_PROC_FULL_MATCH)	 // The name must match fully.
		.value("ONLY_MATCH",	 RunningProcessSystemIdFlags::GET_PROC_ONLY_MATCH)	 // The match must be the only match.
		.value("SERVICE_NAME", RunningProcessSystemIdFlags::GET_PROC_SERVICE_NAME) // The name is a service name instead of an executable name.
	;

	enum_<RunningProcessDescriptionFlags>("RunningProcessDescriptionFlags")
		.value("DEFAULT",					RunningProcessDescriptionFlags::PROC_DESC_DEFAULT)
		.value("NO_PATHS",				RunningProcessDescriptionFlags::PROC_DESC_NO_PATHS)				 // Return only filenames, not full paths.
		.value("NO_SERVICES",			RunningProcessDescriptionFlags::PROC_DESC_NO_SERVICES)		 // Dont look up service names.
		.value("NO_MTS_PACKAGES", RunningProcessDescriptionFlags::PROC_DESC_NO_MTS_PACKAGES) // Dont look up MTS package names.
		.value("NO_COMMAND_LINE", RunningProcessDescriptionFlags::PROC_DESC_NO_COMMAND_LINE) // Dont retrieve the command line.
		.value("NO_SESSION_ID",		RunningProcessDescriptionFlags::PROC_DESC_NO_SESSION_ID)	 // Dont retrieve the session ID.
		.value("NO_USER_NAME",		RunningProcessDescriptionFlags::PROC_DESC_NO_USER_NAME)		 // Dont retrieve the process's user name.
	;

	enum_<AttachFlags>("AttachFlags")
		.value("DEFAULT",										AttachFlags::ATTACH_DEFAULT)
		.value("NONINVASIVE",								AttachFlags::ATTACH_NONINVASIVE)							 // When attaching to a process just examine the process state and suspend the threads.  DebugActiveProcess is not called so the process is not actually being debugged.	This is useful for debugging processes holding locks which interfere with the operation of DebugActiveProcess or in situations where it is not desirable to actually set up as a debugger.
		.value("EXISTING",									AttachFlags::ATTACH_EXISTING)									 // Attempt to attach to a process that was abandoned when being debugged.	This is only supported in some system versions.  This flag also allows multiple debuggers to attach to the same process, which can result in numerous problems unless very carefully managed.
		.value("NONINVASIVE_NO_SUSPEND",		AttachFlags::ATTACH_NONINVASIVE_NO_SUSPEND)		 // When attaching non-invasively, do not suspend threads.	It is the callers responsibility to either suspend the threads itself or be aware that the attach state may not reflect the current state of the process if threads are still running.
		.value("INVASIVE_NO_INITIAL_BREAK", AttachFlags::ATTACH_INVASIVE_NO_INITIAL_BREAK) // When doing an invasive attach do not inject a break-in thread to generate the initial break-in event.  This can be useful to save resources when an initial break is not necessary or when injecting a thread might affect the debuggee's state.	This option is only supported on Windows XP and above.
		.value("INVASIVE_RESUME_PROCESS",		AttachFlags::ATTACH_INVASIVE_RESUME_PROCESS)	 // When doing an invasive attach resume all threads at the time of attach.  This makes it possible to attach to a process created suspended and cause it to start running.
		.value("NONINVASIVE_ALLOW_PARTIAL", AttachFlags::ATTACH_NONINVASIVE_ALLOW_PARTIAL) // When doing a non-invasive attach the engine must recover information for all debuggee elements.  The engine may not have permissions for all elements, for example it may not be able to open all threads, and that would ordinarily block the attach.	This flag allows unusable elements to be ignored.
	;

	enum_<ProcessOptions>("ProcessOptions")
		.value("DETACH_ON_EXIT",		ProcessOptions::PROCESS_DETACH_ON_EXIT)		 // Indicates that the debuggee process should be automatically detached when the debugger exits.  A debugger can explicitly detach on exit or this flag can be set so that detach occurs regardless of how the debugger exits.  This is only supported on some system versions.
		.value("ONLY_THIS_PROCESS", ProcessOptions::PROCESS_ONLY_THIS_PROCESS) // Indicates that processes created by the current process should not be debugged.  Modifying this flag is only supported on some system versions.
	;

	enum_<ConnectSessionFlags>("ConnectSessionFlags")
		.value("DEFAULT",			ConnectSessionFlags::CONNECT_SESSION_DEFAULT)			// Default connect.
		.value("NO_VERSION",	ConnectSessionFlags::CONNECT_SESSION_NO_VERSION)	// Do not output the debugger version.
		.value("NO_ANNOUNCE", ConnectSessionFlags::CONNECT_SESSION_NO_ANNOUNCE) // Do not announce the connection.
	;

	enum_<OutputServersFlags>("OutputServersFlags")
		.value("DEBUGGER", OutputServersFlags::OUTPUT_SERVERS_DEBUGGER) // Debugger servers from StartSever.
		.value("PROCESS",  OutputServersFlags::OUTPUT_SERVERS_PROCESS)	// Process servers from StartProcessServer.
		.value("ALL",			 OutputServersFlags::OUTPUT_SERVERS_ALL)
	;

	enum_<EndSessionFlags>("EndSessionFlags")
		.value("PASSIVE",		 EndSessionFlags::END_PASSIVE)					// Perform cleanup for the session.
		.value("TERMINATE",  EndSessionFlags::END_ACTIVE_TERMINATE) // Actively terminate the session and then perform cleanup.
		.value("DETACH",		 EndSessionFlags::END_ACTIVE_DETACH)		// If possible, detach from all processes and then perform cleanup.
		.value("REENTRANT",  EndSessionFlags::END_REENTRANT)				// Perform whatever cleanup is possible that doesn't require acquiring any locks.  This is useful for situations where a thread is currently using the engine but the application needs to exit and still wants to give the engine the opportunity to clean up as much as possible.  This may leave the engine in an indeterminate state so further engine calls should not be made.	When making a reentrant EndSession call from a remote client it is the callers responsibility to ensure that the server can process the request.	It is best to avoid making such calls.
		.value("DISCONNECT", EndSessionFlags::END_DISCONNECT)				// Notify a server that a remote client is disconnecting.  This isnt required but if it isnt called then no disconnect messages will be generated by the server.
	;

	enum_<ClientIdentifier>("ClientIdentifier")
		.value("UNKNOWN", ClientIdentifier::CLIENT_UNKNOWN)
		.value("VSINT",		ClientIdentifier::CLIENT_VSINT)
		.value("NTSD",		ClientIdentifier::CLIENT_NTSD)
		.value("NTKD",		ClientIdentifier::CLIENT_NTKD)
		.value("CDB",			ClientIdentifier::CLIENT_CDB)
		.value("KD",			ClientIdentifier::CLIENT_KD)
		.value("WINDBG",	ClientIdentifier::CLIENT_WINDBG)
		.value("WINIDE",	ClientIdentifier::CLIENT_WINIDE)
	;

	enum_<DumpFormat>("DumpFormat")
		.value("DEFAULT",																DumpFormat::FORMAT_DEFAULT)
		.value("CAB_SECONDARY_ALL_IMAGES",							DumpFormat::FORMAT_CAB_SECONDARY_ALL_IMAGES) // When creating a CAB with secondary images do searches for all image files, regardless of whether they're needed for the current session or not.
		.value("WRITE_CAB",															DumpFormat::FORMAT_WRITE_CAB)								 // Write dump to a temporary file, then package it into a CAB file and delete the temporary file.
		.value("CAB_SECONDARY_FILES",										DumpFormat::FORMAT_CAB_SECONDARY_FILES)			 // When creating a CAB add secondary files such as current symbols and mapped images.
		.value("NO_OVERWRITE",													DumpFormat::FORMAT_NO_OVERWRITE)						 // Don't overwrite existing files.
		.value("USER_SMALL_FULL_MEMORY",								DumpFormat::FORMAT_USER_SMALL_FULL_MEMORY)
		.value("USER_SMALL_HANDLE_DATA",								DumpFormat::FORMAT_USER_SMALL_HANDLE_DATA)
		.value("USER_SMALL_UNLOADED_MODULES",						DumpFormat::FORMAT_USER_SMALL_UNLOADED_MODULES)
		.value("USER_SMALL_INDIRECT_MEMORY",						DumpFormat::FORMAT_USER_SMALL_INDIRECT_MEMORY)
		.value("USER_SMALL_DATA_SEGMENTS",							DumpFormat::FORMAT_USER_SMALL_DATA_SEGMENTS)
		.value("USER_SMALL_FILTER_MEMORY",							DumpFormat::FORMAT_USER_SMALL_FILTER_MEMORY)
		.value("USER_SMALL_FILTER_PATHS",								DumpFormat::FORMAT_USER_SMALL_FILTER_PATHS)
		.value("USER_SMALL_PROCESS_THREAD_DATA",				DumpFormat::FORMAT_USER_SMALL_PROCESS_THREAD_DATA)
		.value("USER_SMALL_PRIVATE_READ_WRITE_MEMORY",	DumpFormat::FORMAT_USER_SMALL_PRIVATE_READ_WRITE_MEMORY)
		.value("USER_SMALL_NO_OPTIONAL_DATA",						DumpFormat::FORMAT_USER_SMALL_NO_OPTIONAL_DATA)
		.value("USER_SMALL_FULL_MEMORY_INFO",						DumpFormat::FORMAT_USER_SMALL_FULL_MEMORY_INFO)
		.value("USER_SMALL_THREAD_INFO",								DumpFormat::FORMAT_USER_SMALL_THREAD_INFO)
		.value("USER_SMALL_CODE_SEGMENTS",							DumpFormat::FORMAT_USER_SMALL_CODE_SEGMENTS)
		.value("USER_SMALL_NO_AUXILIARY_STATE",					DumpFormat::FORMAT_USER_SMALL_NO_AUXILIARY_STATE)
		.value("USER_SMALL_FULL_AUXILIARY_STATE",				DumpFormat::FORMAT_USER_SMALL_FULL_AUXILIARY_STATE)
		.value("USER_SMALL_MODULE_HEADERS",							DumpFormat::FORMAT_USER_SMALL_MODULE_HEADERS)
		.value("USER_SMALL_FILTER_TRIAGE",							DumpFormat::FORMAT_USER_SMALL_FILTER_TRIAGE)
		.value("USER_SMALL_IGNORE_INACCESSIBLE_MEM",		DumpFormat::FORMAT_USER_SMALL_IGNORE_INACCESSIBLE_MEM)
	;
	////
#if 0
	enum_<CreateFlags>("CreateFlags")
		.value("DEFAULT",										CREATE_DEFAULT)
		.value("ATTACH_PROCESS",						CREATE_ATTACH_PROCESS)
		.value("ATTACH_ONLY_THIS_PROCESS",	CREATE_ATTACH_ONLY_THIS_PROCESS)
		.value("SUSPENDED",									CREATE_SUSPENDED_)
		.value("NO_DEBUG_HEAP",							CREATE_NO_DEBUG_HEAP)
		.value("THROUGH_RTL",								CREATE_THROUGH_RTL)
	;
#endif

	// CDebugClient object
	scope DebugClient = class_<CDebugClient>("DebugClient", no_init)
		.add_property("Control", make_function(&CDebugClient::GetControl, return_value_policy<return_by_value>()))
		.add_property("Registers", make_function(&CDebugClient::GetRegisters, return_value_policy<return_by_value>()))
		.add_property("Symbols", make_function(&CDebugClient::GetSymbols, return_value_policy<return_by_value>()))
		.add_property("DataSpaces", make_function(&CDebugClient::GetDataSpaces, return_value_policy<return_by_value>()))
		.add_property("SystemObjects", make_function(&CDebugClient::GetSystemObjects, return_value_policy<return_by_value>()))

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
			(arg("format")="%s", arg("target")=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS),
			"format and output a string describing the computer and user this client represents.")

		.add_property("OutputMask", &CDebugClient::GetOutputMask, &CDebugClient::SetOutputMask,
			"the output mask currently set for the client.")
		.add_property("OutputWidth", &CDebugClient::GetOutputWidth, &CDebugClient::SetOutputWidth,
			"the width of the output display for this client object.")
		.add_property("QuitLockString", &CDebugClient::GetQuitLockString, &CDebugClient::SetQuitLockString,
			"the string used to prevent the user from accidentally ending the debugging session.")
/* FIXME
		.def("CreateProcess", &CDebugClient::CreateProcess,
			("commandLine", arg("initialDirectory")=std::string(), arg("environmentVariables")=dict(),
			 arg("createFlags")=CREATE_DEFAULT, arg("attachFlags")=ATTACH_DEFAULT,
			 arg("attachProcessId")=0, arg("server")=DEFAULT_PROCESS_SERVER),
			"create a process from the specified command line.")
*/
		.def("AttachProcess", &CDebugClient::AttachProcess,
			("id", arg("flags")=AttachFlags::ATTACH_DEFAULT, arg("server")=DEFAULT_PROCESS_SERVER),
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
			(arg("flags")=AttachKernelFlags::ATTACH_LOCAL_KERNEL, arg("connectOptions")=std::string()),
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
			("filename", arg("qualifier")=CDebugControl::UserDebuggee::USER_SMALL_DUMP, arg("format")=DumpFormat::FORMAT_DEFAULT, arg("comment")=std::string()),
			"creates a user-mode crash dump file.")
		.def("WriteDumpFile", &CDebugClient::WriteKernelDumpFile,
			("filename", arg("qualifier")=CDebugControl::KernelDebuggee::KERNEL_SMALL_DUMP, arg("format")=DumpFormat::FORMAT_DEFAULT, arg("comment")=std::string()),
			"creates a kernel-mode crash dump file.")

		.def("GetRunningProcessSystemIds", &CDebugClient::GetRunningProcessSystemIds,
			(arg("server")=DEFAULT_PROCESS_SERVER),
			"returns a list of all the process ids on the specified system.")
		.def("GetRunningProcessDescription", &CDebugClient::GetRunningProcessDescription,
			(arg("flags")=AttachFlags::ATTACH_DEFAULT, arg("server")=DEFAULT_PROCESS_SERVER),
			"returns information about a process id on the specified system.")
		.def("ConnectProcessServer", &CDebugClient::ConnectProcessServer,
			arg("remoteOptions")=std::string(),
			"connect the debugger engine to a process server. return it's id.")
		.def("DisconnectProcessServer", &CDebugClient::DisconnectProcessServer,
			arg("Server"),
			"disconnect the debugger engine from a process server.")
	;

	CDebugEventCallbacks::Export();
	CDebugInputCallbacks::Export();
	CDebugOutputCallbacks::Export();
} 
void CDebugClient::CDebugEventCallbacks::Export(void)
{
	enum_<CDebugEventCallbacks::InterestMask>("InterestMask")
		.value("BREAKPOINT",						InterestMask::EVENT_BREAKPOINT)
		.value("EXCEPTION",							InterestMask::EVENT_EXCEPTION)
		.value("CREATE_THREAD",					InterestMask::EVENT_CREATE_THREAD)
		.value("EXIT_THREAD",						InterestMask::EVENT_EXIT_THREAD)
		.value("CREATE_PROCESS",				InterestMask::EVENT_CREATE_PROCESS)
		.value("EXIT_PROCESS",					InterestMask::EVENT_EXIT_PROCESS)
		.value("LOAD_MODULE",						InterestMask::EVENT_LOAD_MODULE)
		.value("UNLOAD_MODULE",					InterestMask::EVENT_UNLOAD_MODULE)
		.value("SYSTEM_ERROR",					InterestMask::EVENT_SYSTEM_ERROR)
		.value("SESSION_STATUS",				InterestMask::EVENT_SESSION_STATUS)
		.value("CHANGE_DEBUGGEE_STATE", InterestMask::EVENT_CHANGE_DEBUGGEE_STATE)
		.value("CHANGE_ENGINE_STATE",		InterestMask::EVENT_CHANGE_ENGINE_STATE)
		.value("CHANGE_SYMBOL_STATE",		InterestMask::EVENT_CHANGE_SYMBOL_STATE)
	;

	enum_<CDebugEventCallbacks::SessionStatusFlags>("SessionStatusFlags")
		.value("ACTIVE",												SessionStatusFlags::SESSION_ACTIVE)												// A debuggee has been discovered for the session.
		.value("END_SESSION_ACTIVE_TERMINATE",	SessionStatusFlags::SESSION_END_SESSION_ACTIVE_TERMINATE) // The session has been ended by EndSession.
		.value("END_SESSION_ACTIVE_DETACH",			SessionStatusFlags::SESSION_END_SESSION_ACTIVE_DETACH)
		.value("END_SESSION_PASSIVE",						SessionStatusFlags::SESSION_END_SESSION_PASSIVE)
		.value("END",														SessionStatusFlags::SESSION_END)													// The debuggee has run to completion.	User-mode only.
		.value("REBOOT",												SessionStatusFlags::SESSION_REBOOT)												// The target machine has rebooted.  Kernel-mode only.
		.value("HIBERNATE",											SessionStatusFlags::SESSION_HIBERNATE)										// The target machine has hibernated.  Kernel-mode only.
		.value("FAILURE",												SessionStatusFlags::SESSION_FAILURE)											// The engine was unable to continue the session.
	;

	enum_<CDebugEventCallbacks::ChangeDebuggeeStateFlags>("ChangeDebuggeeStateFlags")
		.value("ALL",				ChangeDebuggeeStateFlags::CDS_ALL)			 // The debuggees state has changed generally, such as when the debuggee has been executing.	Argument is zero.  Otherwise it is DEBUG_ANY_ID.
		.value("REGISTERS", ChangeDebuggeeStateFlags::CDS_REGISTERS) // Registers have changed.  If only a single register changed, argument is the index of the register.
		.value("DATA",			ChangeDebuggeeStateFlags::CDS_DATA)			 // Data spaces have changed.  If only a single space was affected, argument is the data space.  Otherwise it is DEBUG_ANY_ID.
		.value("REFRESH",		ChangeDebuggeeStateFlags::CDS_REFRESH)	 // Inform the GUI clients to refresh debugger windows.
	;

	enum_<CDebugEventCallbacks::ChangeDebuggeeStateRefreshIds>("ChangeDebuggeeStateRefreshIds")
		.value("EVALUATE",									ChangeDebuggeeStateRefreshIds::REFRESH_EVALUATE)
		.value("EXECUTE",										ChangeDebuggeeStateRefreshIds::REFRESH_EXECUTE)
		.value("EXECUTECOMMANDFILE",				ChangeDebuggeeStateRefreshIds::REFRESH_EXECUTECOMMANDFILE)
		.value("ADDBREAKPOINT",							ChangeDebuggeeStateRefreshIds::REFRESH_ADDBREAKPOINT)
		.value("REMOVEBREAKPOINT",					ChangeDebuggeeStateRefreshIds::REFRESH_REMOVEBREAKPOINT)
		.value("WRITEVIRTUAL",							ChangeDebuggeeStateRefreshIds::REFRESH_WRITEVIRTUAL)
		.value("WRITEVIRTUALUNCACHED",			ChangeDebuggeeStateRefreshIds::REFRESH_WRITEVIRTUALUNCACHED)
		.value("WRITEPHYSICAL",							ChangeDebuggeeStateRefreshIds::REFRESH_WRITEPHYSICAL)
		.value("WRITEPHYSICAL2",						ChangeDebuggeeStateRefreshIds::REFRESH_WRITEPHYSICAL2)
		.value("SETVALUE",									ChangeDebuggeeStateRefreshIds::REFRESH_SETVALUE)
		.value("SETVALUE2",									ChangeDebuggeeStateRefreshIds::REFRESH_SETVALUE2)
		.value("SETSCOPE",									ChangeDebuggeeStateRefreshIds::REFRESH_SETSCOPE)
		.value("SETSCOPEFRAMEBYINDEX",			ChangeDebuggeeStateRefreshIds::REFRESH_SETSCOPEFRAMEBYINDEX)
		.value("SETSCOPEFROMJITDEBUGINFO",	ChangeDebuggeeStateRefreshIds::REFRESH_SETSCOPEFROMJITDEBUGINFO)
		.value("SETSCOPEFROMSTOREDEVENT",		ChangeDebuggeeStateRefreshIds::REFRESH_SETSCOPEFROMSTOREDEVENT)
		.value("INLINESTEP",								ChangeDebuggeeStateRefreshIds::REFRESH_INLINESTEP)
		.value("INLINESTEP_PSEUDO",					ChangeDebuggeeStateRefreshIds::REFRESH_INLINESTEP_PSEUDO)
	;

	enum_<CDebugEventCallbacks::ChangeEngineStateFlags>("ChangeEngineStateFlags")
		.value("ALL",									ChangeEngineStateFlags::CES_ALL)								 // The engine state has changed generally.  Argument is zero.
		.value("CURRENT_THREAD",			ChangeEngineStateFlags::CES_CURENT_THREAD)			 // Current thread changed.  This may imply a change of system and process also.	Argument is the ID of the new current thread or DEBUG_ANY_ID if no thread is current.
		.value("EFFECTIVE_PROCESSOR", ChangeEngineStateFlags::CES_EFFECTIVE_PROCESSOR) // Effective processor changed.	Argument is the new processor type.
		.value("BREAKPOINTS",					ChangeEngineStateFlags::CES_BREAKPOINTS)				 // Breakpoints changed.	If only a single breakpoint changed, argument is the ID of the breakpoint.	Otherwise it is DEBUG_ANY_ID.
		.value("CODE_LEVEL",					ChangeEngineStateFlags::CES_CODE_LEVEL)					 // Code interpretation level changed.	Argument is the new level.
		.value("EXECUTION_STATUS",		ChangeEngineStateFlags::CES_EXECUTION_STATUS)		 // Execution status changed.  Argument is the new execution status.
		.value("ENGINE_OPTIONS",			ChangeEngineStateFlags::CES_ENGINE_OPTIONS)			 // Engine options have changed.	Argument is the new options value.
		.value("LOG_FILE",						ChangeEngineStateFlags::CES_LOG_FILE)						 // Log file information has changed.  Argument is TRUE if a log file was opened and FALSE if a log file was closed.
		.value("RADIX",								ChangeEngineStateFlags::CES_RADIX)							 // Default number radix has changed.  Argument is the new radix.
		.value("EVENT_FILTERS",				ChangeEngineStateFlags::CES_EVENT_FILTERS)			 // Event filters changed.	If only a single filter changed the argument is the filter's index, otherwise it is DEBUG_ANY_ID.
		.value("PROCESS_OPTIONS",			ChangeEngineStateFlags::CES_PROCESS_OPTIONS)		 // Process options have changed.  Argument is the new options value.
		.value("EXTENSIONS",					ChangeEngineStateFlags::CES_EXTENSIONS)					 // Extensions have been added or removed.
		.value("SYSTEMS",							ChangeEngineStateFlags::CES_SYSTEMS)						 // Systems have been added or removed.  The argument is the system ID.  Systems, unlike processes and threads, may be created at any time and not just during WaitForEvent.
		.value("ASSEMBLY_OPTIONS",		ChangeEngineStateFlags::CES_ASSEMBLY_OPTIONS)		 // Assembly/disassembly options have changed.	Argument is the new options value.
		.value("EXPRESSION_SYNTAX",		ChangeEngineStateFlags::CES_EXPRESSION_SYNTAX)	 // Expression syntax has changed.	Argument is the new syntax value.
		.value("TEXT_REPLACEMENTS",		ChangeEngineStateFlags::CES_TEXT_REPLACEMENTS)	 // Text replacements have changed.
	;

	enum_<CDebugEventCallbacks::ChangeSymbolStateFlags>("ChangeSymbolStateFlags")
		.value("ALL",							ChangeSymbolStateFlags::CSS_ALL)						// Symbol state has changed generally, such as after reload operations.  Argument is zero.
		.value("LOADS",						ChangeSymbolStateFlags::CSS_LOADS)					// Modules have been loaded.	If only a single module changed, argument is the base address of the module.	Otherwise it is zero.
		.value("UNLOADS",					ChangeSymbolStateFlags::CSS_UNLOADS)				// Modules have been unloaded.	If only a single module changed, argument is the base address of the module.	Otherwise it is zero.
		.value("SCOPE",						ChangeSymbolStateFlags::CSS_SCOPE)					// Current symbol scope changed.
		.value("PATHS",						ChangeSymbolStateFlags::CSS_PATHS)					// Paths have changed.
		.value("SYMBOL_OPTIONS",	ChangeSymbolStateFlags::CSS_SYMBOL_OPTIONS) // Symbol options have changed.  Argument is the new options value.
		.value("TYPE_OPTIONS",		ChangeSymbolStateFlags::CSS_TYPE_OPTIONS)		// Type options have changed.  Argument is the new options value.
	;

	/// CDebugCallbacks
	class_<CDebugCallbacksBase, boost::noncopyable>("DebugCallbacksBase", no_init)
	;
	class_<CComObjectStackEx<CDebugInputCallbacks>, bases<CDebugCallbacksBase>>("DebugInputCallbacks", no_init)
		.add_property("Input", &CDebugInputCallbacks::GetInput, &CDebugInputCallbacks::SetInput)
	;
	class_<CComObjectStackEx<CDebugOutputCallbacks>, bases<CDebugCallbacksBase>>("DebugOutputCallbacks", no_init)
		.add_property("Output", &CDebugOutputCallbacks::GetOutput, &CDebugOutputCallbacks::SetOutput)
	;

#define DEF_EVENT(name, doc) \
		.add_property(#name, \
									&CDebugEventCallbacks::Get##name, \
									&CDebugEventCallbacks::Set##name, \
									doc)

	class_<CComObjectStackEx<CDebugEventCallbacks>, bases<CDebugCallbacksBase>>("DebugEventCallbacks", no_init)
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
#undef DEF_EVENT

	class_<CEventArgs>("EventArgs", no_init)
		.add_property("Source", make_function(&CEventArgs::GetOwner, return_value_policy<return_by_value>()))
	;

	class_<CBreakpointEventArgs, bases<CEventArgs>>("BreakpointEventArgs", no_init)
		.add_property("Breakpoint", make_function(&CBreakpointEventArgs::GetBreakpoint, return_value_policy<return_by_value>()))
	;

	class_<CExceptionEventArgs, bases<CEventArgs>>("ExceptionEventArgs", no_init)
		.add_property("IsFirstChance", &CExceptionEventArgs::IsFirstChance)
		.add_property("Code", &CExceptionEventArgs::GetExceptionCode)
		.add_property("Flags", &CExceptionEventArgs::GetExceptionFlags)
		.add_property("Address", &CExceptionEventArgs::GetExceptionAddress)
		.add_property("Parameters", &CExceptionEventArgs::GetParameters)
	;

	class_<CCreateThreadEventArgs, bases<CEventArgs>>("CreateThreadEventArgs", no_init)
		.add_property("Handle", &CCreateThreadEventArgs::GetHandle)
		.add_property("DataOffset", &CCreateThreadEventArgs::GetDataOffset)
		.add_property("StartOffset", &CCreateThreadEventArgs::GetStartOffset)
	;

	class_<CExitThreadEventArgs, bases<CEventArgs>>("ExitThreadEventArgs", no_init)
		.add_property("ExitCode", &CExitThreadEventArgs::GetExitCode)
	;

	class_<CCreateProcessEventArgs, bases<CEventArgs>>("CreateProcessEventArgs", no_init)
		.add_property("ImageFileHandle", &CCreateProcessEventArgs::GetImageFileHandle)
		.add_property("Handle", &CCreateProcessEventArgs::GetHandle)
		.add_property("BaseOffset", &CCreateProcessEventArgs::GetBaseOffset)
		.add_property("ModuleSize", &CCreateProcessEventArgs::GetModuleSize)
		.add_property("ModuleName", &CCreateProcessEventArgs::GetModuleName)
		.add_property("ImageName", &CCreateProcessEventArgs::GetImageName)
		.add_property("CheckSum", &CCreateProcessEventArgs::GetCheckSum)
		.add_property("TimeDateStamp", &CCreateProcessEventArgs::GetTimeDateStamp)
		.add_property("InitialThreadHandle", &CCreateProcessEventArgs::GetInitialThreadHandle)
		.add_property("ThreadDataOffset", &CCreateProcessEventArgs::GetThreadDataOffset)
		.add_property("StartOffset", &CCreateProcessEventArgs::GetStartOffset)
	;

	class_<CExitProcessEventArgs, bases<CEventArgs>>("ExitProcessEventArgs", no_init)
		.add_property("ExitCode", &CExitProcessEventArgs::GetExitCode)
	;

	class_<CLoadModuleEventArgs, bases<CEventArgs>>("LoadModuleEventArgs", no_init)
		.add_property("ImageFileHandle", &CLoadModuleEventArgs::GetImageFileHandle)
		.add_property("BaseOffset", &CLoadModuleEventArgs::GetBaseOffset)
		.add_property("ModuleSize", &CLoadModuleEventArgs::GetModuleSize)
		.add_property("ModuleName", &CLoadModuleEventArgs::GetModuleName)
		.add_property("ImageName", &CLoadModuleEventArgs::GetImageName)
		.add_property("CheckSum", &CLoadModuleEventArgs::GetCheckSum)
		.add_property("TimeDateStamp", &CLoadModuleEventArgs::GetTimeDateStamp)
	;

	class_<CUnloadModuleEventArgs, bases<CEventArgs>>("UnloadModuleEventArgs", no_init)
		.add_property("ImageBaseName", &CUnloadModuleEventArgs::GetImageBaseName)
		.add_property("BaseOffset", &CUnloadModuleEventArgs::GetBaseOffset)
	;

	class_<CSystemErrorEventArgs, bases<CEventArgs>>("SystemErrorEventArgs", no_init)
		.add_property("Error", &CSystemErrorEventArgs::GetError)
		.add_property("Level", &CSystemErrorEventArgs::GetLevel)
	;

	class_<CSessionStatusEventArgs, bases<CEventArgs>>("SessionStatusEventArgs", no_init)
		.add_property("Status", &CSessionStatusEventArgs::GetStatus)
	;

	class_<CStateChangeEventArgs<CDebugEventCallbacks::ChangeEngineStateFlags>, bases<CEventArgs>>("ChangeEngineStateArgs", no_init)
		.add_property("Flags", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeEngineStateFlags>::GetFlags)
		.add_property("Argument", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeEngineStateFlags>::GetArgument)
	;
	class_<CStateChangeEventArgs<CDebugEventCallbacks::ChangeDebuggeeStateFlags>, bases<CEventArgs>>("ChangeDebuggeeStateArgs", no_init)
		.add_property("Flags", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeDebuggeeStateFlags>::GetFlags)
		.add_property("Argument", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeDebuggeeStateFlags>::GetArgument)
	;
	class_<CStateChangeEventArgs<CDebugEventCallbacks::ChangeSymbolStateFlags>, bases<CEventArgs>>("ChangeSymbolStateArgs", no_init)
		.add_property("Flags", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeSymbolStateFlags>::GetFlags)
		.add_property("Argument", &CStateChangeEventArgs<CDebugEventCallbacks::ChangeSymbolStateFlags>::GetArgument)
	;
}

STDMETHODIMP CDebugClient::CDebugInputCallbacks::StartInput(__in ULONG BufferSize)
{
	HRESULT hr;
	if (!m_input)
		return E_NOTIMPL;

	try {
		m_input(CInputEventArgs(m_intf, BufferSize));
		hr = S_OK;
	} catch (error_already_set) {
		PyErr_Print();
		hr = E_FAIL;
	}
	return hr;
}
STDMETHODIMP CDebugClient::CDebugInputCallbacks::EndInput(void)
{
	return S_OK;
}

STDMETHODIMP CDebugClient::CDebugOutputCallbacks::Output(__in ULONG Mask, __in PCSTR Text)
{
	HRESULT hr;
	if (!m_output)
		return E_NOTIMPL;

	try {
		m_output(COutputEventArgs(m_intf, Mask, Text));
		hr = S_OK;
	} catch (error_already_set) {
		PyErr_Print();
		hr = E_FAIL;
	}
	return hr;
}

ULONG CDebugClient::GetExitCode(void) const
{
	ULONG code;

	m_intf->GetExitCode(&code);
	return code;
}

const std::string CDebugClient::GetIdentity(void) const
{
	char buf[1024];
	ULONG size = _countof(buf);

	m_intf->GetIdentity(buf, size, &size);
	return std::string(buf, size);
}
void CDebugClient::OutputIdentity(const std::string& format, CDebugControl::OutputControl target) const
{
	m_intf->OutputIdentity(static_cast<ULONG>(target), 0, format.c_str());
}

// FIXME: wrap the flags in a sane fashion
/*
void CDebugClient::CreateProcess(const std::string& commandLine, const std::string& initialDirectory,
																 dict environmentVariables, CDebugClient::CreateFlags createFlags,
																 CDebugClient::AttachFlags attachFlags, ULONG attachProcessId, ULONG64 server) const
{
	HRESULT hr;
	bool attach = attachProcessId != 0 ||
								((static_cast<ULONG>(createFlags)) & (DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS)) != 0;

	if (initialDirectory.empty() && utils::empty(environmentVariables))
		if (attach) {
			hr = m_intf->CreateProcessAndAttach(server, const_cast<PSTR>(commandLine.c_str()),
				static_cast<ULONG>(createFlags), attachProcessId, static_cast<ULONG>(attachFlags));
		} else
			hr = m_intf->CreateProcess(server, const_cast<PSTR>(commandLine.c_str()), static_cast<ULONG>(createFlags));
	else {
		list items = environmentVariables.items();
		std::ostringstream oss;

		while (!utils::empty(items)) {
			tuple item = tuple(items.pop());
			oss << utils::str(item[0]) << "=" << utils::str(item[1]) << std::endl;
		}
		oss << std::endl;

		std::string env = oss.str();
		std::replace(env.begin(), env.end(), '\n', '\0');

		CComQIPtr<IDebugClient5> intf(m_intf);
		if (!intf) utils::RaiseException("The debug engine doesn't support IDebugClient5 interface", PyExc_NotImplementedError);

		DEBUG_CREATE_PROCESS_OPTIONS options = { static_cast<ULONG>(createFlags), 0, 0, 0 };

		if (attach) {
			hr = intf->CreateProcessAndAttach2(server, const_cast<PSTR>(commandLine.c_str()),
				&options, sizeof(options), const_cast<PSTR>(initialDirectory.c_str()), env.c_str(),
				attachProcessId, static_cast<ULONG>(attachFlags));
		} else {
			hr = intf->CreateProcess2(server, const_cast<PSTR>(commandLine.c_str()),
				&options, sizeof(options), const_cast<PSTR>(initialDirectory.c_str()), env.c_str());
		}
	}
	Check(hr);
}
*/

// FIXME: these should all return HRESULT instead of throwing an exception
HRESULT CDebugClient::AttachProcess(ULONG id, CDebugClient::AttachFlags flags, ULONG64 server) const
{
	HRESULT hr;
	hr = m_intf->AttachProcess(server, id, static_cast<ULONG>(flags));
	Check(hr);
	return hr;
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
	intf->AbandonCurrentProcess();
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

ULONG64 CDebugClient::ConnectProcessServer(const std::string& remoteOptions) const
{
	ULONG64 serverId = 0;
	HRESULT hr = 0;

	hr = m_intf->ConnectProcessServer(remoteOptions.c_str(), &serverId);
	Check(hr);	// FIXME: these should all return the error code somehow
	/* XXX: Need to implement GetRunningProcessSystemIds */
	return serverId;
}

void CDebugClient::DisconnectProcessServer(ULONG64 Server) const
{
	Check(m_intf->DisconnectProcessServer(Server));
}

void CDebugClient::AttachKernel(CDebugClient::AttachKernelFlags flags, const std::string& connectOptions) const
{
	Check(m_intf->AttachKernel(static_cast<ULONG>(flags), connectOptions.empty() ? NULL : connectOptions.c_str()));
}

bool CDebugClient::IsKernelDebuggerEnabled(void) const
{
	HRESULT hr = CComQIPtr<IDebugClient2>(m_intf)->IsKernelDebuggerEnabled();
	Check(hr);	// FIXME: should always return error code
	return S_OK == hr;
}

const std::string CDebugClient::GetKernelConnectionOptions(void) const
{
	char options[1024]; ULONG size;

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

	Check(hr);	// FIXME: always return error code
	return hr == S_OK;
}
void CDebugClient::ExitDispatch(void) const
{
	m_intf->ExitDispatch(m_intf);
}

ULONG CDebugClient::GetNumberOfEventCallbacks(CDebugClient::CDebugEventCallbacks::InterestMask event) const
{
	CComQIPtr<IDebugClient5> intf(m_intf);
	ULONG res, count;

	res = static_cast<ULONG>(event);
	Check(intf->GetNumberEventCallbacks(res, &count));
	return count;
}
ULONG CDebugClient::GetNumberOfInputCallbacks(void) const
{
	CComQIPtr<IDebugClient5> intf(m_intf);
	ULONG count;

	Check(intf->GetNumberInputCallbacks(&count));
	return count;
}
ULONG CDebugClient::GetNumberOfOutputCallbacks(void) const
{
	CComQIPtr<IDebugClient5> intf(m_intf);
	ULONG count;

	Check(intf->GetNumberOutputCallbacks(&count));
	return count;
}

CDebugOutput::OutputMask CDebugClient::GetOutputMask(void) const
{
	ULONG mask;

	Check(m_intf->GetOutputMask(&mask));
	return static_cast<CDebugOutput::OutputMask>(mask);
}
void CDebugClient::SetOutputMask(CDebugOutput::OutputMask mask) const
{
	ULONG res = static_cast<ULONG>(mask);

	Check(m_intf->SetOutputMask(res));
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
	CComQIPtr<IDebugClient5> intf(m_intf);
	char szMsg[1024];
	ULONG nMsg = _countof(szMsg);

	Check(intf->GetQuitLockString(szMsg, nMsg, &nMsg));
	return std::string(szMsg, nMsg-1);
}

void CDebugClient::SetQuitLockString(const std::string str) const
{
	CComQIPtr<IDebugClient5> intf(m_intf);

	Check(intf->SetQuitLockString(str.c_str()));
}

void CDebugClient::OpenDumpFile(const std::string& filename) const
{
	Check(m_intf->OpenDumpFile(filename.c_str()));
}
void CDebugClient::AddDumpInformationFile(const std::string& filename) const
{
	CComQIPtr<IDebugClient2> intf(m_intf);

	// FIXME: DEBUG_DUMP_FILE_PAGE_FILE_DUMP should be an enumeration
	Check(intf->AddDumpInformationFile(filename.c_str(), DEBUG_DUMP_FILE_PAGE_FILE_DUMP));	// XXX
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
	for (ULONG i=0; i<number; i++) {
		Check(intf->GetDumpFile(i, szFile, _countof(szFile), &nFile, &handle, &nType));
		files[std::string(szFile, nFile-1)] = make_tuple(handle, nType);
	}
	return files;
}

void CDebugClient::WriteUserDumpFile(const std::string& filename, CDebugControl::UserDebuggee qualifier, CDebugClient::DumpFormat format, const std::string& comment) const
{
	CComQIPtr<IDebugClient2> intf(m_intf);

	if (DumpFormat::FORMAT_DEFAULT == format && comment.empty()) {
		Check(m_intf->WriteDumpFile(filename.c_str(), static_cast<ULONG>(qualifier)));
	} else {
		Check(intf->WriteDumpFile2(filename.c_str(), static_cast<ULONG>(qualifier), static_cast<ULONG>(format), comment.empty()? comment.c_str() : NULL));
	}
}

void CDebugClient::WriteKernelDumpFile(const std::string& filename, CDebugControl::KernelDebuggee qualifier, CDebugClient::DumpFormat format, const std::string& comment) const
{
	CComQIPtr<IDebugClient2> intf(m_intf);

	if (DumpFormat::FORMAT_DEFAULT == format && comment.empty()) {
		Check(m_intf->WriteDumpFile(filename.c_str(), static_cast<ULONG>(qualifier)));
	} else {
		Check(intf->WriteDumpFile2(filename.c_str(), static_cast<ULONG>(qualifier), static_cast<ULONG>(format), comment.empty()? comment.c_str() : NULL));
	}
}

const list CDebugClient::GetRunningProcessSystemIds(ULONG64 server) const
{
	list pids;
	ULONG count = 0;
	PULONG store(0);

	Check( m_intf->GetRunningProcessSystemIds(server, NULL, 0, &count) );

	try {
		store = new ULONG[count];
		Check( m_intf->GetRunningProcessSystemIds(server, store, count, &count) );

		for (ULONG i = 0; i < count; i++) {
			pids.append( store[i] );
		}

	} catch (std::exception&) {
		if (store)
			delete [] store;
		throw;
	}
	delete [] store;
	return pids;
}
tuple CDebugClient::GetRunningProcessDescription(ULONG systemId, ULONG flags, ULONG64 server) const
{
	HRESULT hr;
	ULONG exeNameSize, descriptionSize;
	ULONG actualExeNameSize, actualDescriptionSize;
	char buffer[1]; // docs say that the size must be != NULL?

	/* figure out the size */
	hr = m_intf->GetRunningProcessDescription(server, systemId, flags,
		buffer, sizeof(buffer), &actualExeNameSize,
		buffer, sizeof(buffer), &actualDescriptionSize
	);
	Check(hr);

	/* allocate */
	exeNameSize = actualExeNameSize;
	descriptionSize = actualDescriptionSize;

	boost::shared_ptr<char> exeName(new char[actualExeNameSize+1]);
	boost::shared_ptr<char> description(new char[actualDescriptionSize+1]);

	/* read it again */
	hr = m_intf->GetRunningProcessDescription(server, systemId, flags,
		exeName.get(), exeNameSize, &actualExeNameSize,
		description.get(), descriptionSize, &actualDescriptionSize
	);
	Check(hr);

	/* sanity */
	if (exeNameSize != actualExeNameSize)
		utils::warning("_PyDbgEng.DebugClient.GetRunningProcessDescription : returned exename size differs from previous");
	if (descriptionSize != actualDescriptionSize)
		utils::warning("_PyDbgEng.DebugClient.GetRunningProcessDescription : returned description size differs from previous");

	/* return tuple */
	boost::python::str first(std::string(exeName.get())), second(std::string(description.get()));
	return make_tuple(systemId, first, second);
}

// CDebugEventCallbacks
void CDebugClient::CDebugEventCallbacks::Get(IDebugEventCallbacks** callbacks)
{
	Check(m_intf->GetEventCallbacks(callbacks));
}

void CDebugClient::CDebugEventCallbacks::Set(IDebugEventCallbacks* callbacks)
{
	Check(m_intf->SetEventCallbacks(callbacks));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::GetInterestMask(__out PULONG Mask)
{
	ULONG res;

	res = 0;
	for(CallbackMap::const_iterator it = m_eventCallbacks.begin(); it != m_eventCallbacks.end(); it++)
		res |= it->first;
	*Mask = res;
	return S_OK;
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::Breakpoint(__in PDEBUG_BREAKPOINT Bp)
{
	HRESULT hr;
	hr = CallEventCallback(DEBUG_EVENT_BREAKPOINT, CBreakpointEventArgs(m_intf, Bp));
	return hr;
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::Exception(__in PEXCEPTION_RECORD64 Exception, __in ULONG FirstChance)
{
	HRESULT hr;
	hr = CallEventCallback(DEBUG_EVENT_EXCEPTION, CExceptionEventArgs(m_intf, Exception, FirstChance));
	return hr;
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::CreateThread(__in ULONG64 Handle, __in ULONG64 DataOffset, __in ULONG64 StartOffset)
{
	return CallEventCallback(DEBUG_EVENT_CREATE_THREAD, CCreateThreadEventArgs(m_intf, Handle, DataOffset, StartOffset));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ExitThread(__in ULONG ExitCode)
{
	return CallEventCallback(DEBUG_EVENT_EXIT_THREAD, CExitThreadEventArgs(m_intf, ExitCode));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::CreateProcess(__in ULONG64 ImageFileHandle, __in ULONG64 Handle,
																															 __in ULONG64 BaseOffset, __in ULONG ModuleSize,
																															 __in_opt PCSTR ModuleName, __in_opt PCSTR ImageName,
																															 __in ULONG CheckSum, __in ULONG TimeDateStamp,
																															 __in ULONG64 InitialThreadHandle, __in ULONG64 ThreadDataOffset,
																															 __in ULONG64 StartOffset)
{
	return CallEventCallback(DEBUG_EVENT_CREATE_PROCESS, CCreateProcessEventArgs(m_intf,
		ImageFileHandle, Handle, BaseOffset, ModuleSize, ModuleName, ImageName,
		CheckSum, TimeDateStamp, InitialThreadHandle, ThreadDataOffset, StartOffset));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ExitProcess(__in ULONG ExitCode)
{
	return CallEventCallback(DEBUG_EVENT_EXIT_PROCESS, CExitProcessEventArgs(m_intf, ExitCode));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::LoadModule(__in ULONG64 ImageFileHandle, __in ULONG64 BaseOffset,
																														__in ULONG ModuleSize, __in_opt PCSTR ModuleName,
																														__in_opt PCSTR ImageName, __in ULONG CheckSum,
																														__in ULONG TimeDateStamp)
{
	return CallEventCallback(DEBUG_EVENT_LOAD_MODULE, CLoadModuleEventArgs(m_intf,
		ImageFileHandle, BaseOffset, ModuleSize, ModuleName, ImageName, CheckSum, TimeDateStamp));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::UnloadModule(__in_opt PCSTR ImageBaseName, __in ULONG64 BaseOffset)
{
	return CallEventCallback(DEBUG_EVENT_UNLOAD_MODULE, CUnloadModuleEventArgs(m_intf, ImageBaseName, BaseOffset));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::SystemError(__in ULONG Error, __in ULONG Level)
{
	return CallEventCallback(DEBUG_EVENT_SYSTEM_ERROR, CSystemErrorEventArgs(m_intf, Error, Level));
}

STDMETHODIMP CDebugClient::CDebugEventCallbacks::SessionStatus(__in ULONG Status)
{
	return CallEventCallback(DEBUG_EVENT_SESSION_STATUS, CSessionStatusEventArgs(m_intf, Status));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeDebuggeeState(__in ULONG Flags, __in ULONG64 Argument)
{
	return CallEventCallback(DEBUG_EVENT_CHANGE_DEBUGGEE_STATE, CStateChangeEventArgs<CDebugClient::CDebugEventCallbacks::ChangeDebuggeeStateFlags>(m_intf, Flags, Argument));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeEngineState(__in ULONG Flags, __in ULONG64 Argument)
{
	return CallEventCallback(DEBUG_EVENT_CHANGE_ENGINE_STATE, CStateChangeEventArgs<CDebugClient::CDebugEventCallbacks::ChangeEngineStateFlags>(m_intf, Flags, Argument));
}
STDMETHODIMP CDebugClient::CDebugEventCallbacks::ChangeSymbolState(__in ULONG Flags, __in ULONG64 Argument)
{
	return CallEventCallback(DEBUG_EVENT_CHANGE_SYMBOL_STATE, CStateChangeEventArgs<CDebugClient::CDebugEventCallbacks::ChangeSymbolStateFlags>(m_intf, Flags, Argument));
}

/* CDebugClient::CEventArgs */
const CDebugClient&
CDebugClient::CEventArgs::GetOwner(void) const
{
	return CDebugClient(m_intf);
}

const CDebugControl*
CDebugClient::CEventArgs::GetController(void) const
{
	return new CDebugControl(m_intf);
}

/* CDebugClient::CDebugCallbacksBase */
IDebugClient*
CDebugClient::CDebugCallbacksBase::SetInterface(IDebugClient* intf)
{
	CDebugClient result(m_intf);
	m_intf = intf;
	return result;
}

/* CDebugClient::CDebugEventCallbacks */
object
CDebugClient::CDebugEventCallbacks::GetEvent(ULONG id) const
{
		CallbackMap::const_iterator it = m_eventCallbacks.find(id);
		return it == m_eventCallbacks.end()? object(borrowed(Py_None)) : it->second;
}

void
CDebugClient::CDebugEventCallbacks::SetEvent(ULONG id, object event)
{
	if (event.ptr() == Py_None)
		m_eventCallbacks.erase(id);
	else
		m_eventCallbacks.insert(std::make_pair(id, event));
	this->Set(this);
}

/* CDebugClient::CBreakpointEventArgs */
const CDebugBreakpoint&
CDebugClient::CBreakpointEventArgs::GetBreakpoint(void) const
{
	return CDebugBreakpoint(GetController(), m_bp);
}

/* CDebugClient::CExceptionEventArgs */
DWORD
CDebugClient::CExceptionEventArgs::GetExceptionCode(void) const
{
	return m_pRecord->ExceptionCode;
}

DWORD
CDebugClient::CExceptionEventArgs::GetExceptionFlags(void) const
{
	return m_pRecord->ExceptionFlags;
}

DWORD64
CDebugClient::CExceptionEventArgs::GetExceptionAddress(void) const
{
	return m_pRecord->ExceptionAddress;
}

const list
CDebugClient::CExceptionEventArgs::GetParameters(void) const
{
	list params;
	for (DWORD i=0; i<m_pRecord->NumberParameters; i++) {
		params.append(m_pRecord->ExceptionInformation[i]);
	}
	return params;
}

bool
CDebugClient::CExceptionEventArgs::IsFirstChance(void) const
{
	return m_bFirstChance;
}

/* CDebugClient::CCreateThreadEventArgs */
ULONG64 CDebugClient::CCreateThreadEventArgs::GetHandle(void) const
{
	return m_handle;
}
ULONG64 CDebugClient::CCreateThreadEventArgs::GetDataOffset(void) const
{
	return m_dataOffset;
}
ULONG64 CDebugClient::CCreateThreadEventArgs::GetStartOffset(void) const
{
	return m_startOffset;
}

/* CDebugClient::CExitThreadEventArgs */
ULONG CDebugClient::CExitThreadEventArgs::GetExitCode(void) const
{
	return m_exitCode;
}

/* CDebugClient::CCreateProcessEventArgs */
ULONG64
CDebugClient::CCreateProcessEventArgs::GetImageFileHandle(void) const
{
	return m_uImageFileHandle;
}

ULONG64
CDebugClient::CCreateProcessEventArgs::GetHandle(void) const
{
	return m_uHandle;
}

ULONG64
CDebugClient::CCreateProcessEventArgs::GetBaseOffset(void) const
{
	return m_uBaseOffset;
}

ULONG
CDebugClient::CCreateProcessEventArgs::GetModuleSize(void) const
{
	return m_uModuleSize;
}

PCSTR
CDebugClient::CCreateProcessEventArgs::GetModuleName(void) const
{
	return m_szModuleName;
}

PCSTR
CDebugClient::CCreateProcessEventArgs::GetImageName(void) const
{
	return m_szImageName;
}

ULONG
CDebugClient::CCreateProcessEventArgs::GetCheckSum(void) const
{
	return m_uCheckSum;
}

ULONG
CDebugClient::CCreateProcessEventArgs::GetTimeDateStamp(void) const
{
	return m_uTimeDateStamp;
}

ULONG64
CDebugClient::CCreateProcessEventArgs::GetInitialThreadHandle(void) const
{
	return m_uInitialThreadHandle;
}

ULONG64
CDebugClient::CCreateProcessEventArgs::GetThreadDataOffset(void) const
{
	return m_uThreadDataOffset;
}

ULONG64
CDebugClient::CCreateProcessEventArgs::GetStartOffset(void) const
{
	return m_uStartOffset;
}

/* CDebugClient::CExitProcessEventArgs */
ULONG
CDebugClient::CExitProcessEventArgs::GetExitCode(void) const
{
	return m_exitCode;
}

/* CDebugClient::CLoadModuleEventArgs */
ULONG64
CDebugClient::CLoadModuleEventArgs::GetImageFileHandle(void) const
{
	return m_uImageFileHandle;
}

ULONG64
CDebugClient::CLoadModuleEventArgs::GetBaseOffset(void) const
{
	return m_uBaseOffset;
}

ULONG
CDebugClient::CLoadModuleEventArgs::GetModuleSize(void) const
{
	return m_uModuleSize;
}

PCSTR
CDebugClient::CLoadModuleEventArgs::GetModuleName(void) const
{
	return m_szModuleName;
}

PCSTR
CDebugClient::CLoadModuleEventArgs::GetImageName(void) const
{
	return m_szImageName;
}

ULONG
CDebugClient::CLoadModuleEventArgs::GetCheckSum(void) const
{
	return m_uCheckSum;
}

ULONG
CDebugClient::CLoadModuleEventArgs::GetTimeDateStamp(void) const
{
	return m_uTimeDateStamp;
}

/* CDebugClient::CUnloadModuleEventArgs */
PCSTR
CDebugClient::CUnloadModuleEventArgs::GetImageBaseName(void) const
{
	return m_szImageBaseName;
}

ULONG64
CDebugClient::CUnloadModuleEventArgs::GetBaseOffset(void) const
{
	return m_uBaseOffset;
}

/* CDebugClient::CSystemErrorEventArgs */
ULONG
CDebugClient::CSystemErrorEventArgs::GetError(void) const
{
	return m_uError;
}

ULONG CDebugClient::CSystemErrorEventArgs::GetLevel(void) const
{
	return m_uLevel;
}

/* CDebugClient::CSessionStatusEventArgs */
CDebugClient::CDebugEventCallbacks::SessionStatusFlags
CDebugClient::CSessionStatusEventArgs::GetStatus(void) const
{
	return (CDebugClient::CDebugEventCallbacks::SessionStatusFlags) m_uStatus;
}

/* CDebugClient::CDebugInputCallbacks */
void CDebugClient::CDebugInputCallbacks::Get(IDebugInputCallbacks** callbacks)
{
	Check(m_intf->GetInputCallbacks(callbacks));
}

void CDebugClient::CDebugInputCallbacks::Set(IDebugInputCallbacks* callbacks)
{
	Check(m_intf->SetInputCallbacks(callbacks));
}

const object
CDebugClient::CDebugInputCallbacks::GetInput(void) const
{
	return m_input;
}

void
CDebugClient::CDebugInputCallbacks::SetInput(const object& obj)
{
	m_input = obj;
}

void
CDebugClient::CDebugInputCallbacks::Export(void)
{
	class_<CDebugClient::CInputEventArgs, bases<CDebugClient::CEventArgs>>("InputEventArgs", no_init)
		.add_property("BufferSize", &CDebugClient::CInputEventArgs::GetBufferSize)
		;
}

/* CDebugClient::CInputEventArgs */
ULONG CDebugClient::CInputEventArgs::GetBufferSize(void) const
{
	return m_uBufferSize;
}

/* CDebugClient::CDebugOutputCallbacks */
void
CDebugClient::CDebugOutputCallbacks::Get(IDebugOutputCallbacks** callbacks)
{
	Check(m_intf->GetOutputCallbacks(callbacks));
}

void
CDebugClient::CDebugOutputCallbacks::Set(IDebugOutputCallbacks* callbacks)
{
	Check(m_intf->SetOutputCallbacks(callbacks));
}

const object
CDebugClient::CDebugOutputCallbacks::GetOutput(void) const
{
	return m_output;
}

void
CDebugClient::CDebugOutputCallbacks::SetOutput(const object& obj)
{
	m_output = obj;
}

void
CDebugClient::CDebugOutputCallbacks::Export(void)
{
	enum_<CDebugClient::CDebugOutputCallbacks::InterestMask>("InterestMask")
		.value("EXPLICIT_FLUSH",  InterestMask::OUTCBI_EXPLICIT_FLUSH) // Indicates that the callback wants notifications of all explicit flushes.
		.value("TEXT",            InterestMask::OUTCBI_TEXT)           // Indicates that the callback wants content in text form.
		.value("DML",             InterestMask::OUTCBI_DML)            // Indicates that the callback wants content in markup form.
		.value("ANY",             InterestMask::OUTCBI_ANY_FORMAT)
	;
	enum_<CDebugClient::CDebugOutputCallbacks::CallbackTypes>("CallbackTypes")
		.value("TEXT",  CallbackTypes::OUTCB_TEXT)           // Plain text content, flags are below, argument is mask.
		.value("DML",   CallbackTypes::OUTCB_DML)            // Debugger markup content, flags are below, argument is mask.
		.value("FLUSH", CallbackTypes::OUTCB_EXPLICIT_FLUSH) // Notification of an explicit output flush, flags and argument are zero.
	;

	enum_<CDebugClient::CDebugOutputCallbacks::CallbackFlags>("CallbackFlags")
		.value("FLUSH",                  CallbackFlags::OUTCBF_COMBINED_EXPLICIT_FLUSH)    // The content string was followed by an explicit flush.  This flag will be used instead of a separate DEBUG_OUTCB_EXPLICIT_FLUSH callback when a flush has text to flush, thus avoiding two callbacks.
		.value("HAS_TAGS",               CallbackFlags::OUTCBF_DML_HAS_TAGS)               // The markup content string has embedded tags.
		.value("HAS_SPECIAL_CHARACTERS", CallbackFlags::OUTCBF_DML_HAS_SPECIAL_CHARACTERS) // The markup content has encoded special characters like ", &, < and >.
	;

	class_<CDebugClient::COutputEventArgs, bases<CDebugClient::CEventArgs>>("OutputEventArgs", no_init)
		.add_property("Mask", &CDebugClient::COutputEventArgs::GetMask)
		.add_property("Text", &CDebugClient::COutputEventArgs::GetText)
	;
}

/* CDebugClient::COutputEventArgs */
CDebugOutput::OutputMask
CDebugClient::COutputEventArgs::GetMask(void) const
{
	return static_cast<CDebugOutput::OutputMask>(m_mask);
}

const std::string
CDebugClient::COutputEventArgs::GetText(void) const
{
	return m_text;
}
