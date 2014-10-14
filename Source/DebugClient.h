#pragma once

#include <string>
#include <map>

#include "DebugObject.h"

#include "DebugEngine.h"
#include "DebugControl.h"
#include "DebugRegisters.h"
#include "DebugSymbols.h"
#include "DebugDataSpaces.h"
#include "DebugSystemObjects.h"
#include "DebugAdvanced.h"

typedef void (*PUseClient)(PDEBUG_CLIENT);
typedef PDEBUG_CLIENT (*PReleaseClient)();

class CDebugClient : public CDebugObject<IDebugClient>
{
  typedef CDebugObject<DefaultInterface> __inherited;
  static CComQIPtr<DefaultInterface> s_current;	// XXX: somehow remove the dependency on this maybe?

public:
  static const ULONG DEFAULT_PROCESS_SERVER = 0;

  /* accessors */
  static PDEBUG_CLIENT SetDebugClient(PDEBUG_CLIENT c) {
	  PDEBUG_CLIENT result;
	  result = CDebugClient::s_current.p;
	  CDebugClient::s_current = c;
	  return result;
  }
  static PDEBUG_CLIENT GetDebugClient() {
	  return CDebugClient::s_current.p;
  }

  enum CreateFlags
  {
    CREATE_DEFAULT                  = 0,
    CREATE_ATTACH_PROCESS           = DEBUG_PROCESS,
    CREATE_ATTACH_ONLY_THIS_PROCESS = DEBUG_ONLY_THIS_PROCESS,
    CREATE_SUSPENDED_               = CREATE_SUSPENDED,
    CREATE_NO_DEBUG_HEAP            = DEBUG_CREATE_PROCESS_NO_DEBUG_HEAP,
    CREATE_THROUGH_RTL              = DEBUG_CREATE_PROCESS_THROUGH_RTL
  };

  enum AttachUserFlags
  {
    ATTACH_DEFAULT                    = DEBUG_ATTACH_DEFAULT,
    ATTACH_NONINVASIVE                = DEBUG_ATTACH_NONINVASIVE,
    ATTACH_EXISTING                   = DEBUG_ATTACH_EXISTING,
    ATTACH_NONINVASIVE_NO_SUSPEND     = DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND,
    ATTACH_INVASIVE_NO_INITIAL_BREAK  = DEBUG_ATTACH_INVASIVE_NO_INITIAL_BREAK,
    ATTACH_INVASIVE_RESUME_PROCESS    = DEBUG_ATTACH_INVASIVE_RESUME_PROCESS,
    ATTACH_NONINVASIVE_ALLOW_PARTIAL  = DEBUG_ATTACH_NONINVASIVE_ALLOW_PARTIAL
  };

  enum AttachKernelFlag
  {
    ATTACH_KERNEL_CONNECTION  = DEBUG_ATTACH_KERNEL_CONNECTION,
    ATTACH_LOCAL_KERNEL       = DEBUG_ATTACH_LOCAL_KERNEL,
    ATTACH_EXDI_DRIVER        = DEBUG_ATTACH_EXDI_DRIVER
  };

  enum ChangeStateFlags
  {
    CDS_ALL       = DEBUG_CDS_ALL,
    CDS_REGISTERS = DEBUG_CDS_REGISTERS,
    CDS_DATA      = DEBUG_CDS_DATA
  };

  enum DebugSessionStatus
  {
    SESSION_ACTIVE                       = DEBUG_SESSION_ACTIVE,                       
    SESSION_END_SESSION_ACTIVE_TERMINATE = DEBUG_SESSION_END_SESSION_ACTIVE_TERMINATE, 
    SESSION_END_SESSION_ACTIVE_DETACH    = DEBUG_SESSION_END_SESSION_ACTIVE_DETACH,    
    SESSION_END_SESSION_PASSIVE          = DEBUG_SESSION_END_SESSION_PASSIVE,          
    SESSION_END                          = DEBUG_SESSION_END,                          
    SESSION_REBOOT                       = DEBUG_SESSION_REBOOT,                       
    SESSION_HIBERNATE                    = DEBUG_SESSION_HIBERNATE,                    
    SESSION_FAILURE                      = DEBUG_SESSION_FAILURE                      
  };

  enum DebugEvent
  {
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

  enum DumpFormat
  {
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
    FORMAT_USER_SMALL_FULL_AUXILIARY_STATE      = DEBUG_FORMAT_USER_SMALL_FULL_AUXILIARY_STATE      
  };

    class CEventArgs {
    private:    
        IDebugClient *m_intf;
    protected:
        CEventArgs(IDebugClient *client)
        : m_intf(client) {}
    public:
        const CDebugClient GetOwner(void) const { return CDebugClient(m_intf); }
        const CDebugControl* GetController(void) const { return new CDebugControl(m_intf); }
    };

    class CBreakpointEventArgs : public CEventArgs {
    private:
        PDEBUG_BREAKPOINT m_bp;
    public:
        CBreakpointEventArgs(IDebugClient *client, PDEBUG_BREAKPOINT bp)
        : CEventArgs(client), m_bp(bp)
            {}

        /* FIXME: i think we lose a reference here... maybe COM/ATL will handle it for us */
        const CDebugControl::CBreakpoint GetBreakpoint(void) const  { 
            return CDebugControl::CBreakpoint(GetController(), m_bp);
        }
/*
    //flier lu's original -- is this function misguided?
        const CDebugControl::CBreakpoint GetBreakpoint(void) const  { 
            std::cout << "error here, wtf. snagging a CDebugClient here causes a reference underflow..." << std::endl;
            return CDebugControl::CBreakpoint(NULL, m_bp);
        }
*/
    };

    class CExceptionEventArgs : public CEventArgs {
    private:   
        PEXCEPTION_RECORD64 m_pRecord;
        ULONG m_bFirstChance;
    public:
        CExceptionEventArgs(IDebugClient *client, PEXCEPTION_RECORD64 pException, ULONG bFirstChance)
        : CEventArgs(client), m_pRecord(pException), m_bFirstChance(bFirstChance)
            {}

        DWORD GetExceptionCode(void) const { return m_pRecord->ExceptionCode; }
        DWORD GetExceptionFlags(void) const { return m_pRecord->ExceptionFlags; }
        DWORD64 GetExceptionAddress(void) const { return m_pRecord->ExceptionAddress; }
        const list GetParameters(void) const;
        
        bool IsFirstChance(void) const { return m_bFirstChance; }
    };

    class CCreateThreadEventArgs : public CEventArgs {
    private:
        ULONG64 m_handle, m_dataOffset, m_startOffset;
    public:
        CCreateThreadEventArgs(IDebugClient *client, ULONG64 Handle, ULONG64 DataOffset, ULONG64 StartOffset)
        : CEventArgs(client), m_handle(Handle), m_dataOffset(DataOffset), m_startOffset(StartOffset)
            {}

        ULONG64 GetHandle(void) const { return m_handle; } 
        ULONG64 GetDataOffset(void) const { return m_dataOffset; }
        ULONG64 GetStartOffset(void) const { return m_startOffset; }
    };

    class CExitThreadEventArgs : public CEventArgs {
    private:
        ULONG m_exitCode;
    public:
        CExitThreadEventArgs(IDebugClient *client, ULONG ExitCode)
        : CEventArgs(client), m_exitCode(ExitCode)
            {}
    
        ULONG GetExitCode(void) const { return m_exitCode; }
    };

    class CCreateProcessEventArgs : public CEventArgs
    {
    private:
        ULONG64 m_uImageFileHandle, m_uHandle, m_uBaseOffset,
        m_uInitialThreadHandle, m_uThreadDataOffset, m_uStartOffset;
        ULONG m_uModuleSize, m_uCheckSum, m_uTimeDateStamp;
        PCSTR m_szModuleName, m_szImageName;

    public:
        CCreateProcessEventArgs(IDebugClient *client, ULONG64 ImageFileHandle, ULONG64 Handle, ULONG64 BaseOffset,
                                ULONG ModuleSize, PCSTR ModuleName,  PCSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp,
                                ULONG64 InitialThreadHandle, ULONG64 ThreadDataOffset, ULONG64 StartOffset)
        : CEventArgs(client), m_uImageFileHandle(ImageFileHandle), m_uHandle(Handle), m_uBaseOffset(BaseOffset),
          m_uModuleSize(ModuleSize), m_szModuleName(ModuleName), m_szImageName(ImageName), m_uCheckSum(CheckSum),
          m_uTimeDateStamp(TimeDateStamp), m_uInitialThreadHandle(InitialThreadHandle), m_uThreadDataOffset(ThreadDataOffset),
          m_uStartOffset(StartOffset)
            {}
    
        ULONG64 GetImageFileHandle(void) const { return m_uImageFileHandle; }
        ULONG64 GetHandle(void) const { return m_uHandle; }
        ULONG64 GetBaseOffset(void) const { return m_uBaseOffset; }
        ULONG GetModuleSize(void) const { return m_uModuleSize; }
        PCSTR GetModuleName(void) const { return m_szModuleName; } 
        PCSTR GetImageName(void) const { return m_szImageName; }
        ULONG GetCheckSum(void) const { return m_uCheckSum; }
        ULONG GetTimeDateStamp(void) const { return m_uTimeDateStamp; } 
        ULONG64 GetInitialThreadHandle(void) const { return m_uInitialThreadHandle; }
        ULONG64 GetThreadDataOffset(void) const { return m_uThreadDataOffset; }
        ULONG64 GetStartOffset(void) const { return m_uStartOffset; }
    };

    class CExitProcessEventArgs : public CEventArgs {
    private:
        ULONG m_exitCode;
    public:
        CExitProcessEventArgs(IDebugClient *client, ULONG ExitCode)
        : CEventArgs(client), m_exitCode(ExitCode)
            { }
        
        ULONG GetExitCode(void) const { return m_exitCode; }
    };

    class CLoadModuleEventArgs : public CEventArgs {
    private:
        ULONG64 m_uImageFileHandle, m_uBaseOffset;
        ULONG m_uModuleSize, m_uCheckSum, m_uTimeDateStamp;
        PCSTR m_szModuleName, m_szImageName;
    public:
        CLoadModuleEventArgs(IDebugClient *client, ULONG64 ImageFileHandle, ULONG64 BaseOffset, ULONG ModuleSize,
                             PCSTR ModuleName, PCSTR ImageName, ULONG CheckSum, ULONG TimeDateStamp)
        : CEventArgs(client), m_uImageFileHandle(ImageFileHandle), m_uBaseOffset(BaseOffset), m_uModuleSize(ModuleSize),
        m_szModuleName(ModuleName), m_szImageName(ImageName), m_uCheckSum(CheckSum), m_uTimeDateStamp(TimeDateStamp)
            { }
    
        ULONG64 GetImageFileHandle(void) const { return m_uImageFileHandle; }
        ULONG64 GetBaseOffset(void) const { return m_uBaseOffset; }
        ULONG GetModuleSize(void) const { return m_uModuleSize; }
        PCSTR GetModuleName(void) const { return m_szModuleName; }
        PCSTR GetImageName(void) const { return m_szImageName; } 
        ULONG GetCheckSum(void) const { return m_uCheckSum; }
        ULONG GetTimeDateStamp(void) const { return m_uTimeDateStamp; }
    };

    class CUnloadModuleEventArgs : public CEventArgs {
    private:
        PCSTR m_szImageBaseName;
        ULONG64 m_uBaseOffset;
    public:
        CUnloadModuleEventArgs(IDebugClient *client, PCSTR ImageBaseName, ULONG64 BaseOffset)
        : CEventArgs(client), m_szImageBaseName(ImageBaseName), m_uBaseOffset(BaseOffset)
            {}
    
        PCSTR GetImageBaseName(void) const { return m_szImageBaseName; }
        ULONG64 GetBaseOffset(void) const { return m_uBaseOffset; }
    };

    class CSystemErrorEventArgs : public CEventArgs {
    private:
        ULONG m_uError, m_uLevel;
    public:
        CSystemErrorEventArgs(IDebugClient *client, ULONG Error, ULONG Level)
        : CEventArgs(client), m_uError(Error), m_uLevel(Level)
            {}
    
        ULONG GetError(void) const { return m_uError; }
        ULONG GetLevel(void) const { return m_uLevel; }
    };

    class CSessionStatusEventArgs : public CEventArgs {
    private:
        ULONG m_uStatus;
    public:
        CSessionStatusEventArgs(IDebugClient *client, ULONG Status)
        : CEventArgs(client), m_uStatus(Status)
            { }
    
        DebugSessionStatus GetStatus(void) const { return (DebugSessionStatus) m_uStatus; }
    };

    class CStateChangeEventArgs : public CEventArgs {
    private:
        ULONG m_uFlags;
        ULONG64 m_uArgument;
    public:
        CStateChangeEventArgs(IDebugClient *client, ULONG Flags, ULONG64 Argument)
        : CEventArgs(client), m_uFlags(Flags), m_uArgument(Argument)
            {}
    
        ChangeStateFlags GetFlags(void) const { return (ChangeStateFlags) m_uFlags; }
        ULONG64 GetArgument(void) const { return m_uArgument; }
    };

  /*************
	  All Callbacks inherit from this object.
  */

    class CDebugCallbacksBase : public CComObjectRootEx<CComMultiThreadModel>
    {
    protected:
        IDebugClient* m_intf;
    
    public:
        /** Assigns an interface to a debug client. This is used to access callbacks */
        IDebugClient* SetInterface(IDebugClient* intf) {
            IDebugClient* result;
            result = m_intf;
            m_intf = intf;
            return result;
        }
    };
    
    template <typename I>
    class CDebugCallbacks : public CDebugCallbacksBase,public I
    {
    protected:
		CComQIPtr<I> m_callbacks;

    public:
        virtual void Get(I**) { assert(false); }
        virtual void Set(I*) { assert(false); }

        IDebugClient* SetInterface(IDebugClient* intf) {
            IDebugClient* result;
            result = CDebugCallbacksBase::SetInterface(intf);
            Get(&m_callbacks);
            return result;
        }        
    };
    
    class CDebugEventCallbacks : public CDebugCallbacks<IDebugEventCallbacks>
    {
    public:
        void Get(IDebugEventCallbacks** callbacks) {
            HRESULT hr = m_intf->GetEventCallbacks(callbacks);
            Check(hr);
        }
        void Set(IDebugEventCallbacks* callbacks) {
            HRESULT hr = m_intf->SetEventCallbacks(callbacks);
            Check(hr);
        }

    protected:
        object CDebugClient::CDebugEventCallbacks::GetEvent(ULONG id) const {
            CallbackMap::const_iterator it = m_eventCallbacks.find(id);
            return it == m_eventCallbacks.end()? object(borrowed(Py_None)) : it->second;
        }
        void CDebugClient::CDebugEventCallbacks::SetEvent(ULONG id, object event)  {
            if (event.ptr() == Py_None)
                m_eventCallbacks.erase(id);
            else
                m_eventCallbacks.insert(std::make_pair(id, event));
            this->Set(this);
        }

    private:
        typedef std::map<ULONG, object> CallbackMap;
        CallbackMap m_eventCallbacks;

        template <typename T>
        HRESULT CallEventCallback(ULONG id, const T& args)
        {
            CallbackMap::const_iterator it = m_eventCallbacks.find(id);

            if (it == m_eventCallbacks.end()) {
                return CDebugControl::STATUS_IGNORE_EVENT;  // no events found
            }

            /* FIXME: need to figure out how to unwind the stack, and reraise the exception for python */
            object result;        
            try {
                result = it->second(args);

            } catch (...) {
                if (PyErr_Occurred()) {
                    PyErr_Print();      // XXX: maybe we should save the exception state before we use python again?
                    import("logging").attr("warn")("_PyDbgEng.DebugClient.EventCallbacks : Exception raised by python while handling event");
                    throw_error_already_set();
                } else {
                    import("logging").attr("warn")("_PyDbgEng.DebugClient.EventCallbacks : Exception raised by callback while handling event");
                }
                return CDebugControl::STATUS_BREAK;
            }
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
    class CInputEventArgs : public CEventArgs {
    private:
        ULONG m_uBufferSize;
    public:
        CInputEventArgs(IDebugClient* client, ULONG BufferSize) : CEventArgs(client),m_uBufferSize(BufferSize) {}
        ULONG GetBufferSize(void) const { return m_uBufferSize; }
    };
    class CDebugInputCallbacks : public CDebugCallbacks<IDebugInputCallbacks>
    {    
    private:
        object m_input;

    public:
//        ~CDebugInputCallbacks() { Set(&m_callbacks); }

        void Get(IDebugInputCallbacks** callbacks) { HRESULT hr = m_intf->GetInputCallbacks(callbacks); Check(hr); }
        void Set(IDebugInputCallbacks* callbacks) { HRESULT hr = m_intf->SetInputCallbacks(callbacks); Check(hr);}

    public:
        BEGIN_COM_MAP(CDebugInputCallbacks)
            COM_INTERFACE_ENTRY(IDebugInputCallbacks)
        END_COM_MAP()
    
        STDMETHOD(StartInput)(__in ULONG BufferSize);    
        STDMETHOD(EndInput)(void);
        
        const object GetInput(void) const { return m_input; }
        void SetInput(const object& obj) { m_input = obj; }
    };
    class COutputEventArgs : public CEventArgs {
    private:
        ULONG m_mask;
        std::string m_text;
    public:
        COutputEventArgs(IDebugClient* client, ULONG Mask, __in PCSTR Text) : CEventArgs(client),m_mask(Mask),m_text(Text) {}

        OutputMask GetMask(void) const { return (OutputMask) m_mask; }
        const std::string GetText(void) const { return m_text; }
    };

    class CDebugOutputCallbacks : public CDebugCallbacks<IDebugOutputCallbacks>
    {
    private:
        object m_output;

    public:
//        virtual ~CDebugOutputCallbacks() { Set(&m_callbacks); }

        void Get(IDebugOutputCallbacks** callbacks) { HRESULT hr = m_intf->GetOutputCallbacks(callbacks); Check(hr); }
        void Set(IDebugOutputCallbacks* callbacks) { HRESULT hr = m_intf->SetOutputCallbacks(callbacks); Check(hr);}

    public:
        BEGIN_COM_MAP(CDebugOutputCallbacks)
            COM_INTERFACE_ENTRY(IDebugOutputCallbacks)
        END_COM_MAP()
    
        STDMETHOD(Output)(__in ULONG Mask, __in PCSTR Text);

        const object GetOutput(void) const { return m_output; }
        void SetOutput(const object& obj) { m_output = obj; }
    };

  struct Scope
  {
    CComPtr<DefaultInterface> m_outer;

	Scope(PDEBUG_CLIENT4 Client);
	~Scope(void);
  };

private:
/* CDebugClient's event callback things */

// FIXME: there's a memory leak somewhere when assigning callbacks?
  CComObjectStackEx<CDebugEventCallbacks> m_eventCallbacks;
  CComObjectStackEx<CDebugInputCallbacks> m_inputCallbacks;
  CComObjectStackEx<CDebugOutputCallbacks> m_outputCallbacks;

    /** globally syncing the callbacks */
    void __update() {
        m_eventCallbacks.Set(&m_eventCallbacks);
        m_inputCallbacks.Set(&m_inputCallbacks);
        m_outputCallbacks.Set(&m_outputCallbacks);
    }

public:
	/* Constructors */
    CDebugClient() : __inherited(CDebugEngine::CreateInstance<CDebugClient::DefaultInterface>()) {
        m_eventCallbacks.SetInterface(m_intf);
        m_inputCallbacks.SetInterface(m_intf);
        m_outputCallbacks.SetInterface(m_intf);
        __update();
    }
    CDebugClient(IUnknown *client) : __inherited(client) {
        m_eventCallbacks.SetInterface(m_intf);
        m_inputCallbacks.SetInterface(m_intf);
        m_outputCallbacks.SetInterface(m_intf);
        __update();
    }
    CDebugClient(const CDebugClient& client) : __inherited(client.m_intf) {
        m_eventCallbacks.SetInterface(m_intf);
        m_inputCallbacks.SetInterface(m_intf);
        m_outputCallbacks.SetInterface(m_intf);
        __update();
    }
    ~CDebugClient(void) {}

  static void Export(void);

  static const CDebugClient GetCurrent(void) 
  {
    return CDebugClient(s_current);
  }

  const CDebugControl GetControl(void) const
  {
    return CDebugControl(m_intf);
  }
  const CDebugRegisters GetRegisters(void) const
  {
    return CDebugRegisters(m_intf);
  }
  const CDebugSymbols GetSymbols(void) const
  {
    return CDebugSymbols(m_intf);
  }
  const CDebugDataSpaces GetDataSpaces(void) const
  {
    return CDebugDataSpaces(m_intf);
  }
  const CDebugSystemObjects GetSystemObjects(void) const
  {
    return CDebugSystemObjects(m_intf);
  }
  const CDebugAdvanced GetAdvanced(void) const
  {
    return CDebugAdvanced(m_intf);
  }

  ULONG GetExitCode(void) const;
  const std::string GetIdentity(void) const;
  void OutputIdentity(const std::string& format = "%s", OutputTarget target = TARGET_ALL_CLIENTS) const;

  void CreateProcess(const std::string& commandLine, const std::string& initialDirectory = "", 
    dict environmentVariables = dict(), CreateFlags createFlags = CREATE_DEFAULT, 
    AttachUserFlags attachFlags = ATTACH_DEFAULT, ULONG attachProcessId = 0, ULONG64 server = DEFAULT_PROCESS_SERVER) const;
  HRESULT AttachProcess(ULONG id, AttachUserFlags flags = ATTACH_DEFAULT, ULONG64 server = DEFAULT_PROCESS_SERVER) const;
  void DetachProcesses(void) const;
  void TerminateProcesses(void) const;
  void AbandonCurrentProcess(void) const;
  void DetachCurrentProcess(void) const;
  void TerminateCurrentProcess(void) const;

  ULONG64 ConnectProcessServer(const std::string& remoteOptions) const;
  void DisconnectProcessServer(ULONG64 Server) const;

  void AttachKernel(AttachKernelFlag flags = ATTACH_LOCAL_KERNEL, const std::string& connectOptions = "") const;
  bool IsKernelDebuggerEnabled(void) const;

  const std::string GetKernelConnectionOptions(void) const;
  void SetKernelConnectionOptions(const std::string& options) const;

  bool DispatchCallbacks(ULONG timeout = INFINITE) const;
  void ExitDispatch(void) const;

  ULONG GetNumberOfEventCallbacks(DebugEvent event) const;
  ULONG GetNumberOfInputCallbacks(void) const;
  ULONG GetNumberOfOutputCallbacks(void) const;

  OutputMask GetOutputMask(void) const;
  void SetOutputMask(OutputMask mask) const;
  ULONG GetOutputWidth(void) const;
  void SetOutputWidth(ULONG width) const;

  const std::string GetQuitLockString(void) const;
  void SetQuitLockString(const std::string str) const;

  void OpenDumpFile(const std::string& filename) const;
  void AddDumpInformationFile(const std::string& filename) const;
  const dict GetDumpFiles(void) const;
  void WriteUserDumpFile(const std::string& filename, UserQualifier qualifier = USER_SMALL_DUMP, DumpFormat format = FORMAT_DEFAULT, const std::string& comment = "") const;
  void WriteKernelDumpFile(const std::string& filename, KernelQualifier qualifier = KERNEL_SMALL_DUMP, DumpFormat format = FORMAT_DEFAULT, const std::string& comment = "") const;

  const list GetRunningProcessSystemIds(ULONG64 server=DEFAULT_PROCESS_SERVER) const;
  tuple GetRunningProcessDescription(ULONG systemId, ULONG flags=0, ULONG64 server=DEFAULT_PROCESS_SERVER) const;
};
