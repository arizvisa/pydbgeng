#include "StdAfx.h"
#include "DebugSystemObjects.h"

#include <vector>
#include <algorithm>

#include <datetime.h>

void CDebugSystemObjects::Export(void)
{
  if (::Py_IsInitialized() && !PyDateTimeAPI) PyDateTime_IMPORT;

  scope DebugSystemObjects = class_<CDebugSystemObjects>("DebugSystemObjects", no_init)
    .add_property("EventSystem", &CDebugSystemObjects::GetEventSystem,
      "the engine target for the target in which the last event occurred.")
    .add_property("EventProcess", &CDebugSystemObjects::GetEventProcess,
      "the engine process for the process on which the last event occurred.")
    .add_property("EventThread", &CDebugSystemObjects::GetEventThread,
      "the engine thread for the thread on which the last event occurred.")

    .add_property("CurrentSystem", &CDebugSystemObjects::GetCurrentSystem, &CDebugSystemObjects::SetCurrentSystem, "the engine target for the current process.")
    .add_property("CurrentProcess", &CDebugSystemObjects::GetCurrentProcess, &CDebugSystemObjects::SetCurrentProcess, "the current process.")
    .add_property("CurrentThread", &CDebugSystemObjects::GetCurrentThread, &CDebugSystemObjects::SetCurrentThread, "the current thread.")

    .add_property("ImplicitProcessDataOffset", &CDebugSystemObjects::GetImplicitProcessDataOffset, &CDebugSystemObjects::SetImplicitProcessDataOffset,
      "the implicit process for the current target.")
    .add_property("ImplicitThreadDataOffset", &CDebugSystemObjects::GetImplicitThreadDataOffset, &CDebugSystemObjects::SetImplicitThreadDataOffset,
      "the implicit thread for the current target.")

    .add_property("Systems", &CDebugSystemObjects::GetSystems, "targets to which the engine is currently connected.")
    .add_property("Threads", &CDebugSystemObjects::GetThreads, "threads in the current process.")
    .add_property("Processes", &CDebugSystemObjects::GetProcesses, "processes for the current target.")

    .add_property("TotalNumberOfThreads", &CDebugSystemObjects::GetTotalNumberOfThreads,
      "the total number of threads for all the processes in the current target.")
    .add_property("LargestNumberOfThreads", &CDebugSystemObjects::GetLargestNumberOfThreads,
      "the largest number of threads in any process for the current target.")

    .def("GetThread", &CDebugSystemObjects::GetThreadBySystemId,
      "returns the thread specified by its system thread ID.")
      
    .def("GetProcess", &CDebugSystemObjects::GetProcessBySystemId,
      "returns the process specified by its system process ID.")

    .def("GetCurrentProcessSystemId", &CDebugSystemObjects::GetCurrentProcessSystemId,
      "the current process.")
    .def("GetCurrentThreadSystemId", &CDebugSystemObjects::GetCurrentThreadSystemId,
      "the current thread.")

    /*
    These were added because I honestly would rather have Python raise an exception rather than
    flier's choice of code.
    */

    .def("GetCurrentProcessId", &CDebugSystemObjects::GetCurrentProcessId,
      "the current process.")
    .def("GetCurrentThreadId", &CDebugSystemObjects::GetCurrentThreadId,
        "the current thread")

    .def("GetProcessIdByHandle", &CDebugSystemObjects::GetProcessIdByHandle,
        "returns the process id when given a handle")
    .def("GetProcessIdBySystemId", &CDebugSystemObjects::GetProcessIdBySystemId,
        "returns the process id when given a system id")

    .def("GetThreadIdByHandle", &CDebugSystemObjects::GetThreadIdByHandle,
        "returns the thread id when given a handle")
    .def("GetThreadIdBySystemId", &CDebugSystemObjects::GetThreadIdBySystemId,
        "returns the thread id when given a system id")

    .def("SetCurrentProcessId", &CDebugSystemObjects::SetCurrentProcessId,
      "setting the current process.")
    .def("SetCurrentThreadId", &CDebugSystemObjects::SetCurrentThreadId,
        "setting the current thread")

    .def("GetCurrentProcessHandle", &CDebugSystemObjects::GetCurrentProcessHandle,
      "handle of the current process.")
    .def("GetCurrentThreadHandle", &CDebugSystemObjects::GetCurrentThreadHandle,
      "handle of the current thread.")

    .def("GetCurrentProcessPeb", &CDebugSystemObjects::GetCurrentProcessPeb,
      "peb of the current process.")
    .def("GetCurrentThreadTeb", &CDebugSystemObjects::GetCurrentThreadTeb,
      "teb of the current thread.")

////////
	.def("GetNumberThreads", &CDebugSystemObjects::GetNumberThreads,
	  "returns the number of threads in the current process.")

	.def("GetThreadIdsByIndex", &CDebugSystemObjects::GetThreadIdsByIndex,
	  "returns a list of tuples containing the requested thread ids.")
	;

  class_<CDebugSystemObjects::CDebugSystem>("DebugSystem", no_init)
    .add_property("Id", &CDebugSystemObjects::CDebugSystem::GetId)

    .def("__repr__", &CDebugSystemObjects::CDebugSystem::Repr)
    ;

  class_<CDebugSystemObjects::CSystemObject>("SystemObject", no_init)
    .add_property("EngineId", &CDebugSystemObjects::CSystemObject::GetEngineId)
    .add_property("SystemId", &CDebugSystemObjects::CSystemObject::GetSystemId)
    ;

  class_<CDebugSystemObjects::CThread, bases<CDebugSystemObjects::CSystemObject> >("Thread", no_init)
    .def("__repr__", &CDebugSystemObjects::CThread::Repr)
    ;

  class_<CDebugSystemObjects::CCurrentThread, bases<CDebugSystemObjects::CThread> >("CurrentThreadClass", no_init)
    .add_property("DataOffset", &CDebugSystemObjects::CCurrentThread::GetDataOffset,
      "the location of the system data structure for the current thread.")
    .add_property("Handle", &CDebugSystemObjects::CCurrentThread::GetHandle,
      "the system handle for the current thread.")
    ;

  class_<CDebugSystemObjects::CProcess, bases<CDebugSystemObjects::CSystemObject> >("Process", no_init)
    .def("__repr__", &CDebugSystemObjects::CProcess::Repr)
    ;

  class_<CDebugSystemObjects::CCurrentProcess, bases<CDebugSystemObjects::CProcess> >("CurrentProcessClass", no_init)
    .add_property("DataOffset", &CDebugSystemObjects::CCurrentProcess::GetDataOffset,
      "the location of the system data structure for the current process.")
    .add_property("Handle", &CDebugSystemObjects::CCurrentProcess::GetHandle,
      "the system handle for the current process.")
    .add_property("ExecutableName", &CDebugSystemObjects::CCurrentProcess::GetExecutableName,
      "the name of executable file loaded in the current process.")
    .add_property("UpTime", &CDebugSystemObjects::CCurrentProcess::GetUpTime,
      "the length of time the current process has been running.")
    ;
}

ULONG64 CDebugSystemObjects::CCurrentThread::GetDataOffset(void) const
{
  ULONG64 offset;

  Check(m_intf->GetCurrentThreadDataOffset(&offset));

  return offset;
}
const CDebugDataSpaces::CHandle CDebugSystemObjects::CCurrentThread::GetHandle(void) const
{
  ULONG64 handle;

  Check(m_intf->GetCurrentThreadHandle(&handle));

  return CDebugDataSpaces::CHandle(m_intf, handle);
}

const ULONG64
CDebugSystemObjects::GetCurrentProcessPeb(void) const
{
  ULONG64 address;
  Check( m_intf->GetCurrentProcessPeb(&address) );
  return address;
}

const ULONG64
CDebugSystemObjects::GetCurrentThreadTeb(void) const
{
  ULONG64 address;
  Check( m_intf->GetCurrentThreadTeb(&address) );
  return address;
}

ULONG64 CDebugSystemObjects::CCurrentProcess::GetDataOffset(void) const
{
  ULONG64 offset;

  Check(m_intf->GetCurrentProcessDataOffset(&offset));

  return offset;
}
const CDebugDataSpaces::CHandle CDebugSystemObjects::CCurrentProcess::GetHandle(void) const
{
  ULONG64 handle;

  Check(m_intf->GetCurrentProcessHandle(&handle));

  return CDebugDataSpaces::CHandle(m_intf, handle);
}
const std::string CDebugSystemObjects::CCurrentProcess::GetExecutableName(void) const
{
  char szName[MAX_PATH];
  ULONG nName = _countof(szName);

  Check(m_intf->GetCurrentProcessExecutableName(szName, nName, &nName));

  return std::string(szName, nName-1);
}
const object CDebugSystemObjects::CCurrentProcess::GetUpTime(void) const
{
  CComQIPtr<IDebugSystemObjects2> intf(m_intf);

  ULONG uptime;

  Check(intf->GetCurrentProcessUpTime(&uptime));

  static const ULONG SECONDS_PER_DAY = 60 * 60 * 24;

  return object(handle<>(::PyDelta_FromDSU(uptime / SECONDS_PER_DAY, uptime % SECONDS_PER_DAY, 0)));
}

const CDebugSystemObjects::CDebugSystem CDebugSystemObjects::GetEventSystem(void) const
{
  CComQIPtr<IDebugSystemObjects3> intf(m_intf);

  ULONG id;

  Check(intf->GetEventSystem(&id));

  return CDebugSystem(this, id);
}

const CDebugSystemObjects::CProcess CDebugSystemObjects::GetEventProcess(void) const
{
  ULONG engId;

  Check(m_intf->GetEventProcess(&engId));

  return GetProcessByEngineId(engId);
}
const CDebugSystemObjects::CThread CDebugSystemObjects::GetEventThread(void) const
{
  ULONG engId;

  Check(m_intf->GetEventThread(&engId));

  return GetThreadByEngineId(engId);
}

const CDebugSystemObjects::CDebugSystem CDebugSystemObjects::GetCurrentSystem(void) const
{
  ULONG id = 0;

  CComQIPtr<IDebugSystemObjects3> intf(m_intf);

  Check(intf->GetCurrentSystemId(&id));

  return CDebugSystem(this, id);
}
void CDebugSystemObjects::SetCurrentSystem(const CDebugSystemObjects::CDebugSystem& system) const
{
  CComQIPtr<IDebugSystemObjects3> intf(m_intf);

  Check(intf->SetCurrentSystemId(system.GetId()));
}

const ULONG
CDebugSystemObjects::GetCurrentProcessId(void) const
{
    ULONG pid;
    Check( m_intf->GetCurrentProcessId(&pid) );
    return pid;
}
const ULONG
CDebugSystemObjects::GetCurrentThreadId(void) const
{
    ULONG pid;
    Check( m_intf->GetCurrentThreadId(&pid) );
    return pid;
}

const HRESULT
CDebugSystemObjects::SetCurrentProcessId(ULONG id) const
{
    HRESULT r;
    r = m_intf->SetCurrentProcessId(id);
    Check(r);
    return r;
}
const HRESULT
CDebugSystemObjects::SetCurrentThreadId(ULONG id) const
{
    HRESULT r;
    r = m_intf->SetCurrentThreadId(id);
    Check(r);
    return r;
}

const ULONG64
CDebugSystemObjects::GetCurrentProcessHandle(void) const
{
    ULONG64 handle;
    Check( m_intf->GetCurrentProcessHandle(&handle) );
    return handle;
}
const ULONG64
CDebugSystemObjects::GetCurrentThreadHandle(void) const
{
    ULONG64 handle;
    Check( m_intf->GetCurrentThreadHandle(&handle) );
    return handle;
}

const ULONG
CDebugSystemObjects::GetCurrentProcessSystemId(void) const
{
    ULONG result;
    Check( m_intf->GetCurrentProcessSystemId(&result) );
    return result;
}

const ULONG
CDebugSystemObjects::GetCurrentThreadSystemId(void) const
{
    ULONG result;
    Check( m_intf->GetCurrentThreadSystemId(&result) );
    return result;
}

const CDebugSystemObjects::CCurrentProcess CDebugSystemObjects::GetCurrentProcess(void) const
{
  ULONG engId;

  Check(m_intf->GetCurrentProcessId(&engId));

  return CCurrentProcess(this, engId, GetProcessByEngineId(engId).GetSystemId());
}
void CDebugSystemObjects::SetCurrentProcess(const CDebugSystemObjects::CProcess& process) const
{
  Check(m_intf->SetCurrentProcessId(process.GetEngineId()));
}
const CDebugSystemObjects::CCurrentThread CDebugSystemObjects::GetCurrentThread(void) const
{
  ULONG engId;

  Check(m_intf->GetCurrentThreadId(&engId));

  return CCurrentThread(this, engId, GetThreadByEngineId(engId).GetSystemId());
}
void CDebugSystemObjects::SetCurrentThread(const CDebugSystemObjects::CThread& thread) const
{
  Check(m_intf->SetCurrentThreadId(thread.GetEngineId()));
}

ULONG64 CDebugSystemObjects::GetImplicitProcessDataOffset(void) const
{
  CComQIPtr<IDebugSystemObjects2> intf(m_intf);

  ULONG64 offset;

  Check(intf->GetImplicitProcessDataOffset(&offset));

  return offset;
}
void CDebugSystemObjects::SetImplicitProcessDataOffset(ULONG64 offset) const
{
  CComQIPtr<IDebugSystemObjects2> intf(m_intf);

  Check(intf->SetImplicitProcessDataOffset(offset));
}
ULONG64 CDebugSystemObjects::GetImplicitThreadDataOffset(void) const
{
  CComQIPtr<IDebugSystemObjects2> intf(m_intf);

  ULONG64 offset;

  Check(intf->GetImplicitThreadDataOffset(&offset));

  return offset;
}
void CDebugSystemObjects::SetImplicitThreadDataOffset(ULONG64 offset) const
{
  CComQIPtr<IDebugSystemObjects2> intf(m_intf);

  Check(intf->SetImplicitThreadDataOffset(offset));
}

const list CDebugSystemObjects::GetSystems(void) const
{
  CComQIPtr<IDebugSystemObjects3> intf(m_intf);

  ULONG number = 0, id = 0;

  Check(intf->GetNumberSystems(&number));

  list systems;

  for (ULONG i=0; i<number; i++)
  {
    Check(intf->GetSystemIdsByIndex(i, 1, &id));

    CDebugSystem system(this, id);

    systems.append(system);
  }  

  return systems;
}
const list CDebugSystemObjects::GetThreads(void) const
{
  ULONG number = 0;

  Check(m_intf->GetNumberThreads(&number));

  std::vector<ULONG> engIds(number), sysIds(number);

  Check(m_intf->GetThreadIdsByIndex(0, number, &engIds[0], &sysIds[0]));

  list threads;

  for (ULONG i=0; i<number; i++)
  {
    threads.append(CThread(this, engIds[i], sysIds[i]));
  }

  return threads;
}
const list CDebugSystemObjects::GetProcesses(void) const
{
  ULONG number = 0;

  Check(m_intf->GetNumberProcesses(&number));

  std::vector<ULONG> engIds(number), sysIds(number);

  Check(m_intf->GetProcessIdsByIndex(0, number, &engIds[0], &sysIds[0]));

  list threads;

  for (ULONG i=0; i<number; i++)
  {
    threads.append(CProcess(this, engIds[i], sysIds[i]));
  }

  return threads;
}

const std::pair<ULONG, ULONG> CDebugSystemObjects::GetTotalNumberThreads(void) const
{
  ULONG total, largest;

  Check(m_intf->GetTotalNumberThreads(&total, &largest));

  return std::make_pair(total, largest);
}

const CDebugSystemObjects::CThread CDebugSystemObjects::GetThreadByEngineId(ULONG engineId) const
{
  ULONG number = 0;

  Check(m_intf->GetNumberThreads(&number));

  std::vector<ULONG> engIds(number), sysIds(number);

  Check(m_intf->GetThreadIdsByIndex(0, number, &engIds[0], &sysIds[0]));

  for (ULONG i=0; i<number; i++)  
    if (engIds[i] == engineId)
      return CThread(this, engIds[i], sysIds[i]);
  
  RaiseException("The thread engine id is not exists.", PyExc_IndexError);
}
const CDebugSystemObjects::CProcess CDebugSystemObjects::GetProcessByEngineId(ULONG engineId) const
{
  ULONG number = 0;

  Check(m_intf->GetNumberProcesses(&number));

  std::vector<ULONG> engIds(number), sysIds(number);

  Check(m_intf->GetProcessIdsByIndex(0, number, &engIds[0], &sysIds[0]));

  for (ULONG i=0; i<number; i++)  
    if (engIds[i] == engineId)
      return CProcess(this, engIds[i], sysIds[i]);

  RaiseException("The process engine id is not exists.", PyExc_IndexError);
}

const CDebugSystemObjects::CThread CDebugSystemObjects::GetThreadBySystemId(ULONG systemId) const
{
  ULONG engineId;

  Check(m_intf->GetThreadIdBySystemId(systemId, &engineId));

  return CThread(this, engineId, systemId);
}
const CDebugSystemObjects::CProcess CDebugSystemObjects::GetProcessBySystemId(ULONG systemId) const
{
  ULONG engineId;

  Check(m_intf->GetProcessIdBySystemId(systemId, &engineId));

  return CProcess(this, engineId, systemId);
}
const CDebugSystemObjects::CThread CDebugSystemObjects::GetThreadByTeb(ULONG64 offset) const
{
  ULONG engineId;
  Check(m_intf->GetThreadIdByTeb(offset, &engineId));
  return GetThreadByEngineId(engineId);
}
const CDebugSystemObjects::CProcess CDebugSystemObjects::GetProcessByPeb(ULONG64 offset) const
{
  ULONG engineId;
  Check(m_intf->GetProcessIdByPeb(offset, &engineId));
  return GetProcessByEngineId(engineId);
}
const ULONG
CDebugSystemObjects::GetProcessIdByHandle(ULONG64 handle) const
{
  ULONG pid;
  Check(m_intf->GetProcessIdByHandle(handle, &pid));
  return pid;
}
const ULONG
CDebugSystemObjects::GetProcessIdBySystemId(ULONG SysId) const
{
  ULONG pid;
  Check(m_intf->GetProcessIdBySystemId(SysId, &pid));
  return pid;
}
const ULONG
CDebugSystemObjects::GetThreadIdByHandle(ULONG64 handle) const
{
  ULONG tid;
  Check(m_intf->GetThreadIdByHandle(handle, &tid));
  return tid;
}
const ULONG
CDebugSystemObjects::GetThreadIdBySystemId(ULONG SysId) const
{
  ULONG tid;
  Check(m_intf->GetThreadIdBySystemId(SysId, &tid));
  return tid;
}
const CDebugSystemObjects::CThread CDebugSystemObjects::GetThreadByProcessor(ULONG processor) const
{
  ULONG engineId;

  Check(m_intf->GetThreadIdByProcessor(processor, &engineId));

  return GetThreadByEngineId(engineId);
}

///////
ULONG CDebugSystemObjects::GetNumberThreads(void) const
{
	ULONG result = 0;
	HRESULT r;
	Check(r = m_intf->GetNumberThreads(&result));

	return result;
}

const list CDebugSystemObjects::GetThreadIdsByIndex(ULONG start, ULONG count) const
{
	list tids; HRESULT r;
	PULONG d_ids(0); PULONG s_ids(0);
	CComQIPtr<IDebugSystemObjects> intf(m_intf);

	try {
		d_ids = new ULONG[count]; s_ids = new ULONG[count];
		Check(r = m_intf->GetThreadIdsByIndex(start, count, d_ids, s_ids));

		for (ULONG i = 0; i < count; i++) {
			tids.append( make_tuple( d_ids[i], s_ids[i] ) );
		}

	} catch (std::exception&) {
		if (d_ids)		// if bjarne knew what finally meant, i might not have to
			delete [] d_ids;
		if (s_ids)		// initialize and test these explicitly
			delete [] s_ids;
		throw;
	}
	delete [] d_ids;	
	delete [] s_ids;
	return tids;
}