#include "StdAfx.h"

#include "DebugClient.h"

#include <iostream>
#include <iomanip>

#include <boost/python.hpp>

void CDebugBreakpoint::Export(void)
{
  enum_<CDebugBreakpoint::BreakpointType>("BreakpointType")
    .value("CODE", CDebugBreakpoint::BREAKPOINT_CODE)
    .value("DATA", CDebugBreakpoint::BREAKPOINT_DATA)
    .value("TIME", CDebugBreakpoint::BREAKPOINT_TIME)
    ;

  enum_<CDebugBreakpoint::BreakpointFlag>("BreakpointFlag")
    .value("ENABLED",     CDebugBreakpoint::BREAKPOINT_ENABLED)
    .value("ADDER_ONLY",  CDebugBreakpoint::BREAKPOINT_ADDER_ONLY)
    .value("GO_ONLY",     CDebugBreakpoint::BREAKPOINT_GO_ONLY)
    .value("ONE_SHOT",    CDebugBreakpoint::BREAKPOINT_ONE_SHOT)
    .value("DEFERRED",    CDebugBreakpoint::BREAKPOINT_DEFERRED)
    ;

  enum_<CDebugBreakpoint::AccessType>("AccessType")
    .value("READ",        CDebugBreakpoint::BREAK_READ)
    .value("WRITE",       CDebugBreakpoint::BREAK_WRITE)
    .value("READ_WRITE",  CDebugBreakpoint::BREAK_READ_WRITE)
    .value("EXECUTE",     CDebugBreakpoint::BREAK_EXECUTE)
    .value("IO",          CDebugBreakpoint::BREAK_IO)
    ;

  class_<CDebugBreakpoint>("Breakpoint", no_init)
    .add_property("Id", &CDebugBreakpoint::GetId,
      "a engine's unique identifier for a breakpoint.")
    .add_property("Type", &CDebugBreakpoint::GetType,
      "the type of the breakpoint that a breakpoint is set for.")
    .add_property("Flags", &CDebugBreakpoint::GetFlags, &CDebugBreakpoint::SetFlags, "the flags for a breakpoint.")
    .def("AddFlags", &CDebugBreakpoint::AddFlags, "adds flags to a breakpoint.")
    .def("RemoveFlags", &CDebugBreakpoint::RemoveFlags, "removes flags from a breakpoint.")
    .add_property("Owner", &CDebugBreakpoint::GetOwner, "the client that owns the breakpoint.")
    .add_property("Offset", &CDebugBreakpoint::GetOffset, &CDebugBreakpoint::SetOffset,
      "the location that triggers a breakpoint.")
    .add_property("OffsetExpression", &CDebugBreakpoint::GetOffsetExpression, &CDebugBreakpoint::SetOffsetExpression,
      "the expression string that evaluates to the location that triggers a breakpoint.")
    .add_property("PassCount", &CDebugBreakpoint::GetPassCount, &CDebugBreakpoint::SetPassCount,
      "the number of times that the target was originally required to reach the breakpoint location before the breakpoint is triggered.")
    .add_property("CurrentPassCount", &CDebugBreakpoint::GetCurrentPassCount,
      "the remaining number of times that the target must reach the breakpoint location before the breakpoint is triggered.")
    .add_property("MatchThreadId", &CDebugBreakpoint::GetMatchThreadId, &CDebugBreakpoint::SetMatchThreadId,
      "the engine thread ID of the thread that can trigger a breakpoint.")
    .add_property("Command", &CDebugBreakpoint::GetCommand, &CDebugBreakpoint::SetCommand,
      "the command string that is executed when a breakpoint is triggered.")
    .add_property("DataParameters", &CDebugBreakpoint::GetDataParameters, &CDebugBreakpoint::SetDataParameters,
      "the parameters for a processor breakpoint.")

    .def("__repr__", &CDebugBreakpoint::Repr)
    .def("Enable", &CDebugBreakpoint::Enable, "enables a breakpoint")
    .def("Disable", &CDebugBreakpoint::Disable, "disable a breakpoint")
    .def("Remove", &CDebugBreakpoint::Remove, "disable a breakpoint")
    ;
}

ULONG CDebugBreakpoint::GetId(void) const
{
  ULONG id = 0;
  Check(m_intf->GetId(&id));
  return id;
}

const tuple
CDebugBreakpoint::GetType(void) const
{
  ULONG BreakType, ProcType;
  Check(m_intf->GetType(&BreakType, &ProcType));
  return make_tuple((BreakpointType) BreakType, (CDebugControl::ProcessorType) ProcType);
}

const CDebugClient&
CDebugBreakpoint::GetOwner(void) const
{
  CComPtr<IDebugClient> owner;
  Check(m_intf->GetAdder(&owner));
  return CDebugClient(owner);
}

const list CDebugBreakpoint::GetFlags(void) const
{
  ULONG flags;
  Check(m_intf->GetFlags(&flags));
  return utils::FlagsToList(static_cast<BreakpointFlag>(flags), BREAKPOINT_GO_ONLY, BREAKPOINT_ONE_SHOT);
}

void CDebugBreakpoint::SetFlags(list flags) const
{
  Check(m_intf->SetFlags(utils::FlagsFromList<BreakpointFlag>(flags)));
}

void CDebugBreakpoint::AddFlags(CDebugBreakpoint::BreakpointFlag flags) const
{
  Check(m_intf->AddFlags((ULONG) flags));
}

void CDebugBreakpoint::RemoveFlags(CDebugBreakpoint::BreakpointFlag flags) const
{
  Check(m_intf->RemoveFlags((ULONG) flags));
}

ULONG64 CDebugBreakpoint::GetOffset(void) const
{
  ULONG64 offset;
  Check(m_intf->GetOffset(&offset));
  return offset;
}

void CDebugBreakpoint::SetOffset(ULONG64 offset) const
{
  Check(m_intf->SetOffset(offset));
}

const std::string CDebugBreakpoint::GetOffsetExpression(void) const
{
  char szExpression[1024];
  ULONG nExpression = _countof(szExpression);
  Check(m_intf->GetOffsetExpression(szExpression, nExpression, &nExpression));
  return std::string(szExpression, nExpression-1);
}

void CDebugBreakpoint::SetOffsetExpression(const std::string& expression) const
{
  Check(m_intf->SetOffsetExpression(expression.c_str()));
}

ULONG CDebugBreakpoint::GetPassCount(void) const
{
  ULONG count;
  Check(m_intf->GetPassCount(&count));
  return count;
}

void CDebugBreakpoint::SetPassCount(ULONG count) const
{
  Check(m_intf->SetPassCount(count));
}

ULONG CDebugBreakpoint::GetCurrentPassCount(void) const
{
  ULONG count;
  Check(m_intf->GetCurrentPassCount(&count));
  return count;
}

ULONG CDebugBreakpoint::GetMatchThreadId(void) const
{
  ULONG id;
  Check(m_intf->GetMatchThreadId(&id));
  return id;
}

void CDebugBreakpoint::SetMatchThreadId(ULONG id) const
{
  Check(m_intf->SetMatchThreadId(id));
}

const std::string CDebugBreakpoint::GetCommand(void) const
{
  char szCommand[1024];
  ULONG nCommand = _countof(szCommand);
  Check(m_intf->GetCommand(szCommand, nCommand, &nCommand));
  return std::string(szCommand, nCommand-1);
}

void CDebugBreakpoint::SetCommand(const std::string& command) const
{
  Check(m_intf->SetCommand(command.c_str()));
}

const tuple CDebugBreakpoint::GetDataParameters(void) const
{
  ULONG size, type;
  Check(m_intf->GetDataParameters(&size, &type));
  return make_tuple(size, (AccessType) type);
}

void CDebugBreakpoint::SetDataParameters(tuple params) const
{
  Check(m_intf->SetDataParameters(extract<ULONG>(params[0]), (ULONG)extract<AccessType>(params[1])));
}

const object
CDebugBreakpoint::Repr(const CDebugBreakpoint& breakpoint)
{
  std::ostringstream oss;
  oss << "(Breakpoint #" << breakpoint.GetId() << " "
      << (breakpoint.GetFlags().count(BREAKPOINT_ENABLED) > 0 ? "enabled" : "disabled") << " ";

  static const char * BreakpointTypeNames[] = { "code", "data", "time" };
  BreakpointType type = extract<BreakpointType>(breakpoint.GetType()[0]);

  oss << BreakpointTypeNames[type] << " @ "
      << std::hex << std::setfill('0') << std::setw(8) << breakpoint.GetOffset();

  oss << " " << std::setfill('0') << std::setw(4) << breakpoint.GetCurrentPassCount()
      << "/" << std::setfill('0') << std::setw(4) << breakpoint.GetPassCount() << " ";

  try {
    CDebugClient client(breakpoint.GetOwner());
    tuple name = client.GetSymbols().GetNameByOffset(breakpoint.GetOffset());
    oss << extract<char*>(name[0]) << ":" << extract<ULONG64>(name[1]);
  } catch (...) {
      /*
         XXX: if we can't get a name, we ignore it since it's not
                really important (?)
      */
    ::PyErr_Clear();    // FIXME: we probably shouldn't be using python to fetch the name, heh...
  }
  oss << ")";
  return object(oss.str());
}

void CDebugBreakpoint::Enable(void)
{
    return this->AddFlags(CDebugBreakpoint::BREAKPOINT_ENABLED);
}

void CDebugBreakpoint::Disable(void)
{
    return this->RemoveFlags(CDebugBreakpoint::BREAKPOINT_ENABLED);
}

HRESULT CDebugBreakpoint::Remove(void)
{
    return m_owner->RemoveBreakpoint(m_intf.Detach());
}
