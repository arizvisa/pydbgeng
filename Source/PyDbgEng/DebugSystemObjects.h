#pragma once

#include <vector>

#include "DebugObject.h"

#include "DebugDataSpaces.h"

class CDebugSystemObjects
  : public CDebugObject<IDebugSystemObjects3>
{
  typedef CDebugObject<IDebugSystemObjects3> __inherited;
public:
  class CDebugSystem : public __inherited
  {
    ULONG m_id;
  public:
    CDebugSystem(const CDebugSystemObjects *owner, ULONG id) 
      : __inherited(owner->GetInterface()), m_id(id)
    {
      
    }

    ULONG GetId(void) const { return m_id; }

    static const object Repr(const CDebugSystem& system)
    {
      return "(System %d)" % make_tuple(system.GetId());
    }
  };

  class CSystemObject : public __inherited
  {
  protected:
    ULONG m_engineId, m_systemId;

    CSystemObject(const CDebugSystemObjects *owner, ULONG engineId, ULONG systemId)
      : __inherited(owner->GetInterface()), m_engineId(engineId), m_systemId(systemId)
    {

    }
  public:
    ULONG GetEngineId(void) const { return m_engineId; }
    ULONG GetSystemId(void) const { return m_systemId; }
  };

  class CThread : public CSystemObject
  {
  public:
    CThread(const CDebugSystemObjects *owner, ULONG engineId, ULONG systemId)
      : CSystemObject(owner, engineId, systemId)
    {

    }

    static const object Repr(const CThread& thread)
    {
      return "(Thread %d %d)" % make_tuple(thread.GetEngineId(), thread.GetSystemId());
    }
  };

  class CCurrentThread : public CThread
  {
  public:
    CCurrentThread(const CDebugSystemObjects *owner, ULONG engineId, ULONG systemId)
      : CThread(owner, engineId, systemId)
    {

    }

    CCurrentThread(const CThread& thread) : CThread(thread)
    {

    }

    ULONG64 GetDataOffset(void) const;
    const CDebugDataSpaces::CHandle GetHandle(void) const;
  };

  class CProcess : public CSystemObject
  {
  public:
    CProcess(const CDebugSystemObjects *owner, ULONG engineId, ULONG systemId)
      : CSystemObject(owner, engineId, systemId)
    {

    }

    static const object Repr(const CProcess& process)
    {
      return "(Process %d %d)" % make_tuple(process.GetEngineId(), process.GetSystemId());
    }
  };

  class CCurrentProcess : public CProcess
  {
  public:
    CCurrentProcess(const CDebugSystemObjects *owner, ULONG engineId, ULONG systemId)
      : CProcess(owner, engineId, systemId)
    {

    }

    CCurrentProcess(const CProcess& process) : CProcess(process)
    {

    }

    ULONG64 GetDataOffset(void) const;
    const CDebugDataSpaces::CHandle GetHandle(void) const;
    const std::string GetExecutableName(void) const;
    const object GetUpTime(void) const;
  };

  const std::pair<ULONG, ULONG> GetTotalNumberThreads(void) const;
public:
  CDebugSystemObjects(IUnknown *intf) : __inherited(intf)
  {
  }

  static void Export(void);

  const CDebugSystem GetEventSystem(void) const;
  const CProcess GetEventProcess(void) const;
  const CThread GetEventThread(void) const;

  const CDebugSystem GetCurrentSystem(void) const;
  void SetCurrentSystem(const CDebugSystem& system) const;
  const CCurrentProcess GetCurrentProcess(void) const;
  const ULONG GetCurrentProcessSystemId(void) const;
  const ULONG GetCurrentThreadSystemId(void) const;
  void SetCurrentProcess(const CProcess& process) const;
  const CCurrentThread GetCurrentThread(void) const;
  void SetCurrentThread(const CThread& thread) const;

  ULONG64 GetImplicitProcessDataOffset(void) const;
  void SetImplicitProcessDataOffset(ULONG64 offset) const;
  ULONG64 GetImplicitThreadDataOffset(void) const;
  void SetImplicitThreadDataOffset(ULONG64 offset) const;

  const list GetSystems(void) const;
  const list GetThreads(void) const;
  const list GetProcesses(void) const;

  ULONG GetTotalNumberOfThreads(void) const { return GetTotalNumberThreads().first; }
  ULONG GetLargestNumberOfThreads(void) const { return GetTotalNumberThreads().second; }

  const CThread GetThreadByEngineId(ULONG engineId) const;
  const CProcess GetProcessByEngineId(ULONG engineId) const;

  const CThread GetThreadBySystemId(ULONG systemId) const;
  const CProcess GetProcessBySystemId(ULONG systemId) const;

  const CThread GetThreadByTeb(ULONG64 offset) const;
  const CProcess GetProcessByPeb(ULONG64 offset) const;

  //
  const ULONG GetProcessIdByHandle(ULONG64 handle) const;
  const ULONG GetProcessIdBySystemId(ULONG SysId) const;
  const ULONG GetThreadIdByHandle(ULONG64 handle) const;
  const ULONG GetThreadIdBySystemId(ULONG SysId) const;

  const ULONG GetCurrentProcessId(void) const;
  const ULONG GetCurrentThreadId(void) const;
  const ULONG64 GetCurrentProcessHandle() const;
  const ULONG64 GetCurrentThreadHandle() const;

  const HRESULT SetCurrentProcessId(ULONG id) const;
  const HRESULT SetCurrentThreadId(ULONG id) const;
  const ULONG64 GetCurrentProcessPeb() const;
  const ULONG64 GetCurrentThreadTeb() const;
  //
  const CThread GetThreadByProcessor(ULONG processor) const;

  ULONG GetNumberThreads(void) const;
  const list CDebugSystemObjects::GetThreadIdsByIndex(ULONG start, ULONG count) const;
};

template <>
CDebugObject<IDebugSystemObjects>::operator IDebugSystemObjects*(void) const { return m_intf; }