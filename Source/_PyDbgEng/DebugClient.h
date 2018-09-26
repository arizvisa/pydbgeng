#ifndef __DebugClient_h
#define __DebugClient_h
#pragma once

#include <string>
#include <map>

#include <atlbase.h>
#include <atlcom.h>

class CDebugBreakpoint;
class CDebugOutput;
class CDebugRegisters;
class CDebugSymbols;
class CDebugDataSpaces;
class CDebugSystemObjects;
class CDebugAdvanced;

class CDebugControl;

class CDebugClient : public CDebugObject<IDebugClient>
{
public:
	static const ULONG DEFAULT_PROCESS_SERVER = 0;

/** accessors */
	enum class AttachKernelFlags : ULONG {
		ATTACH_KERNEL_CONNECTION  = DEBUG_ATTACH_KERNEL_CONNECTION,
		ATTACH_LOCAL_KERNEL       = DEBUG_ATTACH_LOCAL_KERNEL,
		ATTACH_EXDI_DRIVER        = DEBUG_ATTACH_EXDI_DRIVER,
		ATTACH_INSTALL_DRIVER     = DEBUG_ATTACH_INSTALL_DRIVER,
	};

	enum class RunningProcessSystemIdFlags : ULONG {
		GET_PROC_DEFAULT = DEBUG_GET_PROC_DEFAULT,
		GET_PROC_FULL_MATCH = DEBUG_GET_PROC_FULL_MATCH,
		GET_PROC_ONLY_MATCH = DEBUG_GET_PROC_ONLY_MATCH,
		GET_PROC_SERVICE_NAME = DEBUG_GET_PROC_SERVICE_NAME,
	};

	enum class RunningProcessDescriptionFlags : ULONG {
		PROC_DESC_DEFAULT = DEBUG_PROC_DESC_DEFAULT,
		PROC_DESC_NO_PATHS = DEBUG_PROC_DESC_NO_PATHS,
		PROC_DESC_NO_SERVICES = DEBUG_PROC_DESC_NO_SERVICES,
		PROC_DESC_NO_MTS_PACKAGES = DEBUG_PROC_DESC_NO_MTS_PACKAGES,
		PROC_DESC_NO_COMMAND_LINE = DEBUG_PROC_DESC_NO_COMMAND_LINE,
		PROC_DESC_NO_SESSION_ID = DEBUG_PROC_DESC_NO_SESSION_ID,
		PROC_DESC_NO_USER_NAME = DEBUG_PROC_DESC_NO_USER_NAME,
	};

	enum class AttachFlags : ULONG {
		ATTACH_DEFAULT = DEBUG_ATTACH_DEFAULT,
		ATTACH_NONINVASIVE = DEBUG_ATTACH_NONINVASIVE,
		ATTACH_EXISTING = DEBUG_ATTACH_EXISTING,
		ATTACH_NONINVASIVE_NO_SUSPEND = DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND,
		ATTACH_INVASIVE_NO_INITIAL_BREAK = DEBUG_ATTACH_INVASIVE_NO_INITIAL_BREAK,
		ATTACH_INVASIVE_RESUME_PROCESS = DEBUG_ATTACH_INVASIVE_RESUME_PROCESS,
		ATTACH_NONINVASIVE_ALLOW_PARTIAL = DEBUG_ATTACH_NONINVASIVE_ALLOW_PARTIAL
	};

	/** Process creation (not mapped 1:1) */
	enum class CreateFlags : ULONG {
		CREATE_FLAGS_DEFAULT                       = 0,
		CREATE_FLAGS_DEBUG_PROCESS                 = DEBUG_PROCESS,
		CREATE_FLAGS_DEBUG_ONLY_THIS_PROCESS       = DEBUG_ONLY_THIS_PROCESS,
		CREATE_FLAGS_SUSPENDED                     = CREATE_SUSPENDED,
		CREATE_FLAGS_DETACHED_PROCESS              = DETACHED_PROCESS,
		CREATE_FLAGS_NEW_CONSOLE                   = CREATE_NEW_CONSOLE,
		CREATE_FLAGS_NORMAL_PRIORITY_CLASS         = NORMAL_PRIORITY_CLASS,
		CREATE_FLAGS_IDLE_PRIORITY_CLASS           = IDLE_PRIORITY_CLASS,
		CREATE_FLAGS_HIGH_PRIORITY_CLASS           = HIGH_PRIORITY_CLASS,
		CREATE_FLAGS_REALTIME_PRIORITY_CLASS       = REALTIME_PRIORITY_CLASS,
		CREATE_FLAGS_NEW_PROCESS_GROUP             = CREATE_NEW_PROCESS_GROUP,
		CREATE_FLAGS_UNICODE_ENVIRONMENT           = CREATE_UNICODE_ENVIRONMENT,
		CREATE_FLAGS_SEPARATE_WOW_VDM              = CREATE_SEPARATE_WOW_VDM,
		CREATE_FLAGS_SHARED_WOW_VDM                = CREATE_SHARED_WOW_VDM,
		CREATE_FLAGS_FORCEDOS                      = CREATE_FORCEDOS,
		CREATE_FLAGS_BELOW_NORMAL_PRIORITY_CLASS   = BELOW_NORMAL_PRIORITY_CLASS,
		CREATE_FLAGS_ABOVE_NORMAL_PRIORITY_CLASS   = ABOVE_NORMAL_PRIORITY_CLASS,
		CREATE_FLAGS_INHERIT_PARENT_AFFINITY       = INHERIT_PARENT_AFFINITY,
		CREATE_FLAGS_INHERIT_CALLER_PRIORITY       = INHERIT_CALLER_PRIORITY,            //Deprecated
		CREATE_FLAGS_PROTECTED_PROCESS             = CREATE_PROTECTED_PROCESS,
		CREATE_FLAGS_EXTENDED_STARTUPINFO_PRESENT  = EXTENDED_STARTUPINFO_PRESENT,
		CREATE_FLAGS_PROCESS_MODE_BACKGROUND_BEGIN = PROCESS_MODE_BACKGROUND_BEGIN,
		CREATE_FLAGS_PROCESS_MODE_BACKGROUND_END   = PROCESS_MODE_BACKGROUND_END,
		CREATE_FLAGS_BREAKAWAY_FROM_JOB            = CREATE_BREAKAWAY_FROM_JOB,
		CREATE_FLAGS_PRESERVE_CODE_AUTHZ_LEVEL     = CREATE_PRESERVE_CODE_AUTHZ_LEVEL,
		CREATE_FLAGS_DEFAULT_ERROR_MODE            = CREATE_DEFAULT_ERROR_MODE,
		CREATE_FLAGS_NO_WINDOW                     = CREATE_NO_WINDOW,
		CREATE_FLAGS_PROFILE_USER                  = PROFILE_USER,
		CREATE_FLAGS_PROFILE_KERNEL                = PROFILE_KERNEL,
		CREATE_FLAGS_PROFILE_SERVER                = PROFILE_SERVER,
		CREATE_FLAGS_IGNORE_SYSTEM_DEFAULT         = CREATE_IGNORE_SYSTEM_DEFAULT,
		CREATE_FLAGS_PROCESS_NO_DEBUG_HEAP         = DEBUG_CREATE_PROCESS_NO_DEBUG_HEAP, // On Windows XP this flag prevents the debug heap from being used in the new process.
		CREATE_FLAGS_PROCESS_THROUGH_RTL           = DEBUG_CREATE_PROCESS_THROUGH_RTL,   // Indicates that the native NT RTL process creation routines should be used instead of Win32.	This is only meaningful for special processes that run as NT native processes.
	};

	enum class DebugCreateFlags : ULONG {
		DEBUG_CREATE_DEFAULT                   = DEBUG_ECREATE_PROCESS_DEFAULT,
		DEBUG_CREATE_INHERIT_HANDLES           = DEBUG_ECREATE_PROCESS_INHERIT_HANDLES,
		DEBUG_CREATE_USE_VERIFIER_FLAGS        = DEBUG_ECREATE_PROCESS_USE_VERIFIER_FLAGS,
		DEBUG_CREATE_USE_IMPLICIT_COMMAND_LINE = DEBUG_ECREATE_PROCESS_USE_IMPLICIT_COMMAND_LINE,
	};
	/** End Process Creation */

	enum class ProcessOptions : ULONG {
		PROCESS_DETACH_ON_EXIT    = DEBUG_PROCESS_DETACH_ON_EXIT,
		PROCESS_ONLY_THIS_PROCESS = DEBUG_PROCESS_ONLY_THIS_PROCESS,
	};

	enum class ConnectSessionFlags : ULONG {
		CONNECT_SESSION_DEFAULT     = DEBUG_CONNECT_SESSION_DEFAULT,
		CONNECT_SESSION_NO_VERSION  = DEBUG_CONNECT_SESSION_NO_VERSION,
		CONNECT_SESSION_NO_ANNOUNCE = DEBUG_CONNECT_SESSION_NO_ANNOUNCE,
	};

	enum class OutputServersFlags : ULONG {
		OUTPUT_SERVERS_DEBUGGER = DEBUG_SERVERS_DEBUGGER,
		OUTPUT_SERVERS_PROCESS  = DEBUG_SERVERS_PROCESS,
		OUTPUT_SERVERS_ALL      = DEBUG_SERVERS_ALL,
	};

	enum class EndSessionFlags : ULONG {
		END_PASSIVE = DEBUG_END_PASSIVE,
		END_ACTIVE_TERMINATE = DEBUG_END_ACTIVE_TERMINATE,
		END_ACTIVE_DETACH = DEBUG_END_ACTIVE_DETACH,
		END_REENTRANT = DEBUG_END_REENTRANT,
		END_DISCONNECT = DEBUG_END_DISCONNECT,
	};

	enum class ClientIdentifier : ULONG {
		CLIENT_UNKNOWN = DEBUG_CLIENT_UNKNOWN,
		CLIENT_VSINT = DEBUG_CLIENT_VSINT,
		CLIENT_NTSD = DEBUG_CLIENT_NTSD,
		CLIENT_NTKD = DEBUG_CLIENT_NTKD,
		CLIENT_CDB = DEBUG_CLIENT_CDB,
		CLIENT_KD = DEBUG_CLIENT_KD,
		CLIENT_WINDBG = DEBUG_CLIENT_WINDBG,
		CLIENT_WINIDE = DEBUG_CLIENT_WINIDE,
	};

	enum class DumpFormat : ULONG {
		FORMAT_DEFAULT                              = DEBUG_FORMAT_DEFAULT,
		FORMAT_CAB_SECONDARY_ALL_IMAGES             = DEBUG_FORMAT_CAB_SECONDARY_ALL_IMAGES,
		FORMAT_WRITE_CAB                            = DEBUG_FORMAT_WRITE_CAB,
		FORMAT_CAB_SECONDARY_FILES                  = DEBUG_FORMAT_CAB_SECONDARY_FILES,
		FORMAT_NO_OVERWRITE                         = DEBUG_FORMAT_NO_OVERWRITE,
		FORMAT_USER_SMALL_FULL_MEMORY               = DEBUG_FORMAT_USER_SMALL_FULL_MEMORY,
		FORMAT_USER_SMALL_HANDLE_DATA               = DEBUG_FORMAT_USER_SMALL_HANDLE_DATA,
		FORMAT_USER_SMALL_UNLOADED_MODULES          = DEBUG_FORMAT_USER_SMALL_UNLOADED_MODULES,
		FORMAT_USER_SMALL_INDIRECT_MEMORY           = DEBUG_FORMAT_USER_SMALL_INDIRECT_MEMORY,
		FORMAT_USER_SMALL_DATA_SEGMENTS             = DEBUG_FORMAT_USER_SMALL_DATA_SEGMENTS,
		FORMAT_USER_SMALL_FILTER_MEMORY             = DEBUG_FORMAT_USER_SMALL_FILTER_MEMORY,
		FORMAT_USER_SMALL_FILTER_PATHS              = DEBUG_FORMAT_USER_SMALL_FILTER_PATHS,
		FORMAT_USER_SMALL_PROCESS_THREAD_DATA       = DEBUG_FORMAT_USER_SMALL_PROCESS_THREAD_DATA,
		FORMAT_USER_SMALL_PRIVATE_READ_WRITE_MEMORY = DEBUG_FORMAT_USER_SMALL_PRIVATE_READ_WRITE_MEMORY,
		FORMAT_USER_SMALL_NO_OPTIONAL_DATA          = DEBUG_FORMAT_USER_SMALL_NO_OPTIONAL_DATA,
		FORMAT_USER_SMALL_FULL_MEMORY_INFO          = DEBUG_FORMAT_USER_SMALL_FULL_MEMORY_INFO,
		FORMAT_USER_SMALL_THREAD_INFO               = DEBUG_FORMAT_USER_SMALL_THREAD_INFO,
		FORMAT_USER_SMALL_CODE_SEGMENTS             = DEBUG_FORMAT_USER_SMALL_CODE_SEGMENTS,
		FORMAT_USER_SMALL_NO_AUXILIARY_STATE        = DEBUG_FORMAT_USER_SMALL_NO_AUXILIARY_STATE,
		FORMAT_USER_SMALL_FULL_AUXILIARY_STATE      = DEBUG_FORMAT_USER_SMALL_FULL_AUXILIARY_STATE,
		FORMAT_USER_SMALL_MODULE_HEADERS            = DEBUG_FORMAT_USER_SMALL_MODULE_HEADERS,
		FORMAT_USER_SMALL_FILTER_TRIAGE             = DEBUG_FORMAT_USER_SMALL_FILTER_TRIAGE,
		FORMAT_USER_SMALL_IGNORE_INACCESSIBLE_MEM   = DEBUG_FORMAT_USER_SMALL_IGNORE_INACCESSIBLE_MEM,
	};

	///
	enum class PyCreateFlags : ULONG {
		CREATE_DEFAULT                  = 0,
		CREATE_ATTACH_PROCESS           = DEBUG_PROCESS,
		CREATE_ATTACH_ONLY_THIS_PROCESS = DEBUG_ONLY_THIS_PROCESS,
		CREATE_SUSPENDED_               = CREATE_SUSPENDED,
		CREATE_NO_DEBUG_HEAP            = DEBUG_CREATE_PROCESS_NO_DEBUG_HEAP,
		CREATE_THROUGH_RTL              = DEBUG_CREATE_PROCESS_THROUGH_RTL
	};

	class CEventArgs {
	private:
		IDebugClient *m_intf;
	protected:
		CEventArgs(IDebugClient *client) :
			m_intf(client) {}
	public:
		const CDebugClient& GetOwner(void) const;
		const CDebugControl* GetController(void) const;
	};

/** callback base class */
	class CDebugCallbacksBase : public CComObjectRootEx<CComMultiThreadModel>
	{
	protected:
		IDebugClient* m_intf;

		CDebugCallbacksBase()
		{
			HRESULT hr;
			hr = ::DebugCreate(__uuidof(IDebugClient), (PVOID*) &m_intf);
			if (hr != S_OK) {
				::PyErr_SetFromWindowsErr(hr);
				throw_error_already_set();
				assert(false == true);
			}
		}

	public:
		/** Assigns an interface to a debug client. This is used to access callbacks */
		IDebugClient* SetInterface(IDebugClient* intf);
	};

	template <typename I>
	class CDebugCallbacks : public CDebugCallbacksBase, public I
	{
	protected:
		CComQIPtr<I> m_callbacks;

	public:
		virtual void Get(I**) { assert(false); }
		virtual void Set(I*) { assert(false); }

		IDebugClient* SetInterface(IDebugClient* intf) {
			CDebugClient result( CDebugCallbacksBase::SetInterface(intf) );
			Get(&m_callbacks);
			return m_intf;
		}
	};

/** Debugging Event callbacks */
	class CDebugEventCallbacks : public CDebugCallbacks<IDebugEventCallbacks>
	{
	public:
		void Set(IDebugEventCallbacks* callbacks);
		void Get(IDebugEventCallbacks** callbacks);
		static void Export(void);

		enum class InterestMask : ULONG {
			EVENT_BREAKPOINT            = DEBUG_EVENT_BREAKPOINT,
			EVENT_EXCEPTION             = DEBUG_EVENT_EXCEPTION,
			EVENT_CREATE_THREAD         = DEBUG_EVENT_CREATE_THREAD,
			EVENT_EXIT_THREAD           = DEBUG_EVENT_EXIT_THREAD,
			EVENT_CREATE_PROCESS        = DEBUG_EVENT_CREATE_PROCESS,
			EVENT_EXIT_PROCESS          = DEBUG_EVENT_EXIT_PROCESS,
			EVENT_LOAD_MODULE           = DEBUG_EVENT_LOAD_MODULE,
			EVENT_UNLOAD_MODULE         = DEBUG_EVENT_UNLOAD_MODULE,
			EVENT_SYSTEM_ERROR          = DEBUG_EVENT_SYSTEM_ERROR,
			EVENT_SESSION_STATUS        = DEBUG_EVENT_SESSION_STATUS,
			EVENT_CHANGE_DEBUGGEE_STATE = DEBUG_EVENT_CHANGE_DEBUGGEE_STATE,
			EVENT_CHANGE_ENGINE_STATE   = DEBUG_EVENT_CHANGE_ENGINE_STATE,
			EVENT_CHANGE_SYMBOL_STATE   = DEBUG_EVENT_CHANGE_SYMBOL_STATE
		};

		enum class SessionStatusFlags : ULONG {
			SESSION_ACTIVE                       = DEBUG_SESSION_ACTIVE,
			SESSION_END_SESSION_ACTIVE_TERMINATE = DEBUG_SESSION_END_SESSION_ACTIVE_TERMINATE,
			SESSION_END_SESSION_ACTIVE_DETACH    = DEBUG_SESSION_END_SESSION_ACTIVE_DETACH,
			SESSION_END_SESSION_PASSIVE          = DEBUG_SESSION_END_SESSION_PASSIVE,
			SESSION_END                          = DEBUG_SESSION_END,
			SESSION_REBOOT                       = DEBUG_SESSION_REBOOT,
			SESSION_HIBERNATE                    = DEBUG_SESSION_HIBERNATE,
			SESSION_FAILURE                      = DEBUG_SESSION_FAILURE
		};

		enum class ChangeDebuggeeStateFlags : ULONG {
			CDS_ALL = DEBUG_CDS_ALL,
			CDS_REGISTERS = DEBUG_CDS_REGISTERS,
			CDS_DATA = DEBUG_CDS_DATA,
			CDS_REFRESH = DEBUG_CDS_REFRESH,
		};

		enum class ChangeDebuggeeStateRefreshIds : ULONG {
			REFRESH_EVALUATE = DEBUG_CDS_REFRESH_EVALUATE,
			REFRESH_EXECUTE = DEBUG_CDS_REFRESH_EXECUTE,
			REFRESH_EXECUTECOMMANDFILE = DEBUG_CDS_REFRESH_EXECUTECOMMANDFILE,
			REFRESH_ADDBREAKPOINT = DEBUG_CDS_REFRESH_ADDBREAKPOINT,
			REFRESH_REMOVEBREAKPOINT = DEBUG_CDS_REFRESH_REMOVEBREAKPOINT,
			REFRESH_WRITEVIRTUAL = DEBUG_CDS_REFRESH_WRITEVIRTUAL,
			REFRESH_WRITEVIRTUALUNCACHED = DEBUG_CDS_REFRESH_WRITEVIRTUALUNCACHED,
			REFRESH_WRITEPHYSICAL = DEBUG_CDS_REFRESH_WRITEPHYSICAL,
			REFRESH_WRITEPHYSICAL2 = DEBUG_CDS_REFRESH_WRITEPHYSICAL2,
			REFRESH_SETVALUE = DEBUG_CDS_REFRESH_SETVALUE,
			REFRESH_SETVALUE2 = DEBUG_CDS_REFRESH_SETVALUE2,
			REFRESH_SETSCOPE = DEBUG_CDS_REFRESH_SETSCOPE,
			REFRESH_SETSCOPEFRAMEBYINDEX = DEBUG_CDS_REFRESH_SETSCOPEFRAMEBYINDEX,
			REFRESH_SETSCOPEFROMJITDEBUGINFO = DEBUG_CDS_REFRESH_SETSCOPEFROMJITDEBUGINFO,
			REFRESH_SETSCOPEFROMSTOREDEVENT = DEBUG_CDS_REFRESH_SETSCOPEFROMSTOREDEVENT,
			REFRESH_INLINESTEP = DEBUG_CDS_REFRESH_INLINESTEP,
			REFRESH_INLINESTEP_PSEUDO = DEBUG_CDS_REFRESH_INLINESTEP_PSEUDO,
		};

		enum class ChangeEngineStateFlags : ULONG {
			CES_ALL = DEBUG_CES_ALL,
			CES_CURENT_THREAD = DEBUG_CES_CURRENT_THREAD,
			CES_EFFECTIVE_PROCESSOR = DEBUG_CES_EFFECTIVE_PROCESSOR,
			CES_BREAKPOINTS = DEBUG_CES_BREAKPOINTS,
			CES_CODE_LEVEL = DEBUG_CES_CODE_LEVEL,
			CES_EXECUTION_STATUS = DEBUG_CES_EXECUTION_STATUS,
			CES_ENGINE_OPTIONS = DEBUG_CES_ENGINE_OPTIONS,
			CES_LOG_FILE = DEBUG_CES_LOG_FILE,
			CES_RADIX = DEBUG_CES_RADIX,
			CES_EVENT_FILTERS = DEBUG_CES_EVENT_FILTERS,
			CES_PROCESS_OPTIONS = DEBUG_CES_PROCESS_OPTIONS,
			CES_EXTENSIONS = DEBUG_CES_EXTENSIONS,
			CES_SYSTEMS = DEBUG_CES_SYSTEMS,
			CES_ASSEMBLY_OPTIONS = DEBUG_CES_ASSEMBLY_OPTIONS,
			CES_EXPRESSION_SYNTAX = DEBUG_CES_EXPRESSION_SYNTAX,
			CES_TEXT_REPLACEMENTS = DEBUG_CES_TEXT_REPLACEMENTS,
		};

		enum class ChangeSymbolStateFlags : ULONG {
			CSS_ALL = DEBUG_CSS_ALL,
			CSS_LOADS = DEBUG_CSS_LOADS,
			CSS_UNLOADS = DEBUG_CSS_UNLOADS,
			CSS_SCOPE = DEBUG_CSS_SCOPE,
			CSS_PATHS = DEBUG_CSS_PATHS,
			CSS_SYMBOL_OPTIONS = DEBUG_CSS_SYMBOL_OPTIONS,
			CSS_TYPE_OPTIONS = DEBUG_CSS_TYPE_OPTIONS,
		};

	protected:
		object GetEvent(ULONG id) const;
		void SetEvent(ULONG id, object event);

	private:
		typedef std::map<ULONG, object> CallbackMap;
		CallbackMap m_eventCallbacks;

		template <typename T>
		HRESULT CallEventCallback(ULONG id, const T& args)
		{
			CallbackMap::const_iterator it = m_eventCallbacks.find(id);

			if (it == m_eventCallbacks.end()) {
				return (HRESULT) CDebugControl::ExecutionStatus::STATUS_IGNORE_EVENT;  // no events found
			}

			object result;
			try {
				result = it->second(args);
			} catch (...) {
				str message;
				message = str("_PyDbgEng.DebugClient.CallEventCallback(");
				message += str(id);
				message += str(") : ");
				message += utils::repr(it->second);
				message += str(" : Exception raised by python while handling event");
				utils::fatal(message);

				handle_exception();
				return (HRESULT) CDebugControl::ExecutionStatus::STATUS_BREAK;
			}
			if (result.is_none())
				return (HRESULT) CDebugControl::ExecutionStatus::STATUS_NO_CHANGE;
			return extract<long>(result);
		}

	public:
		BEGIN_COM_MAP(CDebugEventCallbacks)
			COM_INTERFACE_ENTRY(IDebugEventCallbacks)
		END_COM_MAP()

		#define DECLARE_EVENT(name, id) \
			object Get##name(void) const { return GetEvent(id); } \
			void Set##name(object event) { SetEvent(id, event); };

		DECLARE_EVENT(Breakpoint, DEBUG_EVENT_BREAKPOINT);
		DECLARE_EVENT(Exception, DEBUG_EVENT_EXCEPTION);
		DECLARE_EVENT(CreateThread, DEBUG_EVENT_CREATE_THREAD);
		DECLARE_EVENT(ExitThread, DEBUG_EVENT_EXIT_THREAD);
		DECLARE_EVENT(CreateProcess, DEBUG_EVENT_CREATE_PROCESS);
		DECLARE_EVENT(ExitProcess, DEBUG_EVENT_EXIT_PROCESS);
		DECLARE_EVENT(LoadModule, DEBUG_EVENT_LOAD_MODULE);
		DECLARE_EVENT(UnloadModule, DEBUG_EVENT_UNLOAD_MODULE);
		DECLARE_EVENT(SystemError, DEBUG_EVENT_SYSTEM_ERROR);
		DECLARE_EVENT(SessionStatus, DEBUG_EVENT_SESSION_STATUS);
		DECLARE_EVENT(ChangeDebuggeeState, DEBUG_EVENT_CHANGE_DEBUGGEE_STATE);
		DECLARE_EVENT(ChangeEngineState, DEBUG_EVENT_CHANGE_ENGINE_STATE);
		DECLARE_EVENT(ChangeSymbolState, DEBUG_EVENT_CHANGE_SYMBOL_STATE);

		#undef DECLARE_EVENT

		// IDebugEventCallbacks
		STDMETHOD(GetInterestMask)(__out PULONG Mask);
		STDMETHOD(Breakpoint)(__in PDEBUG_BREAKPOINT Bp);
		STDMETHOD(Exception)(__in PEXCEPTION_RECORD64 Exception, __in ULONG FirstChance);
		STDMETHOD(CreateThread)(__in ULONG64 Handle, __in ULONG64 DataOffset, __in ULONG64 StartOffset);
		STDMETHOD(ExitThread)(__in ULONG ExitCode);
		STDMETHOD(CreateProcess)(__in ULONG64 ImageFileHandle, __in ULONG64 Handle, __in ULONG64 BaseOffset, __in ULONG ModuleSize, __in_opt PCSTR ModuleName, __in_opt PCSTR ImageName, __in ULONG CheckSum, __in ULONG TimeDateStamp, __in ULONG64 InitialThreadHandle, __in ULONG64 ThreadDataOffset, __in ULONG64 StartOffset);
		STDMETHOD(ExitProcess)(__in ULONG ExitCode);
		STDMETHOD(LoadModule)(__in ULONG64 ImageFileHandle, __in ULONG64 BaseOffset, __in ULONG ModuleSize, __in_opt PCSTR ModuleName, __in_opt PCSTR ImageName, __in ULONG CheckSum, __in ULONG TimeDateStamp);
		STDMETHOD(UnloadModule)(__in_opt PCSTR ImageBaseName, __in ULONG64 BaseOffset);
		STDMETHOD(SystemError)(__in ULONG Error, __in ULONG Level);
		STDMETHOD(SessionStatus)(__in ULONG Status);
		STDMETHOD(ChangeDebuggeeState)(__in ULONG Flags, __in ULONG64 Argument);
		STDMETHOD(ChangeEngineState)(__in ULONG Flags, __in ULONG64 Argument);
		STDMETHOD(ChangeSymbolState)(__in ULONG Flags, __in ULONG64 Argument);
	};

	class CBreakpointEventArgs : public CEventArgs {
	private:
		PDEBUG_BREAKPOINT m_bp;
	public:
		CBreakpointEventArgs(IDebugClient *client, PDEBUG_BREAKPOINT bp)
			: CEventArgs(client), m_bp(bp)
		{}
		const CDebugBreakpoint& GetBreakpoint(void) const;
	};

	class CExceptionEventArgs : public CEventArgs {
	private:
		PEXCEPTION_RECORD64 m_pRecord;
		ULONG m_bFirstChance;
	public:
		CExceptionEventArgs(IDebugClient *client, PEXCEPTION_RECORD64 pException, ULONG bFirstChance)
			: CEventArgs(client), m_pRecord(pException), m_bFirstChance(bFirstChance)
		{}

		DWORD GetExceptionCode(void) const;
		DWORD GetExceptionFlags(void) const;
		DWORD64 GetExceptionAddress(void) const;
		const list GetParameters(void) const;
		bool IsFirstChance(void) const;
	};

	class CCreateThreadEventArgs : public CEventArgs {
	private:
		ULONG64 m_handle, m_dataOffset, m_startOffset;
	public:
		CCreateThreadEventArgs(IDebugClient *client, ULONG64 Handle, ULONG64 DataOffset, ULONG64 StartOffset)
			: CEventArgs(client), m_handle(Handle), m_dataOffset(DataOffset), m_startOffset(StartOffset)
		{}

		ULONG64 GetHandle(void) const;
		ULONG64 GetDataOffset(void) const;
		ULONG64 GetStartOffset(void) const;
	};

	class CExitThreadEventArgs : public CEventArgs {
	private:
		ULONG m_exitCode;
	public:
		CExitThreadEventArgs(IDebugClient *client, ULONG ExitCode)
			: CEventArgs(client), m_exitCode(ExitCode)
		{}
		ULONG GetExitCode(void) const;
	};

	class CCreateProcessEventArgs : public CEventArgs
	{
	private:
		ULONG64 m_uImageFileHandle, m_uHandle, m_uBaseOffset,
		m_uInitialThreadHandle, m_uThreadDataOffset, m_uStartOffset;
		ULONG m_uModuleSize, m_uCheckSum, m_uTimeDateStamp;
		PCSTR m_szModuleName, m_szImageName;

	public:
		CCreateProcessEventArgs(IDebugClient *client, ULONG64 ImageFileHandle, ULONG64 Handle, ULONG64 BaseOffset, ULONG ModuleSize, PCSTR ModuleName,	PCSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp, ULONG64 InitialThreadHandle, ULONG64 ThreadDataOffset, ULONG64 StartOffset)
			: CEventArgs(client), m_uImageFileHandle(ImageFileHandle), m_uHandle(Handle), m_uBaseOffset(BaseOffset), m_uModuleSize(ModuleSize), m_szModuleName(ModuleName), m_szImageName(ImageName), m_uCheckSum(CheckSum), m_uTimeDateStamp(TimeDateStamp), m_uInitialThreadHandle(InitialThreadHandle), m_uThreadDataOffset(ThreadDataOffset), m_uStartOffset(StartOffset)
		{}

		ULONG64 GetImageFileHandle(void) const;
		ULONG64 GetHandle(void) const;
		ULONG64 GetBaseOffset(void) const;
		ULONG GetModuleSize(void) const;
		PCSTR GetModuleName(void) const;
		PCSTR GetImageName(void) const;
		ULONG GetCheckSum(void) const;
		ULONG GetTimeDateStamp(void) const;
		ULONG64 GetInitialThreadHandle(void) const;
		ULONG64 GetThreadDataOffset(void) const;
		ULONG64 GetStartOffset(void) const;
	};

	class CExitProcessEventArgs : public CEventArgs {
	private:
		ULONG m_exitCode;
	public:
		CExitProcessEventArgs(IDebugClient *client, ULONG ExitCode)
			: CEventArgs(client), m_exitCode(ExitCode)
		{}
		ULONG GetExitCode(void) const;
	};

	class CLoadModuleEventArgs : public CEventArgs {
	private:
		ULONG64 m_uImageFileHandle, m_uBaseOffset;
		ULONG m_uModuleSize, m_uCheckSum, m_uTimeDateStamp;
		PCSTR m_szModuleName, m_szImageName;
	public:
		CLoadModuleEventArgs(IDebugClient *client, ULONG64 ImageFileHandle, ULONG64 BaseOffset, ULONG ModuleSize, PCSTR ModuleName, PCSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp)
			: CEventArgs(client), m_uImageFileHandle(ImageFileHandle), m_uBaseOffset(BaseOffset), m_uModuleSize(ModuleSize), m_szModuleName(ModuleName), m_szImageName(ImageName), m_uCheckSum(CheckSum), m_uTimeDateStamp(TimeDateStamp)
		{}

		ULONG64 GetImageFileHandle(void) const;
		ULONG64 GetBaseOffset(void) const;
		ULONG GetModuleSize(void) const;
		PCSTR GetModuleName(void) const;
		PCSTR GetImageName(void) const;
		ULONG GetCheckSum(void) const;
		ULONG GetTimeDateStamp(void) const;
	};

	class CUnloadModuleEventArgs : public CEventArgs {
	private:
		PCSTR m_szImageBaseName;
		ULONG64 m_uBaseOffset;
	public:
		CUnloadModuleEventArgs(IDebugClient *client, PCSTR ImageBaseName, ULONG64 BaseOffset)
			: CEventArgs(client), m_szImageBaseName(ImageBaseName), m_uBaseOffset(BaseOffset)
		{}
		PCSTR GetImageBaseName(void) const;
		ULONG64 GetBaseOffset(void) const;
	};

	class CSystemErrorEventArgs : public CEventArgs {
	private:
		ULONG m_uError, m_uLevel;
	public:
		CSystemErrorEventArgs(IDebugClient *client, ULONG Error, ULONG Level)
			: CEventArgs(client), m_uError(Error), m_uLevel(Level)
		{}
		ULONG GetError(void) const;
		ULONG GetLevel(void) const;
	};

	class CSessionStatusEventArgs : public CEventArgs {
	private:
		ULONG m_uStatus;
	public:
		CSessionStatusEventArgs(IDebugClient *client, ULONG Status)
			: CEventArgs(client), m_uStatus(Status)
		{}
		CDebugClient::CDebugEventCallbacks::SessionStatusFlags GetStatus(void) const;
	};

	template <class T>
	class CStateChangeEventArgs : public CEventArgs {
	private:
		ULONG m_uFlags; ULONG64 m_uArgument;
	public:
		CStateChangeEventArgs(IDebugClient *client, ULONG Flags, ULONG64 Argument)
			: CEventArgs(client), m_uFlags(Flags), m_uArgument(Argument)
		{}
		T GetFlags(void) const { return (T) m_uFlags; }
		ULONG64 GetArgument(void) const { return m_uArgument; }
	};

/** Input callbacks */
	class CDebugInputCallbacks : public CDebugCallbacks<IDebugInputCallbacks>
	{
	private:
		object m_input;

	public:
		void Get(IDebugInputCallbacks** callbacks);
		void Set(IDebugInputCallbacks* callbacks);
		static void Export(void);

	public:
		BEGIN_COM_MAP(CDebugInputCallbacks)
			COM_INTERFACE_ENTRY(IDebugInputCallbacks)
		END_COM_MAP()

		STDMETHOD(StartInput)(__in ULONG BufferSize);
		STDMETHOD(EndInput)(void);

		const object GetInput(void) const;
		void SetInput(const object& obj);
	};

	class CInputEventArgs : public CEventArgs {
	private:
		ULONG m_uBufferSize;
	public:
		CInputEventArgs(IDebugClient* client, ULONG BufferSize) :
			CEventArgs(client),m_uBufferSize(BufferSize)
		{}

		ULONG GetBufferSize(void) const;
	};

/** Output callbacks */
	class CDebugOutputCallbacks : public CDebugCallbacks<IDebugOutputCallbacks>
	{
	private:
		object m_output;

	public:
		void Get(IDebugOutputCallbacks** callbacks);
		void Set(IDebugOutputCallbacks* callbacks);
		static void Export(void);

		enum class InterestMask : ULONG {
			OUTCBI_EXPLICIT_FLUSH = DEBUG_OUTCBI_EXPLICIT_FLUSH,
			OUTCBI_TEXT = DEBUG_OUTCBI_TEXT,
			OUTCBI_DML = DEBUG_OUTCBI_DML,
			OUTCBI_ANY_FORMAT = DEBUG_OUTCBI_ANY_FORMAT,
		};

		enum class CallbackTypes : ULONG {
			OUTCB_TEXT = DEBUG_OUTCB_TEXT,
			OUTCB_DML = DEBUG_OUTCB_DML,
			OUTCB_EXPLICIT_FLUSH = DEBUG_OUTCB_EXPLICIT_FLUSH,
		};

		enum class CallbackFlags : ULONG {
			OUTCBF_COMBINED_EXPLICIT_FLUSH = DEBUG_OUTCBF_COMBINED_EXPLICIT_FLUSH,
			OUTCBF_DML_HAS_TAGS = DEBUG_OUTCBF_DML_HAS_TAGS,
			OUTCBF_DML_HAS_SPECIAL_CHARACTERS = DEBUG_OUTCBF_DML_HAS_SPECIAL_CHARACTERS,
		};

	public:
		BEGIN_COM_MAP(CDebugOutputCallbacks)
			COM_INTERFACE_ENTRY(IDebugOutputCallbacks)
		END_COM_MAP()

		STDMETHOD(Output)(__in ULONG Mask, __in PCSTR Text);

		const object GetOutput(void) const;
		void SetOutput(const object& obj);
	};
	class COutputEventArgs : public CEventArgs {
	private:
		ULONG m_mask;
		std::string m_text;

	public:
		COutputEventArgs(IDebugClient* client, ULONG Mask, __in PCSTR Text) :
			CEventArgs(client),m_mask(Mask),m_text(Text)
		{}

		CDebugOutput::OutputMask GetMask(void) const;
		const std::string GetText(void) const;
	};

private:
/** event callbacks */
	CComObjectStackEx<CDebugEventCallbacks> m_eventCallbacks;
	CComObjectStackEx<CDebugInputCallbacks> m_inputCallbacks;
	CComObjectStackEx<CDebugOutputCallbacks> m_outputCallbacks;

	// globally syncing the callbacks
	void SetInterface(IDebugClient* intf) {
		m_eventCallbacks.SetInterface(intf);
		m_inputCallbacks.SetInterface(intf);
		m_outputCallbacks.SetInterface(intf);

		m_eventCallbacks.Set(&m_eventCallbacks);
		m_inputCallbacks.Set(&m_inputCallbacks);
		m_outputCallbacks.Set(&m_outputCallbacks);
	}

public:
/** Scope */
	CDebugClient() : DebugInterface()
	{ SetInterface(m_intf); }
	CDebugClient(IUnknown *client) :
		DebugInterface(client),
		m_eventCallbacks(m_intf), m_inputCallbacks(m_intf), m_outputCallbacks(m_intf)
	{}
	CDebugClient(const CDebugClient& client) :
		DebugInterface(client.m_intf),
		m_eventCallbacks(m_intf), m_inputCallbacks(m_intf), m_outputCallbacks(m_intf)
	{}
	~CDebugClient(void) {}

	static void Export(void);

/** Inlines */
	inline const CDebugControl& GetControl(void) const
	{ return CDebugControl(m_intf); }
	inline const CDebugRegisters& GetRegisters(void) const
	{ return CDebugRegisters(m_intf); }
	inline const CDebugSymbols& GetSymbols(void) const
	{ return CDebugSymbols(m_intf); }
	inline const CDebugDataSpaces& GetDataSpaces(void) const
	{ return CDebugDataSpaces(m_intf); }
	inline const CDebugSystemObjects& GetSystemObjects(void) const
	{ return CDebugSystemObjects(m_intf); }
	inline const CDebugAdvanced& GetAdvanced(void) const
	{ return CDebugAdvanced(m_intf); }

/** Methods */
	ULONG GetExitCode(void) const;
	const std::string GetIdentity(void) const;
	void OutputIdentity(const std::string& format="%s", CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	// Kernel debugging
	void AttachKernel(AttachKernelFlags flags=AttachKernelFlags::ATTACH_LOCAL_KERNEL, const std::string& connectOptions="") const;
	bool IsKernelDebuggerEnabled(void) const;
	const std::string GetKernelConnectionOptions(void) const;
	void SetKernelConnectionOptions(const std::string& options) const;

	// Callback methods
	bool DispatchCallbacks(ULONG timeout = INFINITE) const;
	void ExitDispatch(void) const;

	ULONG GetNumberOfEventCallbacks(CDebugEventCallbacks::InterestMask event) const;
	ULONG GetNumberOfInputCallbacks(void) const;
	ULONG GetNumberOfOutputCallbacks(void) const;

	// Output methods
	CDebugOutput::OutputMask GetOutputMask(void) const;
	void SetOutputMask(CDebugOutput::OutputMask mask) const;
	ULONG GetOutputWidth(void) const;
	void SetOutputWidth(ULONG width) const;

	const std::string GetQuitLockString(void) const;
	void SetQuitLockString(const std::string str) const;

	// Dumpfile methods
	void OpenDumpFile(const std::string& filename) const;
	void AddDumpInformationFile(const std::string& filename) const;
	const dict GetDumpFiles(void) const;
	void WriteUserDumpFile(const std::string& filename, CDebugControl::UserDebuggee qualifier=CDebugControl::UserDebuggee::USER_SMALL_DUMP, DumpFormat format=CDebugClient::DumpFormat::FORMAT_DEFAULT, const std::string& comment="") const;
	void WriteKernelDumpFile(const std::string& filename, CDebugControl::KernelDebuggee qualifier=CDebugControl::KernelDebuggee::KERNEL_SMALL_DUMP, DumpFormat format=CDebugClient::DumpFormat::FORMAT_DEFAULT, const std::string& comment="") const;

	// Process methods
	ULONG64 ConnectProcessServer(const std::string& remoteOptions) const;
	void DisconnectProcessServer(ULONG64 Server) const;

	const list GetRunningProcessSystemIds(ULONG64 server=DEFAULT_PROCESS_SERVER) const;
	tuple GetRunningProcessDescription(ULONG systemId, ULONG flags=0, ULONG64 server=DEFAULT_PROCESS_SERVER) const;

/* FIXME
	void CreateProcess(const std::string& commandLine, const std::string& initialDirectory = "",
	dict environmentVariables = dict(), PyCreateFlags createFlags = CREATE_DEFAULT,
	CDebugClient::AttachFlags attachFlags = ATTACH_DEFAULT, ULONG attachProcessId = 0, ULONG64 server = DEFAULT_PROCESS_SERVER) const;
*/
	HRESULT AttachProcess(ULONG id, CDebugClient::AttachFlags flags=AttachFlags::ATTACH_DEFAULT, ULONG64 server=CDebugClient::DEFAULT_PROCESS_SERVER) const;
	void DetachProcesses(void) const;
	void TerminateProcesses(void) const;
	void AbandonCurrentProcess(void) const;
	void DetachCurrentProcess(void) const;
	void TerminateCurrentProcess(void) const;
};
#endif
