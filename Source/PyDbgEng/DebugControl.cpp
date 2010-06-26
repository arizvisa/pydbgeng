#include "StdAfx.h"
#include "DebugControl.h"

#include <sstream>
#include <iomanip>

#include <datetime.h>

#include "DebugClient.h"

void CDebugControl::Export(void)
{
  if (::Py_IsInitialized() && !PyDateTimeAPI) PyDateTime_IMPORT;

  enum_<CDebugControl::EngineOption>("EngineOption")
    .value("IGNORE_DBGHELP_VERSION",      CDebugControl::ENGOPT_IGNORE_DBGHELP_VERSION)
    .value("IGNORE_EXTENSION_VERSIONS",   CDebugControl::ENGOPT_IGNORE_EXTENSION_VERSIONS)
    .value("ALLOW_NETWORK_PATHS",         CDebugControl::ENGOPT_ALLOW_NETWORK_PATHS)
    .value("DISALLOW_NETWORK_PATHS",      CDebugControl::ENGOPT_DISALLOW_NETWORK_PATHS)
    .value("NETWORK_PATHS",               CDebugControl::ENGOPT_NETWORK_PATHS)
    .value("IGNORE_LOADER_EXCEPTIONS",    CDebugControl::ENGOPT_IGNORE_LOADER_EXCEPTIONS)
    .value("INITIAL_BREAK",               CDebugControl::ENGOPT_INITIAL_BREAK)
    .value("INITIAL_MODULE_BREAK",        CDebugControl::ENGOPT_INITIAL_MODULE_BREAK)
    .value("FINAL_BREAK",                 CDebugControl::ENGOPT_FINAL_BREAK)
    .value("NO_EXECUTE_REPEAT",           CDebugControl::ENGOPT_NO_EXECUTE_REPEAT)
    .value("FAIL_INCOMPLETE_INFORMATION", CDebugControl::ENGOPT_FAIL_INCOMPLETE_INFORMATION)
    .value("ALLOW_READ_ONLY_BREAKPOINTS", CDebugControl::ENGOPT_ALLOW_READ_ONLY_BREAKPOINTS)
    .value("SYNCHRONIZE_BREAKPOINTS",     CDebugControl::ENGOPT_SYNCHRONIZE_BREAKPOINTS)
    .value("DISALLOW_SHELL_COMMANDS",     CDebugControl::ENGOPT_DISALLOW_SHELL_COMMANDS)
    .value("KD_QUIET_MODE",               CDebugControl::ENGOPT_KD_QUIET_MODE)
    .value("DISABLE_MANAGED_SUPPORT",     CDebugControl::ENGOPT_DISABLE_MANAGED_SUPPORT)
    .value("DISABLE_MODULE_SYMBOL_LOAD",  CDebugControl::ENGOPT_DISABLE_MODULE_SYMBOL_LOAD)
    .value("DISABLE_EXECUTION_COMMANDS",  CDebugControl::ENGOPT_DISABLE_EXECUTION_COMMANDS)
    .value("DISALLOW_IMAGE_FILE_MAPPING", CDebugControl::ENGOPT_DISALLOW_IMAGE_FILE_MAPPING) 
    .value("PREFER_DML",                  CDebugControl::ENGOPT_PREFER_DML)
    ;

  enum_<CDebugControl::ExecutionStatus>("ExecutionStatus")
    .value("NO_CHANGE",         CDebugControl::STATUS_NO_CHANGE)
    .value("GO",                CDebugControl::STATUS_GO)
    .value("GO_HANDLED",        CDebugControl::STATUS_GO_HANDLED)
    .value("GO_NOT_HANDLED",    CDebugControl::STATUS_GO_NOT_HANDLED)
    .value("STEP_OVER",         CDebugControl::STATUS_STEP_OVER)
    .value("STEP_INTO",         CDebugControl::STATUS_STEP_INTO)
    .value("BREAK",             CDebugControl::STATUS_BREAK)
    .value("NO_DEBUGGEE",       CDebugControl::STATUS_NO_DEBUGGEE)
    .value("STEP_BRANCH",       CDebugControl::STATUS_STEP_BRANCH)
    .value("IGNORE_EVENT",      CDebugControl::STATUS_IGNORE_EVENT)
    .value("RESTART_REQUESTED", CDebugControl::STATUS_RESTART_REQUESTED)
    ;

  enum_<CDebugControl::DebuggeeClass>("DebuggeeClass")
    .value("UNINITIALIZED", CDebugControl::CLASS_UNINITIALIZED)
    .value("KERNEL",        CDebugControl::CLASS_KERNEL)
    .value("USER",          CDebugControl::CLASS_USER)
    ;

  enum_<CDebugControl::CodeLevel>("CodeLevel")
    .value("SOURCE",    CDebugControl::LEVEL_SOURCE)
    .value("ASSEMBLY",  CDebugControl::LEVEL_ASSEMBLY)
    ;

  enum_<CDebugControl::ErrorLevel>("ErrorLevel")
    .value("ERROR",       CDebugControl::LEVEL_ERROR)
    .value("MINORERROR",  CDebugControl::LEVEL_MINORERROR)
    .value("WARNING",     CDebugControl::LEVEL_WARNING)
    ;

  enum_<CDebugControl::NumberRadix>("NumberRadix")
    .value("OCT", CDebugControl::RADIX_OCT)
    .value("DEC", CDebugControl::RADIX_DEC)
    .value("HEX", CDebugControl::RADIX_HEX)
    ;

  enum_<CDebugControl::ExecuteFlag>("ExecuteFlag")
    .value("DEFAULT",     CDebugControl::EXECUTE_DEFAULT)
    .value("ECHO",        CDebugControl::EXECUTE_ECHO)
    .value("NOT_LOGGED",  CDebugControl::EXECUTE_NOT_LOGGED)
    .value("NO_REPEAT",   CDebugControl::EXECUTE_NO_REPEAT)
    ;

  enum_<CDebugControl::ExpressionSyntax>("ExpressionSyntax")
    .value("MASM",      CDebugControl::EXPR_MASM)
    .value("CPP",       CDebugControl::EXPR_CPLUSPLUS)
    ;

  enum_<CDebugControl::AssemblyOption>("AssemblyOption")
    .value("DEFAULT",             CDebugControl::ASMOPT_DEFAULT)
    .value("VERBOSE",             CDebugControl::ASMOPT_VERBOSE)
    .value("NO_CODE_BYTES",       CDebugControl::ASMOPT_NO_CODE_BYTES)
    .value("IGNORE_OUTPUT_WIDTH", CDebugControl::ASMOPT_IGNORE_OUTPUT_WIDTH)
    .value("SOURCE_LINE_NUMBER",  CDebugControl::ASMOPT_SOURCE_LINE_NUMBER)
    ;

  enum_<CDebugControl::DisassembleFlag>("DisassembleFlag")
    .value("DEFAULT",             CDebugControl::DISASM_DEFAULT) 
    .value("EFFECTIVE_ADDRESS",   CDebugControl::DISASM_EFFECTIVE_ADDRESS)
    .value("MATCHING_SYMBOLS",    CDebugControl::DISASM_MATCHING_SYMBOLS)
    .value("SOURCE_LINE_NUMBER",  CDebugControl::DISASM_SOURCE_LINE_NUMBER)
    .value("SOURCE_FILE_NAME",    CDebugControl::DISASM_SOURCE_FILE_NAME)
    ;

  enum_<CDebugControl::InterruptType>("InterruptType")
    .value("ACTIVE",  CDebugControl::INTR_ACTIVE)
    .value("PASSIVE", CDebugControl::INTR_PASSIVE)
    .value("EXIT",    CDebugControl::INTR_EXIT)
    ;

  enum_<CDebugControl::PlatformId>("PlatformType")
    .value("WIN32S",  CDebugControl::ID_WIN32S)
    .value("WINDOWS", CDebugControl::ID_WINDOWS)
    .value("NT",      CDebugControl::ID_NT)
    ;

  enum_<CDebugControl::ProcessorType>("ProcessorType")
    .value("I386",  CDebugControl::TYPE_I386)
    .value("ARM",   CDebugControl::TYPE_ARM)
    .value("IA64",  CDebugControl::TYPE_IA64)
    .value("AMD64", CDebugControl::TYPE_AMD64)
    .value("ALPHA", CDebugControl::TYPE_ALPHA)
    .value("EBC",   CDebugControl::TYPE_EBC)
    ;

  enum_<CDebugControl::BreakStatus>("BreakStatus")    
    .value("BREAK",               CDebugControl::FILTER_BREAK)
    .value("SECOND_CHANCE_BREAK", CDebugControl::FILTER_SECOND_CHANCE_BREAK)
    .value("OUTPUT",              CDebugControl::FILTER_OUTPUT)
    .value("IGNORE",              CDebugControl::FILTER_IGNORE)
    .value("REMOVE",              CDebugControl::FILTER_REMOVE)
    ;

  enum_<CDebugControl::BreakpointType>("BreakpointType")
    .value("CODE", CDebugControl::BREAKPOINT_CODE)
    .value("DATA", CDebugControl::BREAKPOINT_DATA)
    .value("TIME", CDebugControl::BREAKPOINT_TIME)
    ;

  enum_<CDebugControl::BreakpointFlag>("BreakpointFlag")
    .value("ENABLED",     CDebugControl::BREAKPOINT_ENABLED)
    .value("ADDER_ONLY",  CDebugControl::BREAKPOINT_ADDER_ONLY)
    .value("GO_ONLY",     CDebugControl::BREAKPOINT_GO_ONLY)
    .value("ONE_SHOT",    CDebugControl::BREAKPOINT_ONE_SHOT)
    .value("DEFERRED",    CDebugControl::BREAKPOINT_DEFERRED)
    ;

  enum_<CDebugControl::AccessType>("AccessType")
    .value("READ",        CDebugControl::BREAK_READ)
    .value("WRITE",       CDebugControl::BREAK_WRITE)
    .value("READ_WRITE",  CDebugControl::BREAK_READ_WRITE)
    .value("EXECUTE",     CDebugControl::BREAK_EXECUTE)
    .value("IO",          CDebugControl::BREAK_IO)
    ;

  scope DebugControl = class_<CDebugControl>("DebugControl", no_init)
    .add_property("PageSize", &CDebugControl::GetPageSize,
      "the page size for the effective processor mode.")
    .add_property("IsPointer64Bit", &CDebugControl::IsPointer64Bit,
      "the effective processor uses 64-bit pointers.")

    .add_property("VersionInformation", &CDebugControl::GetVersionInformation)
    .def("OutputVersionInformation", &CDebugControl::OutputVersionInformation, 
      (arg("target")=TARGET_ALL_CLIENTS),
      "prints version information about the debugger engine to the debugger console.")

    .add_property("EngineOptions", &CDebugControl::GetEngineOptions, &CDebugControl::SetEngineOptions,
      "the engine's options.")
    .def("AddEngineOption", &CDebugControl::AddEngineOption,
      "turns on some of the debugger engine's options.")
    .def("RemoveEngineOption", &CDebugControl::RemoveEngineOption,
      "turns off some of the engine's options.")

    .add_property("ExecutionStatus", &CDebugControl::GetExecutionStatus, &CDebugControl::SetExecutionStatus,
      "information about the execution status of the debugger engine.")
    .add_property("DebuggeeType", &CDebugControl::GetDebuggeeType, "the nature of the current target.")
    .add_property("CodeLevel", &CDebugControl::GetCodeLevel, &CDebugControl::SetCodeLevel,
      "the current code level and is mainly used when stepping through code.")
    .add_property("Radix", &CDebugControl::GetRadix, &CDebugControl::SetRadix, 
      "the default radix (number base) used by the debugger engine when it evaluates and displays MASM expressions, and when it displays symbol information.")
    .add_property("OutputLevel", &CDebugControl::GetSystemOutputLevel, &CDebugControl::SetSystemOutputLevel,
      "the level at which system errors are printed to the engine's output.")
    .add_property("BreakLevel", &CDebugControl::GetSystemBreakLevel, &CDebugControl::SetSystemBreakLevel,
      "the level at which system errors break into the debugger.")

    .add_property("EventFilters", &CDebugControl::GetEventFilters,
      "the event filters currently used by the engine.")
    .add_property("ExceptionFilters", &CDebugControl::GetExceptionFilters,
      "the exceptions that can be controlled using the specific exception filters.")

    .add_property("Breakpoints", &CDebugControl::GetBreakpoints,
      "breakpoints for the current process.")
    .def("AddBreakpoint", &CDebugControl::AddBreakpoint, 
      (arg("type")=BREAKPOINT_CODE, arg("id")=DEBUG_ANY_ID), 
      "create a new breakpoint for the current target.")

    .add_property("SupportedProcessorTypes", &CDebugControl::GetSupportedProcessorTypes,
      "the processor types supported by the debugger engine. ")
    .add_property("PossibleExecutingProcessorTypes", &CDebugControl::GetPossibleExecutingProcessorTypes, 
      "the processor types that are supported by the computer running the current target.")

    .add_property("ActualProcessorType", &CDebugControl::GetActualProcessorType, 
      "the processor type of the physical processor of the computer that is running the target.")
    .add_property("EffectiveProcessorType", &CDebugControl::GetEffectiveProcessorType, &CDebugControl::SetEffectiveProcessorType,
      "the effective processor type of the processor of the computer that is running the target.")
    .add_property("ExecutingProcessorType", &CDebugControl::GetExecutingProcessorType,
      "the executing processor type for the processor for which the last event occurred.")

    .add_property("CurrentTimeDate", &CDebugControl::GetCurrentTimeDate, 
      "the time of the current target.")
    .add_property("CurrentSystemUpTime", &CDebugControl::GetCurrentSystemUpTime, 
      "the current target's computer has been running since it was last started.")

    .add_property("Events", &CDebugControl::GetEvents, "the events for the current target")
    .add_property("CurrentEvent", &CDebugControl::GetCurrentEvent, 
      "the current event within the current list of events for the current target, if such a list exists.")

    .def("WaitForEvent", &CDebugControl::WaitForEvent, 
      (args("timeout")=INFINITE), 
      "waits for an event that breaks into the debugger engine application.")

    .add_property("HasInterrupt", &CDebugControl::HasInterrupt,
      "whether a user interrupt was issued.")
    .def("SetInterrupt", &CDebugControl::SetInterrupt, (arg("type")=INTR_ACTIVE),
      "registers a user interrupt or breaks into the debugger.")
    .add_property("InterruptTimeout", &CDebugControl::GetInterruptTimeout, &CDebugControl::SetInterruptTimeout,
      "the number of seconds that the engine will wait when requesting a break into the debugger.")

    .add_property("ExpressionSyntax", &CDebugControl::GetExpressionSyntax, &CDebugControl::SetExpressionSyntax,
      "the current syntax that the engine is using for evaluating expressions.")
    .add_property("ExpressionSyntaxNames", &CDebugControl::GetExpressionSyntaxNames,
      "the full and abbreviated names of supported expression syntax.")
    .def("SetExpressionSyntaxByName", &CDebugControl::SetExpressionSyntaxByName,
      "set the syntax that the engine will use to evaluate expressions.")

    .def("Evaluate", &CDebugControl::Evaluate, ("expression", arg("type")=DEBUG_VALUE_INVALID),
      "evaluate an expression, returning the result.")
    .def("Execute", &CDebugControl::Execute, ("command", arg("flags")=EXECUTE_DEFAULT, arg("target")=TARGET_ALL_CLIENTS),
      "execute the specified debugger commands.")
    .def("ExecuteCommandFile", &CDebugControl::ExecuteCommandFile, 
      ("commandFile", arg("flags")=EXECUTE_DEFAULT, arg("target")=TARGET_ALL_CLIENTS),
      "open the specified file and execute the debugger commands that are contained within.")

    .add_property("AssemblyOptions", &CDebugControl::GetAssemblyOptions, &CDebugControl::SetAssemblyOptions,
      "the assembly and disassembly options that affect how the debugger engine assembles and disassembles processor instructions for the target.")
    .def("AddAssemblyOption", &CDebugControl::AddAssemblyOption,
      "turns on some of the assembly and disassembly options.")
    .def("RemoveAssemblyOption", &CDebugControl::RemoveAssemblyOption,
      "turns off some of the assembly and disassembly options.")

    .def("Assemble", &CDebugControl::Assemble, ("offset", "instr"),
      "assemble a single processor instruction. The assembled instruction is placed in the target's memory.")
    .def("Disassemble", &CDebugControl::Disassemble, ("offset", arg("flags")=DISASM_DEFAULT, arg("bufSize")=4096), 
      "disassemble a processor instruction in the target's memory.")
    .def("OutputDisassembly", &CDebugControl::OutputDisassembly, 
      ("offset", arg("flags")=DISASM_DEFAULT, arg("target")=TARGET_ALL_CLIENTS),
      "disassembles a processor instruction and sends the disassembly to the output callbacks.")
    .add_property("DisassembleEffectiveOffset", &CDebugControl::GetDisassembleEffectiveOffset,
      "the address of the last instruction disassembled using Disassemble.")
    .def("GetNearInstruction", &CDebugControl::GetNearInstruction, ("offset", arg("delta")),
      "the location of a processor instruction relative to a given location.")
    .add_property("ReturnOffset", &CDebugControl::GetReturnOffset,
      "the return address for the current function.")

    .def("Input", &CDebugControl::Input, 
      "quest an input string from the debugger engine.")
    .def("ReturnInput", &CDebugControl::ReturnInput, 
      "send an input string to the engine following a request for input.")
    .def("Output", &CDebugControl::Output, 
      ("text", arg("mask")=MASK_NORMAL, arg("target")=TARGET_ALL_CLIENTS),
      "send the text to output callbacks that were registered with some of the engine's clients.")

    .add_property("TextMacros", &CDebugControl::GetTextMacros,
      "the value of a fixed-name alias.")
    .def("SetTextMacro", &CDebugControl::SetTextMacroByName,
      "set the value of a fixed-name alias.")
    .def("SetTextMacro", &CDebugControl::SetTextMacroBySlot,
      "set the value of a fixed-name alias.")

    .def("GetStackFrames", &CDebugControl::GetStackFrames, 
      (arg("frameOffset")=0, arg("stackOffset")=0, arg("InstructionOffset")=0),
      "the frames at the top of the specified call stack.")
    ;

  class_<CDebugControl::CProcessor>("Processor", no_init)
    .add_property("Type", &CDebugControl::CProcessor::GetType)
    .add_property("Name", &CDebugControl::CProcessor::GetName)
    .add_property("Abbrev", &CDebugControl::CProcessor::GetAbbrev)

    .def("__repr__", &CDebugControl::CProcessor::Repr)
    ;

  class_<CDebugControl::CEventFilter>("EventFilter", no_init)
    .add_property("Index", &CDebugControl::CEventFilter::GetIndex)
    .add_property("Text", &CDebugControl::CEventFilter::GetText)
    .add_property("Status", 
      &CDebugControl::CEventFilter::GetStatus, 
      &CDebugControl::CEventFilter::SetStatus)
    .add_property("Command", 
      &CDebugControl::CEventFilter::GetCommand, 
      &CDebugControl::CEventFilter::SetCommand)

    .def("__repr__", &CDebugControl::CEventFilter::Repr)
    ;

  class_<CDebugControl::CExceptionFilter, bases<CDebugControl::CEventFilter> >("ExceptionFilter", no_init)
    .add_property("Handled", 
      &CDebugControl::CExceptionFilter::IsHandled, 
      &CDebugControl::CExceptionFilter::SetHandled)
    .add_property("SecondCommand", 
      &CDebugControl::CExceptionFilter::GetSecondCommand, 
      &CDebugControl::CExceptionFilter::SetSecondCommand)

    .def("__repr__", &CDebugControl::CExceptionFilter::Repr)
    ;

  class_<CDebugControl::CBreakpoint>("Breakpoint", no_init)
    .add_property("Id", &CDebugControl::CBreakpoint::GetId, 
      "a engine's unique identifier for a breakpoint.")
    .add_property("Type", &CDebugControl::CBreakpoint::GetType, 
      "the type of the breakpoint that a breakpoint is set for.")
    .add_property("Flags", &CDebugControl::CBreakpoint::GetFlags, &CDebugControl::CBreakpoint::SetFlags, "the flags for a breakpoint.")
    .def("AddFlags", &CDebugControl::CBreakpoint::AddFlags, "adds flags to a breakpoint.")
    .def("RemoveFlags", &CDebugControl::CBreakpoint::RemoveFlags, "removes flags from a breakpoint.")
    .add_property("Owner", &CDebugControl::CBreakpoint::GetOwer, "the client that owns the breakpoint.")
    .add_property("Offset", &CDebugControl::CBreakpoint::GetOffset, &CDebugControl::CBreakpoint::SetOffset,
      "the location that triggers a breakpoint.")
    .add_property("OffsetExpression", &CDebugControl::CBreakpoint::GetOffsetExpression, &CDebugControl::CBreakpoint::SetOffsetExpression,
      "the expression string that evaluates to the location that triggers a breakpoint.")
    .add_property("PassCount", &CDebugControl::CBreakpoint::GetPassCount, &CDebugControl::CBreakpoint::SetPassCount,
      "the number of times that the target was originally required to reach the breakpoint location before the breakpoint is triggered.")
    .add_property("CurrentPassCount", &CDebugControl::CBreakpoint::GetCurrentPassCount,
      "the remaining number of times that the target must reach the breakpoint location before the breakpoint is triggered.")
    .add_property("MatchThreadId", &CDebugControl::CBreakpoint::GetMatchThreadId, &CDebugControl::CBreakpoint::SetMatchThreadId,
      "the engine thread ID of the thread that can trigger a breakpoint.")
    .add_property("Command", &CDebugControl::CBreakpoint::GetCommand, &CDebugControl::CBreakpoint::SetCommand,
      "the command string that is executed when a breakpoint is triggered.")
    .add_property("DataParameters", &CDebugControl::CBreakpoint::GetDataParameters, &CDebugControl::CBreakpoint::SetDataParameters,
      "the parameters for a processor breakpoint.")

    .def("__repr__", &CDebugControl::CBreakpoint::Repr)
    ;

  class_<CDebugControl::CEvent>("Event", no_init)
    .add_property("Index", &CDebugControl::CEvent::GetIndex)
    .add_property("Name", &CDebugControl::CEvent::GetName)

    .def("__repr__", &CDebugControl::CEvent::Repr)
    ;

  class_<CDebugControl::CStackFrame>("StackFrame", no_init)
    .add_property("InstructionOffset", &CDebugControl::CStackFrame::GetInstructionOffset,
      "The location in the process's virtual address space of the related instruction for the stack frame.")
    .add_property("ReturnOffset", &CDebugControl::CStackFrame::GetReturnOffset,
      "The location in the process's virtual address space of the return address for the stack frame.")
    .add_property("FrameOffset", &CDebugControl::CStackFrame::GetFrameOffset,
      "The location in the process's virtual address space of the stack frame, if known.")
    .add_property("StackOffset", &CDebugControl::CStackFrame::GetStackOffset,
      "The location in the process's virtual address space of the processor stack.")
    .add_property("FuncTableEntry", &CDebugControl::CStackFrame::GetFuncTableEntry,
      "The location in the target's virtual address space of the function entry for this frame, if available.")
    .add_property("Params", &CDebugControl::CStackFrame::GetParams,
      "The values of the first four stack slots that are passed to the function, if available.")
    .add_property("Virtual", &CDebugControl::CStackFrame::IsVirtual, 
      "his stack frame was generated by the debugger by unwinding.")
    .add_property("FrameNumber", &CDebugControl::CStackFrame::GetFrameNumber, 
      "The index of the frame. This index counts the number of frames from the top of the call stack. ")
    ;
}

const std::pair<const std::string, const std::string> CDebugControl::CProcessor::GetNames(void) const
{
  char szName[256], szAbbrev[256];
  ULONG ulName = _countof(szName), ulAbbrev = _countof(szAbbrev);

  Check(m_intf->GetProcessorTypeNames((ULONG) m_type, szName, ulName, &ulName, szAbbrev, ulAbbrev, &ulAbbrev));

  return std::make_pair(std::string(szName, ulName-1), std::string(szAbbrev, ulAbbrev-1));
}

const std::string CDebugControl::CEventFilter::GetCommand(void) const
{
  char buf[1024];
  ULONG size = _countof(buf);

  Check(m_intf->GetEventFilterCommand(m_idx, buf, size, &size));

  return std::string(buf, size-1);
}

void CDebugControl::CEventFilter::SetCommand(const std::string& command) const
{
  Check(m_intf->SetEventFilterCommand(m_idx, command.c_str()));
}

const std::string CDebugControl::CEventFilter::GetArgument(void) const
{
  char buf[1024];
  ULONG size = _countof(buf);

  Check(m_intf->GetSpecificFilterArgument(m_idx, buf, size, &size));

  return std::string(buf, size-1);
}

void CDebugControl::CEventFilter::SetArgument(const std::string& argument) const
{
  Check(m_intf->SetSpecificFilterArgument(m_idx, argument.c_str()));
}

CDebugControl::BreakStatus CDebugControl::CEventFilter::GetStatus(void) const
{
  DEBUG_SPECIFIC_FILTER_PARAMETERS param;

  Check(m_intf->GetSpecificFilterParameters(m_idx, 1, &param));

  return (CDebugControl::BreakStatus) param.ExecutionOption;
}
void CDebugControl::CEventFilter::SetStatus(CDebugControl::BreakStatus status) const
{
  DEBUG_SPECIFIC_FILTER_PARAMETERS param;

  Check(m_intf->GetSpecificFilterParameters(m_idx, 1, &param));

  param.ExecutionOption = status;

  Check(m_intf->SetSpecificFilterParameters(m_idx, 1, &param));
}

const std::string CDebugControl::CExceptionFilter::GetSecondCommand(void) const
{
  char buf[1024];
  ULONG size = _countof(buf);

  Check(m_intf->GetExceptionFilterSecondCommand(m_idx, buf, size, &size));

  return std::string(buf, size-1);
}

void CDebugControl::CExceptionFilter::SetSecondCommand(const std::string& command) const
{
  Check(m_intf->SetExceptionFilterSecondCommand(m_idx, command.c_str()));
}

CDebugControl::BreakStatus CDebugControl::CExceptionFilter::GetStatus(void) const
{
  DEBUG_EXCEPTION_FILTER_PARAMETERS param;

  Check(m_intf->GetExceptionFilterParameters(1, NULL, m_idx, &param));

  return (BreakStatus) param.ExecutionOption;
}
void CDebugControl::CExceptionFilter::SetStatus(BreakStatus status) const
{
  DEBUG_EXCEPTION_FILTER_PARAMETERS param;

  Check(m_intf->GetExceptionFilterParameters(1, NULL, m_idx, &param));

  param.ExecutionOption = status;

  Check(m_intf->SetExceptionFilterParameters(1, &param));
}

bool CDebugControl::CExceptionFilter::IsHandled(void) const
{
  DEBUG_EXCEPTION_FILTER_PARAMETERS param;

  Check(m_intf->GetExceptionFilterParameters(1, NULL, m_idx, &param));

  return DEBUG_FILTER_GO_HANDLED == param.ContinueOption;
}
void CDebugControl::CExceptionFilter::SetHandled(bool handled) const
{
  DEBUG_EXCEPTION_FILTER_PARAMETERS param;

  Check(m_intf->GetExceptionFilterParameters(1, NULL, m_idx, &param));

  param.ContinueOption = handled ? DEBUG_FILTER_GO_HANDLED : DEBUG_FILTER_GO_NOT_HANDLED;

  Check(m_intf->SetExceptionFilterParameters(1, &param));
}

void CDebugControl::CBreakpoint::Remove(void) 
{
  Check(m_intf->RemoveBreakpoint(m_bp));
}

ULONG CDebugControl::CBreakpoint::GetId(void) const
{
  ULONG id = 0;

  Check(m_bp->GetId(&id));

  return id;
}
const tuple CDebugControl::CBreakpoint::GetType(void) const
{
  ULONG BreakType, ProcType;

  Check(m_bp->GetType(&BreakType, &ProcType));

  return make_tuple((BreakpointType) BreakType, (ProcessorType) ProcType);
}
const CDebugClient CDebugControl::CBreakpoint::GetOwer(void) const
{
  CComPtr<IDebugClient> owner;

  Check(m_bp->GetAdder(&owner));

  return CDebugClient(owner);
}
const list CDebugControl::CBreakpoint::GetFlags(void) const
{
  ULONG flags;

  Check(m_bp->GetFlags(&flags));

  return FlagsToList(static_cast<BreakpointFlag>(flags), BREAKPOINT_GO_ONLY, BREAKPOINT_ONE_SHOT);
}
void CDebugControl::CBreakpoint::SetFlags(list flags) const
{
  Check(m_bp->SetFlags(FlagsFromList<BreakpointFlag>(flags)));
}
void CDebugControl::CBreakpoint::AddFlags(CDebugControl::BreakpointFlag flags) const
{
  Check(m_bp->AddFlags((ULONG) flags));
}
void CDebugControl::CBreakpoint::RemoveFlags(CDebugControl::BreakpointFlag flags) const
{
  Check(m_bp->RemoveFlags((ULONG) flags));
}
ULONG64 CDebugControl::CBreakpoint::GetOffset(void) const
{
  ULONG64 offset;

  Check(m_bp->GetOffset(&offset));

  return offset;
}
void CDebugControl::CBreakpoint::SetOffset(ULONG64 offset) const
{
  Check(m_bp->SetOffset(offset));
}
const std::string CDebugControl::CBreakpoint::GetOffsetExpression(void) const
{
  char szExpression[1024];
  ULONG nExpression = _countof(szExpression);

  Check(m_bp->GetOffsetExpression(szExpression, nExpression, &nExpression));

  return std::string(szExpression, nExpression-1);
}
void CDebugControl::CBreakpoint::SetOffsetExpression(const std::string& expression) const
{
  Check(m_bp->SetOffsetExpression(expression.c_str()));
}

ULONG CDebugControl::CBreakpoint::GetPassCount(void) const
{
  ULONG count;

  Check(m_bp->GetPassCount(&count));

  return count;
}
void CDebugControl::CBreakpoint::SetPassCount(ULONG count) const
{
  Check(m_bp->SetPassCount(count));
}
ULONG CDebugControl::CBreakpoint::GetCurrentPassCount(void) const
{
  ULONG count;

  Check(m_bp->GetCurrentPassCount(&count));

  return count;
}

ULONG CDebugControl::CBreakpoint::GetMatchThreadId(void) const
{
  ULONG id;

  Check(m_bp->GetMatchThreadId(&id));

  return id;
}
void CDebugControl::CBreakpoint::SetMatchThreadId(ULONG id) const
{
  Check(m_bp->SetMatchThreadId(id));
}

const std::string CDebugControl::CBreakpoint::GetCommand(void) const
{
  char szCommand[1024];
  ULONG nCommand = _countof(szCommand);

  Check(m_bp->GetCommand(szCommand, nCommand, &nCommand));

  return std::string(szCommand, nCommand-1);
}
void CDebugControl::CBreakpoint::SetCommand(const std::string& command) const
{
  Check(m_bp->SetCommand(command.c_str()));
}

const tuple CDebugControl::CBreakpoint::GetDataParameters(void) const
{
  ULONG size, type;

  Check(m_bp->GetDataParameters(&size, &type));

  return make_tuple(size, (AccessType) type);
}
void CDebugControl::CBreakpoint::SetDataParameters(tuple params) const
{
  Check(m_bp->SetDataParameters(extract<ULONG>(params[0]), (ULONG)extract<AccessType>(params[1])));
}

const object CDebugControl::CBreakpoint::Repr(const CBreakpoint& bp)
{
  std::ostringstream oss;

  oss << "(Breakpoint #" << bp.GetId() << " "
      << (bp.GetFlags().count(BREAKPOINT_ENABLED) > 0 ? "enabled" : "disabled") << " ";

  static const char * BreakpointTypeNames[] = { "code", "data", "time" };

  BreakpointType type = extract<BreakpointType>(bp.GetType()[0]);

  oss << BreakpointTypeNames[type] << " @ "
      << std::hex << std::setfill('0') << std::setw(8) << bp.GetOffset();

  oss << " " << std::setfill('0') << std::setw(4) << bp.GetCurrentPassCount()
      << "/" << std::setfill('0') << std::setw(4) << bp.GetPassCount() << " ";

  tuple name = bp.GetOwer().GetSymbols().GetNameByOffset(bp.GetOffset());

  oss << extract<char *>(name[0]) << ":" << extract<ULONG64>(name[1]);

  oss << ")";

  return object(oss.str());
}

const std::string CDebugControl::CEvent::GetName(void) const
{
  char szName[256];
  ULONG nName = _countof(szName);

  Check(CComQIPtr<IDebugControl3>(m_intf)->GetEventIndexDescription(
    m_idx, DEBUG_EINDEX_NAME, szName, nName, &nName));

  return std::string(szName, nName-1);
}

ULONG CDebugControl::GetPageSize(void) const
{
  ULONG size = 0;

  Check(m_intf->GetPageSize(&size));

  return size;
}

bool CDebugControl::IsPointer64Bit(void) const
{
  return S_OK == m_intf->IsPointer64Bit();
}

const dict CDebugControl::GetVersionInformation(void) const
{
  ULONG ulPlatformId, ulOsMajor, ulOsMinor, ulServicePackNumber, ulKdMajor, ulKdMinor;  
  std::string sServicePack, sBuild;

  char szServicePack[256], szBuild[256];
  ULONG ulServicePack = _countof(szServicePack), ulBuild = _countof(szBuild);

  Check(m_intf->GetSystemVersion(&ulPlatformId, &ulKdMajor, &ulKdMinor,
    szServicePack, ulServicePack, &ulServicePack, &ulServicePackNumber, szBuild, ulBuild, &ulBuild));

  CComQIPtr<IDebugControl4> intf(m_intf);

  Check(intf->GetSystemVersionValues(&ulPlatformId, &ulOsMajor, &ulOsMinor, &ulKdMajor, &ulKdMinor));

  dict info;

  info["PlatformId"] = ulPlatformId;
  info["ServicePack"] = make_tuple(std::string(szServicePack, ulServicePack-1), ulServicePackNumber);
  info["Build"] = std::string(szBuild, ulBuild-1);
  info["Version"] = make_tuple(ulOsMajor, ulOsMinor, ulKdMajor, ulKdMinor);

  return info;
}

void CDebugControl::OutputVersionInformation(OutputTarget target) const
{
  Check(m_intf->OutputVersionInformation((ULONG) target));
}

const list CDebugControl::GetEngineOptions(void) const
{
  ULONG options;

  Check(m_intf->GetEngineOptions(&options));

  return FlagsToList<EngineOption>(static_cast<EngineOption>(options), ENGOPT_IGNORE_DBGHELP_VERSION, ENGOPT_PREFER_DML);
}
void CDebugControl::SetEngineOptions(const list& options) const
{
  Check(m_intf->SetEngineOptions(FlagsFromList<EngineOption>(options)));
}
void CDebugControl::AddEngineOption(CDebugControl::EngineOption option) const
{
  Check(m_intf->AddEngineOptions((ULONG) option));
}
void CDebugControl::RemoveEngineOption(CDebugControl::EngineOption option) const
{
  Check(m_intf->RemoveEngineOptions((ULONG) option));
}

CDebugControl::ExecutionStatus CDebugControl::GetExecutionStatus(void) const
{
  ULONG status;

  Check(m_intf->GetExecutionStatus(&status));

  return (ExecutionStatus) status;
}
void CDebugControl::SetExecutionStatus(ExecutionStatus status) const
{
  Check(m_intf->SetExecutionStatus((ULONG) status));
}

const tuple CDebugControl::GetDebuggeeType(void) const
{
  ULONG Class = 0, Qualifier = 0;

  Check(m_intf->GetDebuggeeType(&Class, &Qualifier));

  switch (Class)
  {
  case CLASS_KERNEL: 
    return make_tuple((DebuggeeClass) Class, (KernelQualifier) Qualifier);
  case CLASS_USER: 
    return make_tuple((DebuggeeClass) Class, (UserQualifier) Qualifier);
  default:
    return make_tuple((DebuggeeClass) Class, Qualifier);
  }
}

CDebugControl::CodeLevel CDebugControl::GetCodeLevel(void) const
{
  ULONG level = 0;

  Check(m_intf->GetCodeLevel(&level));

  return (CodeLevel) level;
}
void CDebugControl::SetCodeLevel(CDebugControl::CodeLevel level) const
{
  Check(m_intf->SetCodeLevel((ULONG) level));
}

CDebugControl::NumberRadix CDebugControl::GetRadix(void) const
{
  ULONG radix = 0;

  Check(m_intf->GetRadix(&radix));

  return (NumberRadix) radix;
}
void CDebugControl::SetRadix(CDebugControl::NumberRadix radix) const
{
  Check(m_intf->SetRadix((ULONG) radix));
}

const std::pair<ULONG, ULONG> CDebugControl::GetSystemErrorControl(void) const
{
  ULONG outputLevel, breakLevel;

  Check(m_intf->GetSystemErrorControl(&outputLevel, &breakLevel));

  return std::make_pair(outputLevel, breakLevel);
}

void CDebugControl::SetSystemOutputLevel(ErrorLevel level) const
{
  Check(m_intf->SetSystemErrorControl((ULONG) level, GetSystemErrorControl().second));  
}
void CDebugControl::SetSystemBreakLevel(ErrorLevel level) const
{
  Check(m_intf->SetSystemErrorControl(GetSystemErrorControl().first, (ULONG) level));  
}

CDebugControl::ProcessorType CDebugControl::GetActualProcessorType(void) const
{
  ULONG type;

  Check(m_intf->GetActualProcessorType(&type));

  return (ProcessorType) type;
}
CDebugControl::ProcessorType CDebugControl::GetEffectiveProcessorType(void) const
{
  ULONG type;

  Check(m_intf->GetEffectiveProcessorType(&type));

  return (ProcessorType) type;
}
void CDebugControl::SetEffectiveProcessorType(CDebugControl::ProcessorType type) const
{
  Check(m_intf->SetEffectiveProcessorType((ULONG) type));
}
CDebugControl::ProcessorType CDebugControl::GetExecutingProcessorType(void) const
{
  ULONG type;

  Check(m_intf->GetExecutingProcessorType(&type));

  return (ProcessorType) type;
}

object CDebugControl::GetCurrentTimeDate(void) const
{
  CComQIPtr<IDebugControl2> intf(m_intf);

  time_t now;

  Check(intf->GetCurrentTimeDate((PULONG) &now));

  tm t;
  
  Check(localtime_s(&t, &now));

  return object(handle<>(::PyDateTime_FromDateAndTime(
    1900 + t.tm_year, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, 0)));
}

object CDebugControl::GetCurrentSystemUpTime(void) const
{
  CComQIPtr<IDebugControl2> intf(m_intf);

  ULONG uptime = 0;

  Check(intf->GetCurrentSystemUpTime(&uptime));

  static const ULONG SECONDS_PER_DAY = 60 * 60 * 24;

  return object(handle<>(::PyDelta_FromDSU(uptime / SECONDS_PER_DAY, uptime % SECONDS_PER_DAY, 0)));
}

const dict CDebugControl::GetEventFilters(void) const
{
  ULONG SpecificEvents, SpecificExceptions, ArbitraryExceptions;

  Check(m_intf->GetNumberEventFilters(&SpecificEvents, &SpecificExceptions, &ArbitraryExceptions));

  dict filters;
  char buf[1024];

  for (size_t i=0; i<SpecificEvents; i++)
  {
    ULONG size = _countof(buf);

    Check(m_intf->GetEventFilterText(i, buf, size, &size));

    CEventFilter filter(this, i, std::string(buf, size-1));

    filters[filter.GetText()] = filter;
  }

  return filters;
}
const dict CDebugControl::GetExceptionFilters(void) const
{
  ULONG SpecificEvents, SpecificExceptions, ArbitraryExceptions;

  Check(m_intf->GetNumberEventFilters(&SpecificEvents, &SpecificExceptions, &ArbitraryExceptions));

  dict filters;
  char buf[1024];

  for (size_t i=SpecificEvents; i<SpecificEvents+SpecificExceptions; i++)
  {
    ULONG size = _countof(buf);

    Check(m_intf->GetEventFilterText(i, buf, size, &size));

    CExceptionFilter filter(this, i, std::string(buf, size-1));

    filters[filter.GetText()] = filter;
  }

  return filters;
}

const dict CDebugControl::GetSupportedProcessorTypes(void) const
{
  ULONG number, type;

  // the processor types supported by the engine.

  Check(m_intf->GetNumberSupportedProcessorTypes(&number));

  dict processors;

  for (ULONG i=0; i<number; i++)
  {
    Check(m_intf->GetSupportedProcessorTypes(i, 1, &type));

    CProcessor processor(this, (ProcessorType) type);

    processors[processor.GetName()] = processor;
  }

  return processors;
}
const dict CDebugControl::GetPossibleExecutingProcessorTypes(void) const
{
  ULONG number, type;

  // the processor types that are supported by the computer running the current target.

  Check(m_intf->GetNumberPossibleExecutingProcessorTypes(&number));

  dict processors;

  for (ULONG i=0; i<number; i++)
  {
    Check(m_intf->GetPossibleExecutingProcessorTypes(i, 1, &type));
    
    CProcessor processor(this, (ProcessorType) type);

    processors[processor.GetName()] = processor;
  }

  return processors;
}

const list CDebugControl::GetBreakpoints(void) const
{
  ULONG Number = 0;

  Check(m_intf->GetNumberBreakpoints(&Number));

  list breakpoints;

  for (size_t i=0; i<Number; i++)
    breakpoints.append(CBreakpoint(this, i));

  return breakpoints;
}
const CDebugControl::CBreakpoint CDebugControl::AddBreakpoint(CDebugControl::BreakpointType type, ULONG id) 
{
  CComPtr<IDebugBreakpoint> intf;

  Check(m_intf->AddBreakpoint((ULONG) type, id, &intf));

  return CBreakpoint(this, intf);
}

void CDebugControl::Output(const std::string& text, OutputMask mask, OutputTarget target) const
{
  if (TARGET_ALL_CLIENTS == target)
    Check(m_intf->Output((ULONG) mask, "%s", text.c_str()));
  else
    Check(m_intf->ControlledOutput((ULONG) target, (ULONG) mask, "%s", text.c_str()));
}

const list CDebugControl::GetEvents(void) const
{
  CComQIPtr<IDebugControl3> intf(m_intf);

  ULONG number;

  Check(intf->GetNumberEvents(&number));

  list events;

  for (ULONG i=0; i<number; i++)
    events.append(CEvent(this, i));

  return events;
}
const CDebugControl::CEvent CDebugControl::GetCurrentEvent(void) const
{
  ULONG idx;

  Check(CComQIPtr<IDebugControl3>(m_intf)->GetCurrentEventIndex(&idx));

  return CEvent(this, idx);
}

bool CDebugControl::WaitForEvent(ULONG timeout) const
{
  HRESULT hr = m_intf->WaitForEvent(0, timeout);

  Check(hr);

  return hr == S_OK;
}

bool CDebugControl::HasInterrupt(void) const
{
  HRESULT hr = m_intf->GetInterrupt();

  if (FAILED(hr)) Check(hr);

  return S_OK == hr;
}
void CDebugControl::SetInterrupt(InterruptType type) const
{
  Check(m_intf->SetInterrupt((ULONG) type));
}

ULONG CDebugControl::GetInterruptTimeout(void) const
{
  ULONG seconds;

  Check(m_intf->GetInterruptTimeout(&seconds));

  return seconds;
}
void CDebugControl::SetInterruptTimeout(ULONG seconds) const
{
  Check(m_intf->SetInterruptTimeout(seconds));
}

CDebugControl::ExpressionSyntax CDebugControl::GetExpressionSyntax(void) const
{
  ULONG syntax;

  Check(CComQIPtr<IDebugControl3>(m_intf)->GetExpressionSyntax(&syntax));

  return (ExpressionSyntax) syntax;
}
void CDebugControl::SetExpressionSyntax(CDebugControl::ExpressionSyntax syntax) const
{
  Check(CComQIPtr<IDebugControl3>(m_intf)->SetExpressionSyntax((ULONG) syntax));  
}

const dict CDebugControl::GetExpressionSyntaxNames(void) const
{
  CComQIPtr<IDebugControl3> intf(m_intf);

  ULONG number;

  Check(intf->GetNumberExpressionSyntaxes(&number));

  char szFullName[64], szAbbrevName[16];
  ULONG nFullName, nAbbrevName;

  dict syntax;

  for (ULONG i=0; i<number; i++)
  {
    Check(intf->GetExpressionSyntaxNames(i, szFullName, _countof(szFullName), &nFullName, szAbbrevName, _countof(szAbbrevName), &nAbbrevName));

    syntax[std::string(szAbbrevName, nAbbrevName-1)] = std::string(szFullName, nFullName-1);
  }

  return syntax;
}
void CDebugControl::SetExpressionSyntaxByName(const std::string& abbrev) const
{
  Check(CComQIPtr<IDebugControl3>(m_intf)->SetExpressionSyntaxByName(abbrev.c_str()));
}

const object CDebugControl::Evaluate(const std::string& expression, ULONG type) const
{
  DEBUG_VALUE value;
  ULONG idx = 0;
  HRESULT hr = m_intf->Evaluate(expression.c_str(), type, &value, &idx);

  if (FAILED(hr)) 
  {
    Check(m_intf->Output(DEBUG_OUTPUT_WARNING, 
      "An error occurred while evaluating the expression.\n"));
    Check(m_intf->Output(DEBUG_OUTPUT_WARNING, 
      "%s\n", expression.c_str()));

    std::string str(idx, ' ');

    str += "^ there was a syntax error or an undefined variable\n";

    Check(m_intf->Output(DEBUG_OUTPUT_WARNING, str.c_str()));

    memset(&value, 0, sizeof(value));
  }

  return SUCCEEDED(hr) ? ToObject(value) : object();
}

void CDebugControl::Execute(const std::string& command, ExecuteFlag flags, OutputTarget target) const
{
  Check(m_intf->Execute((ULONG) target, command.c_str(), (ULONG) flags));
}

void CDebugControl::ExecuteCommandFile(const std::string& commandFile, ExecuteFlag flags, OutputTarget target) const
{
  Check(m_intf->ExecuteCommandFile((ULONG) target, commandFile.c_str(), (ULONG) flags));
}

const list CDebugControl::GetAssemblyOptions(void) const
{
  ULONG options;

  Check(CComQIPtr<IDebugControl3>(m_intf)->GetAssemblyOptions(&options));

  return FlagsToList<AssemblyOption>(static_cast<AssemblyOption>(options), ASMOPT_VERBOSE, ASMOPT_SOURCE_LINE_NUMBER);
}
void CDebugControl::SetAssemblyOptions(const list& options) const
{
  Check(CComQIPtr<IDebugControl3>(m_intf)->SetAssemblyOptions(FlagsFromList<AssemblyOption>(options)));
}
void CDebugControl::AddAssemblyOption(CDebugControl::AssemblyOption options) const
{
  Check(CComQIPtr<IDebugControl3>(m_intf)->AddAssemblyOptions((ULONG) options));
}
void CDebugControl::RemoveAssemblyOption(CDebugControl::AssemblyOption options) const
{
  Check(CComQIPtr<IDebugControl3>(m_intf)->RemoveAssemblyOptions((ULONG) options));
}

ULONG64 CDebugControl::Assemble(ULONG64 offset, const std::string& instr) const
{
  ULONG64 end;

  Check(m_intf->Assemble(offset, instr.c_str(), &end));

  return end;
}
const tuple CDebugControl::Disassemble(ULONG64 offset, DisassembleFlag flags, size_t bufSize) const
{
  ULONG64 end;
  std::string buf(bufSize, '\0');
  ULONG size = buf.size();

  Check(m_intf->Disassemble(offset, (ULONG) flags, const_cast<char *>(buf.c_str()), size, &size, &end));

  buf.resize(size-1);

  return make_tuple(buf, end);
}
ULONG64 CDebugControl::OutputDisassembly(ULONG64 offset, DisassembleFlag flags, OutputTarget target) const
{
  ULONG64 end;

  Check(m_intf->OutputDisassembly((ULONG) target, offset, (ULONG) flags, &end));

  return end;
}

ULONG64 CDebugControl::GetDisassembleEffectiveOffset(void) const
{
  ULONG64 offset;

  Check(m_intf->GetDisassembleEffectiveOffset(&offset));

  return offset;
}
ULONG64 CDebugControl::GetNearInstruction(ULONG64 offset, LONG delta) const
{
  ULONG64 nearOffset;

  Check(m_intf->GetNearInstruction(offset, delta, &nearOffset));

  return nearOffset;
}
ULONG64 CDebugControl::GetReturnOffset(void) const
{
  ULONG64 offset;

  Check(m_intf->GetReturnOffset(&offset));

  return offset;
}

const std::string CDebugControl::Input(void) const
{
  HRESULT hr;
  std::string input(1024, '\0');
  ULONG size;

  while(input.size() < 1024*1024) 
  {
    hr = m_intf->Input(const_cast<char *>(input.c_str()), input.size(), &size);

    if (FAILED(hr)) Check(hr);

    if (S_OK == hr) 
    {
      input.resize(size);
      break;
    }

    input.resize(input.size() * 2);
  };

  return input;
}
void CDebugControl::ReturnInput(const std::string& input) const
{
  Check(m_intf->ReturnInput(input.c_str()));
}

const dict CDebugControl::GetTextMacros(void) const
{
  dict macros;
  char szName[] = "$u0", szMacro[MAX_MACRO_SIZE];
  ULONG nMacro;

  for (int i=0; i<10; i++)
  {
    szName[2] = '0' + i;

    Check(m_intf->GetTextMacro(i, szMacro, _countof(szMacro), &nMacro));

    macros[std::string(szName)] = std::string(szMacro, nMacro-1);
  }

  return macros;
}
void CDebugControl::SetTextMacroBySlot(ULONG slot, const std::string& macro) const
{
  Check(m_intf->SetTextMacro(slot, macro.c_str()));
}
void CDebugControl::SetTextMacroByName(const std::string& name, const std::string& macro) const
{
  char szName[] = "$u0";

  for (int i=0; i<10; i++)
  {
    szName[2] = '0' + i;

    if (0 == strcmp(szName, name.c_str()))
    {
      SetTextMacroBySlot(i, macro);
      return;
    }
  }

  RaiseException("macro name is not exists.", PyExc_KeyError);
}

const dict CDebugControl::GetTextReplacements(void) const
{
  CComQIPtr<IDebugControl2> intf(m_intf);

  ULONG number;
  char szAlias[MAX_MACRO_SIZE], szValue[MAX_MACRO_SIZE];
  ULONG nAlias, nValue;

  Check(intf->GetNumberTextReplacements(&number));

  dict replacements;

  for (ULONG i=0; i<number; i++)
  {
    Check(intf->GetTextReplacement(
      NULL, i, szAlias, _countof(szAlias), &nAlias, szValue, _countof(szValue), &nValue));

    replacements[std::string(szAlias, nAlias-1)] = std::string(szValue, nValue-1);
  }

  return replacements;
}
const std::string CDebugControl::GetTextReplacement(const std::string& alias) const
{
  char szValue[MAX_MACRO_SIZE];
  ULONG nValue = _countof(szValue);

  Check(CComQIPtr<IDebugControl2>(m_intf)->GetTextReplacement(
    alias.c_str(), 0, NULL, 0, NULL, szValue, _countof(szValue), &nValue));

  return std::string(szValue, nValue-1);
}
void CDebugControl::SetTextReplacement(const std::string& alias, const std::string& value) const
{
  Check(CComQIPtr<IDebugControl2>(m_intf)->SetTextReplacement(alias.c_str(), value.c_str()));
}
void CDebugControl::RemoveTextReplacements(void) const
{
  Check(CComQIPtr<IDebugControl2>(m_intf)->RemoveTextReplacements());
}
void CDebugControl::OutputTextReplacements(OutputTarget target) const
{
  Check(CComQIPtr<IDebugControl2>(m_intf)->OutputTextReplacements((ULONG) target, DEBUG_OUT_TEXT_REPL_DEFAULT));
}

const list CDebugControl::GetStackFrames(ULONG64 FrameOffset, ULONG64 StackOffset, ULONG64 InstructionOffset) const
{
  DEBUG_STACK_FRAME frames[MAX_STACK_DEPTH];
  ULONG filled = _countof(frames);

  Check(m_intf->GetStackTrace(FrameOffset, StackOffset, InstructionOffset, 
    frames, _countof(frames), &filled));

  list result;

  for (ULONG i=0; i<filled; i++)
  {
    result.append(CStackFrame(frames[i]));
  }

  return result;
}