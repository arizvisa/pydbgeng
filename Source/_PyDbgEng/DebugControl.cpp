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
#include "DebugClient.h"

#include "DebugControl.h"

#include <sstream>
#include <iomanip>

#include <datetime.h>
#include <boost/python.hpp>
using namespace boost::python;

void CDebugControl::Export(void)
{
	if (::Py_IsInitialized() && !PyDateTimeAPI)
		PyDateTime_IMPORT;

	enum_<OutputControl>("OutputControl")
		.value("THIS_CLIENT",       OutputControl::OUTPUT_THIS_CLIENT)
		.value("ALL_CLIENTS",       OutputControl::OUTPUT_ALL_CLIENTS)
		.value("ALL_OTHER_CLIENTS", OutputControl::OUTPUT_ALL_OTHER_CLIENTS)
		.value("IGNORE",            OutputControl::OUTPUT_IGNORE)
		.value("LOG_ONLY",          OutputControl::OUTPUT_LOG_ONLY)
		.value("NOT_LOGGED",        OutputControl::OUTPUT_NOT_LOGGED)
		.value("OVERRIDE_MASK",     OutputControl::OUTPUT_OVERRIDE_MASK)
		.value("DML",               OutputControl::OUTPUT_DML)
		.value("AMBIENT_DML",       OutputControl::OUTPUT_AMBIENT_DML)
		.value("AMBIENT_TEXT",      OutputControl::OUTPUT_AMBIENT_TEXT)
	;

	enum_<EngineOption>("EngineOption")
		.value("IGNORE_DBGHELP_VERSION",      EngineOption::ENGOPT_IGNORE_DBGHELP_VERSION)
		.value("IGNORE_EXTENSION_VERSIONS",   EngineOption::ENGOPT_IGNORE_EXTENSION_VERSIONS)
		.value("ALLOW_NETWORK_PATHS",         EngineOption::ENGOPT_ALLOW_NETWORK_PATHS)
		.value("DISALLOW_NETWORK_PATHS",      EngineOption::ENGOPT_DISALLOW_NETWORK_PATHS)
		.value("NETWORK_PATHS",               EngineOption::ENGOPT_NETWORK_PATHS)
		.value("IGNORE_LOADER_EXCEPTIONS",    EngineOption::ENGOPT_IGNORE_LOADER_EXCEPTIONS)
		.value("INITIAL_BREAK",               EngineOption::ENGOPT_INITIAL_BREAK)
		.value("INITIAL_MODULE_BREAK",        EngineOption::ENGOPT_INITIAL_MODULE_BREAK)
		.value("FINAL_BREAK",                 EngineOption::ENGOPT_FINAL_BREAK)
		.value("NO_EXECUTE_REPEAT",           EngineOption::ENGOPT_NO_EXECUTE_REPEAT)
		.value("FAIL_INCOMPLETE_INFORMATION", EngineOption::ENGOPT_FAIL_INCOMPLETE_INFORMATION)
		.value("ALLOW_READ_ONLY_BREAKPOINTS", EngineOption::ENGOPT_ALLOW_READ_ONLY_BREAKPOINTS)
		.value("SYNCHRONIZE_BREAKPOINTS",     EngineOption::ENGOPT_SYNCHRONIZE_BREAKPOINTS)
		.value("DISALLOW_SHELL_COMMANDS",     EngineOption::ENGOPT_DISALLOW_SHELL_COMMANDS)
		.value("KD_QUIET_MODE",               EngineOption::ENGOPT_KD_QUIET_MODE)
		.value("DISABLE_MANAGED_SUPPORT",     EngineOption::ENGOPT_DISABLE_MANAGED_SUPPORT)
		.value("DISABLE_MODULE_SYMBOL_LOAD",  EngineOption::ENGOPT_DISABLE_MODULE_SYMBOL_LOAD)
		.value("DISABLE_EXECUTION_COMMANDS",  EngineOption::ENGOPT_DISABLE_EXECUTION_COMMANDS)
		.value("DISALLOW_IMAGE_FILE_MAPPING", EngineOption::ENGOPT_DISALLOW_IMAGE_FILE_MAPPING)
		.value("PREFER_DML",                  EngineOption::ENGOPT_PREFER_DML)
	;

	enum_<ExecutionStatus>("ExecutionStatus")
		.value("NO_CHANGE",           ExecutionStatus::STATUS_NO_CHANGE)
		.value("GO",                  ExecutionStatus::STATUS_GO)
		.value("GO_HANDLED",          ExecutionStatus::STATUS_GO_HANDLED)
		.value("GO_NOT_HANDLED",      ExecutionStatus::STATUS_GO_NOT_HANDLED)
		.value("STEP_OVER",           ExecutionStatus::STATUS_STEP_OVER)
		.value("STEP_INTO",           ExecutionStatus::STATUS_STEP_INTO)
		.value("BREAK",               ExecutionStatus::STATUS_BREAK)
		.value("NO_DEBUGGEE",         ExecutionStatus::STATUS_NO_DEBUGGEE)
		.value("STEP_BRANCH",         ExecutionStatus::STATUS_STEP_BRANCH)
		.value("IGNORE_EVENT",        ExecutionStatus::STATUS_IGNORE_EVENT)
		.value("RESTART_REQUESTED",   ExecutionStatus::STATUS_RESTART_REQUESTED)
		.value("REVERSE_GO",          ExecutionStatus::STATUS_REVERSE_GO)
		.value("REVERSE_STEP_BRANCH", ExecutionStatus::STATUS_REVERSE_STEP_BRANCH)
		.value("REVERSE_STEP_OVER",   ExecutionStatus::STATUS_REVERSE_STEP_OVER)
		.value("REVERSE_STEP_INTO",   ExecutionStatus::STATUS_REVERSE_STEP_INTO)
		.value("OUT_OF_SYNC",         ExecutionStatus::STATUS_OUT_OF_SYNC)
		.value("WAIT_INPUT",          ExecutionStatus::STATUS_WAIT_INPUT)
		.value("TIMEOUT",             ExecutionStatus::STATUS_TIMEOUT)
	;

	enum_<DebuggeeClass>("DebuggeeClass")
		.value("UNINITIALIZED", DebuggeeClass::CLASS_UNINITIALIZED)
		.value("KERNEL",        DebuggeeClass::CLASS_KERNEL)
		.value("USER",          DebuggeeClass::CLASS_USER)
	;

	enum_<DumpType>("DumpType")
		.value("DUMP_SMALL",      DumpType::DUMP_SMALL)
		.value("DUMP_DEFAULT",    DumpType::DUMP_DEFAULT)
		.value("DUMP_FULL",       DumpType::DUMP_FULL)
		.value("DUMP_IMAGE_FILE", DumpType::DUMP_IMAGE_FILE)
		.value("DUMP_TRACE_LOG",  DumpType::DUMP_TRACE_LOG)
		.value("DUMP_WINDOWS_CE", DumpType::DUMP_WINDOWS_CE)
	;

	enum_<UserDebuggee>("UserDebuggee")
		.value("PROCESS",         UserDebuggee::USER_PROCESS)
		.value("PROCESS_SERVER",  UserDebuggee::USER_PROCESS_SERVER)
		.value("IDNA",            UserDebuggee::USER_IDNA)
		.value("SMALL_DUMP",      UserDebuggee::USER_SMALL_DUMP)
		.value("DUMP",            UserDebuggee::USER_DUMP)
		.value("DUMP_WINDOWS_CE", UserDebuggee::USER_DUMP_WINDOWS_CE)
	;

	enum_<KernelDebuggee>("KernelDebuggee")
		.value("CONNECTION",     KernelDebuggee::KERNEL_CONNECTION)
		.value("LOCAL",          KernelDebuggee::KERNEL_LOCAL)
		.value("EXDI_DRIVER",    KernelDebuggee::KERNEL_EXDI_DRIVER)
		.value("IDNA",           KernelDebuggee::KERNEL_IDNA)
		.value("INSTALL_DRIVER", KernelDebuggee::KERNEL_INSTALL_DRIVER)
		.value("SMALL_DUMP",     KernelDebuggee::KERNEL_SMALL_DUMP)
		.value("DUMP",           KernelDebuggee::KERNEL_DUMP)
		.value("FULL_DUMP",      KernelDebuggee::KERNEL_FULL_DUMP)
		.value("TRACE_LOG",      KernelDebuggee::KERNEL_TRACE_LOG)
	;

	enum_<CodeLevel>("CodeLevel")
		.value("SOURCE",   CodeLevel::LEVEL_SOURCE)
		.value("ASSEMBLY", CodeLevel::LEVEL_ASSEMBLY)
	;

	enum_<ErrorLevel>("ErrorLevel")
		.value("ERROR",      ErrorLevel::LEVEL_ERROR)
		.value("MINORERROR", ErrorLevel::LEVEL_MINORERROR)
		.value("WARNING",    ErrorLevel::LEVEL_WARNING)
	;

	enum_<NumberRadix>("NumberRadix")
		.value("OCT", NumberRadix::RADIX_OCT)
		.value("DEC", NumberRadix::RADIX_DEC)
		.value("HEX", NumberRadix::RADIX_HEX)
	;

	enum_<ExecuteFlag>("ExecuteFlag")
		.value("DEFAULT",    ExecuteFlag::EXECUTE_DEFAULT)
		.value("ECHO",       ExecuteFlag::EXECUTE_ECHO)
		.value("NOT_LOGGED", ExecuteFlag::EXECUTE_NOT_LOGGED)
		.value("NO_REPEAT",  ExecuteFlag::EXECUTE_NO_REPEAT)
	;

	enum_<ExpressionSyntax>("ExpressionSyntax")
		.value("MASM", ExpressionSyntax::EXPR_MASM)
		.value("CPP",  ExpressionSyntax::EXPR_CPLUSPLUS)
	;

	enum_<AssemblyOption>("AssemblyOption")
		.value("DEFAULT",             AssemblyOption::ASMOPT_DEFAULT)
		.value("VERBOSE",             AssemblyOption::ASMOPT_VERBOSE)
		.value("NO_CODE_BYTES",       AssemblyOption::ASMOPT_NO_CODE_BYTES)
		.value("IGNORE_OUTPUT_WIDTH", AssemblyOption::ASMOPT_IGNORE_OUTPUT_WIDTH)
		.value("SOURCE_LINE_NUMBER",  AssemblyOption::ASMOPT_SOURCE_LINE_NUMBER)
	;

	enum_<DisassembleFlag>("DisassembleFlag")
		.value("DEFAULT",            DisassembleFlag::DISASM_DEFAULT)
		.value("EFFECTIVE_ADDRESS",  DisassembleFlag::DISASM_EFFECTIVE_ADDRESS)
		.value("MATCHING_SYMBOLS",   DisassembleFlag::DISASM_MATCHING_SYMBOLS)
		.value("SOURCE_LINE_NUMBER", DisassembleFlag::DISASM_SOURCE_LINE_NUMBER)
		.value("SOURCE_FILE_NAME",   DisassembleFlag::DISASM_SOURCE_FILE_NAME)
	;

	enum_<InterruptType>("InterruptType")
		.value("ACTIVE",  InterruptType::INTR_ACTIVE)
		.value("PASSIVE", InterruptType::INTR_PASSIVE)
		.value("EXIT",    InterruptType::INTR_EXIT)
	;

	enum_<PlatformId>("PlatformType")
		.value("WIN32S",  PlatformId::ID_WIN32S)
		.value("WINDOWS", PlatformId::ID_WINDOWS)
		.value("NT",      PlatformId::ID_NT)
	;

	enum_<ProcessorType>("ProcessorType")
		.value("I386",  ProcessorType::TYPE_I386)
		.value("ARM",   ProcessorType::TYPE_ARM)
		.value("IA64",  ProcessorType::TYPE_IA64)
		.value("AMD64", ProcessorType::TYPE_AMD64)
		.value("ALPHA", ProcessorType::TYPE_ALPHA)
		.value("EBC",   ProcessorType::TYPE_EBC)
	;

	enum_<BreakStatus>("BreakStatus")
		.value("BREAK",               BreakStatus::FILTER_BREAK)
		.value("SECOND_CHANCE_BREAK", BreakStatus::FILTER_SECOND_CHANCE_BREAK)
		.value("OUTPUT",              BreakStatus::FILTER_OUTPUT)
		.value("IGNORE",              BreakStatus::FILTER_IGNORE)
		.value("REMOVE",              BreakStatus::FILTER_REMOVE)
	;

	scope DebugControl = class_<CDebugControl>("DebugControl", no_init)
		.add_property("PageSize", &CDebugControl::GetPageSize,
			"the page size for the effective processor mode.")
		.add_property("IsPointer64Bit", &CDebugControl::IsPointer64Bit,
			"the effective processor uses 64-bit pointers.")

		.add_property("VersionInformation", &CDebugControl::GetVersionInformation)
		.def("OutputVersionInformation", &CDebugControl::OutputVersionInformation,
			(arg("target")=OutputControl::OUTPUT_ALL_CLIENTS),    // FIXME
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
			(arg("type")=CDebugBreakpoint::BreakpointType::BREAKPOINT_CODE, arg("id")=DEBUG_ANY_ID),
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
		.def("SetInterrupt", &CDebugControl::SetInterrupt, (arg("type")=InterruptType::INTR_ACTIVE),
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
		.def("Execute", &CDebugControl::Execute, ("command", arg("flags")=ExecuteFlag::EXECUTE_DEFAULT, arg("target")=OutputControl::OUTPUT_ALL_CLIENTS),
			"execute the specified debugger commands.")
		.def("ExecuteCommandFile", &CDebugControl::ExecuteCommandFile,
			("commandFile", arg("flags")=ExecuteFlag::EXECUTE_DEFAULT, arg("target")=OutputControl::OUTPUT_ALL_CLIENTS),
			"open the specified file and execute the debugger commands that are contained within.")

		.add_property("AssemblyOptions", &CDebugControl::GetAssemblyOptions, &CDebugControl::SetAssemblyOptions,
			"the assembly and disassembly options that affect how the debugger engine assembles and disassembles processor instructions for the target.")
		.def("AddAssemblyOption", &CDebugControl::AddAssemblyOption,
			"turns on some of the assembly and disassembly options.")
		.def("RemoveAssemblyOption", &CDebugControl::RemoveAssemblyOption,
			"turns off some of the assembly and disassembly options.")

		.def("Assemble", &CDebugControl::Assemble, ("offset", "instr"),
			"assemble a single processor instruction. The assembled instruction is placed in the target's memory.")
		.def("Disassemble", &CDebugControl::Disassemble, ("offset", arg("flags")=DisassembleFlag::DISASM_DEFAULT, arg("bufSize")=4096),
			"disassemble a processor instruction in the target's memory.")
		.def("OutputDisassembly", &CDebugControl::OutputDisassembly,
			("offset", arg("flags")=DisassembleFlag::DISASM_DEFAULT, arg("target")=OutputControl::OUTPUT_ALL_CLIENTS),
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
			("text", arg("mask")=CDebugOutput::OutputMask::MASK_NORMAL, arg("target")=OutputControl::OUTPUT_ALL_CLIENTS),
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

	class_<CProcessor>("Processor", no_init)
		.add_property("Type", &CProcessor::GetType)

		.add_property("Name", &CProcessor::GetName)
		.add_property("Abbrev", &CProcessor::GetAbbrev)

		.def("__repr__", &CProcessor::Repr)
	;

	class_<CEventFilter>("EventFilter", no_init)
		.add_property("Index", &CEventFilter::GetIndex)
		.add_property("Text", &CEventFilter::GetText)
		.add_property("Status", &CEventFilter::GetStatus, &CEventFilter::SetStatus)
		.add_property("Command", &CEventFilter::GetCommand, &CEventFilter::SetCommand)

		.def("__repr__", &CEventFilter::Repr)
	;

	class_<CExceptionFilter, bases<CEventFilter>>("ExceptionFilter", no_init)
		.add_property("Handled",
			&CExceptionFilter::IsHandled,
			&CExceptionFilter::SetHandled)
		.add_property("SecondCommand",
			&CExceptionFilter::GetSecondCommand,
			&CExceptionFilter::SetSecondCommand)

		.def("__repr__", &CExceptionFilter::Repr)
	;

	class_<CEvent>("Event", no_init)
		.add_property("Index", &CEvent::GetIndex)
		.add_property("Name", &CEvent::GetName)

		.def("__repr__", &CEvent::Repr)
	;

	class_<CStackFrame>("StackFrame", no_init)
		.add_property("InstructionOffset", &CStackFrame::GetInstructionOffset,
			"The location in the process's virtual address space of the related instruction for the stack frame.")
		.add_property("ReturnOffset", &CStackFrame::GetReturnOffset,
			"The location in the process's virtual address space of the return address for the stack frame.")
		.add_property("FrameOffset", &CStackFrame::GetFrameOffset,
			"The location in the process's virtual address space of the stack frame, if known.")
		.add_property("StackOffset", &CStackFrame::GetStackOffset,
			"The location in the process's virtual address space of the processor stack.")
		.add_property("FuncTableEntry", &CStackFrame::GetFuncTableEntry,
			"The location in the target's virtual address space of the function entry for this frame, if available.")
		.add_property("Params", &CStackFrame::GetParams,
			"The values of the first four stack slots that are passed to the function, if available.")
		.add_property("Virtual", &CStackFrame::IsVirtual,
			"his stack frame was generated by the debugger by unwinding.")
		.add_property("FrameNumber", &CStackFrame::GetFrameNumber,
			"The index of the frame. This index counts the number of frames from the top of the call stack. ")
	;
}

const std::pair<const std::string, const std::string> CDebugControl::CProcessor::GetNames(void) const
{
	char szName[256], szAbbrev[256];
	ULONG ulName = _countof(szName), ulAbbrev = _countof(szAbbrev);
	Check(m_intf->GetProcessorTypeNames(static_cast<ULONG>(m_type), szName, ulName, &ulName, szAbbrev, ulAbbrev, &ulAbbrev));
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
	return static_cast<BreakStatus>(param.ExecutionOption);
}

void CDebugControl::CEventFilter::SetStatus(CDebugControl::BreakStatus status) const
{
	DEBUG_SPECIFIC_FILTER_PARAMETERS param;
	Check(m_intf->GetSpecificFilterParameters(m_idx, 1, &param));

	param.ExecutionOption = static_cast<ULONG>(status);
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
	return static_cast<BreakStatus>(param.ExecutionOption);
}

void CDebugControl::CExceptionFilter::SetStatus(BreakStatus status) const
{
	DEBUG_EXCEPTION_FILTER_PARAMETERS param;
	Check(m_intf->GetExceptionFilterParameters(1, NULL, m_idx, &param));

	param.ExecutionOption = static_cast<ULONG>(status);
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

const std::string CDebugControl::CEvent::GetName(void) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);
	char szName[256];
	ULONG nName = _countof(szName);

	// FIXME: DEBUG_EINDEX_NAME should be an enumeration
	Check(intf->GetEventIndexDescription(m_idx, DEBUG_EINDEX_NAME, szName, nName, &nName));
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
	info["ServicePack"] = make_tuple( str(std::string(szServicePack,ulServicePack-1)), ulServicePackNumber);
	info["Build"] = std::string(szBuild, ulBuild-1);
	info["Version"] = make_tuple(ulOsMajor, ulOsMinor, ulKdMajor, ulKdMinor);
	return info;
}

void CDebugControl::OutputVersionInformation(CDebugControl::OutputControl target) const
{
	Check(m_intf->OutputVersionInformation((ULONG) target));
}

const list CDebugControl::GetEngineOptions(void) const
{
	ULONG res;
	Check(m_intf->GetEngineOptions(&res));

	return utils::FlagsToList(static_cast<EngineOption>(res), EngineOption::ENGOPT_IGNORE_DBGHELP_VERSION, EngineOption::ENGOPT_PREFER_DML);
}

void CDebugControl::SetEngineOptions(const list& options) const
{
	ULONG res = utils::FlagsFromList<ULONG, EngineOption>(options);

	Check(m_intf->SetEngineOptions(res));
}

void CDebugControl::AddEngineOption(CDebugControl::EngineOption option) const
{
	ULONG res = static_cast<ULONG>(option);

	Check(m_intf->AddEngineOptions(res));
}

void CDebugControl::RemoveEngineOption(CDebugControl::EngineOption option) const
{
	Check(m_intf->RemoveEngineOptions(static_cast<ULONG>(option)));
}

CDebugControl::ExecutionStatus CDebugControl::GetExecutionStatus(void) const
{
	ULONG status;

	Check(m_intf->GetExecutionStatus(&status));
	return (ExecutionStatus) status;
}

void CDebugControl::SetExecutionStatus(ExecutionStatus status) const
{
	Check(m_intf->SetExecutionStatus(static_cast<ULONG>(status)));
}

const tuple CDebugControl::GetDebuggeeType(void) const
{
	ULONG Class = 0, Qualifier = 0;
	Check(m_intf->GetDebuggeeType(&Class, &Qualifier));

	switch (Class) {
	case static_cast<ULONG>(DebuggeeClass::CLASS_KERNEL):
		return make_tuple(static_cast<DebuggeeClass>(Class), static_cast<KernelDebuggee>(Qualifier));
	case static_cast<ULONG>(DebuggeeClass::CLASS_USER):
		return make_tuple(static_cast<DebuggeeClass>(Class), static_cast<CDebugControl::UserDebuggee>(Qualifier));
	default:
		return make_tuple(static_cast<DebuggeeClass>(Class), Qualifier);
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
	Check(m_intf->SetCodeLevel(static_cast<ULONG>(level)));
}

CDebugControl::NumberRadix CDebugControl::GetRadix(void) const
{
	ULONG radix = 0;

	Check(m_intf->GetRadix(&radix));
	return static_cast<NumberRadix>(radix);
}

void CDebugControl::SetRadix(CDebugControl::NumberRadix radix) const
{
	Check(m_intf->SetRadix(static_cast<ULONG>(radix)));
}

const std::pair<ULONG, ULONG> CDebugControl::GetSystemErrorControl(void) const
{
	ULONG outputLevel, breakLevel;

	Check(m_intf->GetSystemErrorControl(&outputLevel, &breakLevel));
	return std::make_pair(outputLevel, breakLevel);
}

void CDebugControl::SetSystemOutputLevel(ErrorLevel level) const
{
	Check(m_intf->SetSystemErrorControl(static_cast<ULONG>(level), GetSystemErrorControl().second));
}

void CDebugControl::SetSystemBreakLevel(ErrorLevel level) const
{
	Check(m_intf->SetSystemErrorControl(GetSystemErrorControl().first, static_cast<ULONG>(level)));
}

CDebugControl::ProcessorType CDebugControl::GetActualProcessorType(void) const
{
	ULONG type;

	Check(m_intf->GetActualProcessorType(&type));
	return static_cast<ProcessorType>(type);
}

CDebugControl::ProcessorType CDebugControl::GetEffectiveProcessorType(void) const
{
	ULONG type;

	Check(m_intf->GetEffectiveProcessorType(&type));
	return static_cast<ProcessorType>(type);
}

void CDebugControl::SetEffectiveProcessorType(CDebugControl::ProcessorType type) const
{
	Check(m_intf->SetEffectiveProcessorType(static_cast<ULONG>(type)));
}

CDebugControl::ProcessorType CDebugControl::GetExecutingProcessorType(void) const
{
	ULONG type;

	Check(m_intf->GetExecutingProcessorType(&type));
	return static_cast<ProcessorType>(type);
}

object CDebugControl::GetCurrentTimeDate(void) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);

	ULONG current;
	Check(intf->GetCurrentTimeDate(&current));

	time_t now = static_cast<time_t>(current);

	tm t;
	Check(localtime_s(&t, &now));

	return object(handle<>(::PyDateTime_FromDateAndTime(
		1900 + t.tm_year, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, 0)));
}

object CDebugControl::GetCurrentSystemUpTime(void) const
{
	static const ULONG SECONDS_PER_DAY = 60 * 60 * 24;

	CComQIPtr<IDebugControl2> intf(m_intf);
	ULONG uptime = 0;

	Check(intf->GetCurrentSystemUpTime(&uptime));
	return object(handle<>(::PyDelta_FromDSU(uptime / SECONDS_PER_DAY, uptime % SECONDS_PER_DAY, 0)));
}

const dict CDebugControl::GetEventFilters(void) const
{
	ULONG SpecificEvents, SpecificExceptions, ArbitraryExceptions;
	Check(m_intf->GetNumberEventFilters(&SpecificEvents, &SpecificExceptions, &ArbitraryExceptions));

	dict filters; char buf[1024];
	for (size_t i=0; i<SpecificEvents; i++) {
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

	dict filters; char buf[1024];
	for (size_t i=SpecificEvents; i<SpecificEvents+SpecificExceptions; i++) {
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
	for (ULONG i=0; i<number; i++) {
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
	for (ULONG i=0; i<number; i++) {
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

	list res;
	for (size_t i=0; i<Number; i++)
		res.append(CDebugBreakpoint(this, i));
	return res;
}

CDebugBreakpoint
CDebugControl::AddBreakpoint(CDebugBreakpoint::BreakpointType type, ULONG id)
{
	CComPtr<IDebugBreakpoint> intf;

	Check(m_intf->AddBreakpoint(static_cast<ULONG>(type), id, &intf));
	return CDebugBreakpoint(this, intf);
}

void CDebugControl::Output(const std::string& text, CDebugOutput::OutputMask mask, CDebugControl::OutputControl target) const
{
	if (CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS == target) {
		Check(m_intf->Output(static_cast<ULONG>(mask), "%s", text.c_str()));
	} else {
		Check(m_intf->ControlledOutput(static_cast<ULONG>(target), static_cast<ULONG>(mask), "%s", text.c_str()));
	}
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
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG idx;

	Check(intf->GetCurrentEventIndex(&idx));
	return CEvent(this, idx);
}

ULONG CDebugControl::WaitForEvent(ULONG timeout) const
{
	// FIXME: Check() this if FAILED() or raise an exception or something?
	return m_intf->WaitForEvent(0, timeout);
}

bool CDebugControl::HasInterrupt(void) const
{
	HRESULT hr = m_intf->GetInterrupt();
	if (FAILED(hr))
		Check(hr);
	return hr;
}

void CDebugControl::SetInterrupt(InterruptType type) const
{
	Check(m_intf->SetInterrupt(static_cast<ULONG>(type)));
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
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG syntax;

	Check(intf->GetExpressionSyntax(&syntax));
	return static_cast<ExpressionSyntax>(syntax);
}

void CDebugControl::SetExpressionSyntax(CDebugControl::ExpressionSyntax syntax) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG res = static_cast<ULONG>(syntax);

	Check(intf->SetExpressionSyntax(res));
}

const dict CDebugControl::GetExpressionSyntaxNames(void) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);

	ULONG number;
	Check(intf->GetNumberExpressionSyntaxes(&number));

	char szFullName[64], szAbbrevName[16];
	ULONG nFullName, nAbbrevName;
	dict syntax;

	for (ULONG i=0; i<number; i++) {
		Check(intf->GetExpressionSyntaxNames(i, szFullName, _countof(szFullName), &nFullName, szAbbrevName, _countof(szAbbrevName), &nAbbrevName));
		syntax[std::string(szAbbrevName, nAbbrevName-1)] = std::string(szFullName, nFullName-1);
	}
	return syntax;
}

void CDebugControl::SetExpressionSyntaxByName(const std::string& abbrev) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);

	Check(intf->SetExpressionSyntaxByName(abbrev.c_str()));
}

const object CDebugControl::Evaluate(const std::string& expression, ULONG type) const
{
	DEBUG_VALUE value;
	ULONG idx = 0;
	HRESULT hr = m_intf->Evaluate(expression.c_str(), type, &value, &idx);

	if (FAILED(hr)) {
		// FIXME: consolidate this into an exception or something?
		Check(m_intf->Output(DEBUG_OUTPUT_WARNING, "An error occurred while evaluating the expression.\n"));
		Check(m_intf->Output(DEBUG_OUTPUT_WARNING, "%s\n", expression.c_str()));

		std::string str(idx, ' ');
		str += "^ there was a syntax error or an undefined variable\n";

		Check(m_intf->Output(DEBUG_OUTPUT_WARNING, str.c_str()));
		memset(&value, 0, sizeof(value));
	}
	return SUCCEEDED(hr)? utils::ToObject(value) : object();
}

void CDebugControl::Execute(const std::string& command, ExecuteFlag flags, CDebugControl::OutputControl target) const
{
	Check(m_intf->Execute(static_cast<ULONG>(target), command.c_str(), static_cast<ULONG>(flags)));
}

void CDebugControl::ExecuteCommandFile(const std::string& commandFile, ExecuteFlag flags, CDebugControl::OutputControl target) const
{
	Check(m_intf->ExecuteCommandFile(static_cast<ULONG>(target), commandFile.c_str(), static_cast<ULONG>(flags)));
}

const list CDebugControl::GetAssemblyOptions(void) const
{
	ULONG res;
	CComQIPtr<IDebugControl3> intf(m_intf);
	Check(intf->GetAssemblyOptions(&res));

	return utils::FlagsToList(static_cast<AssemblyOption>(res), AssemblyOption::ASMOPT_VERBOSE, AssemblyOption::ASMOPT_SOURCE_LINE_NUMBER);
}

void CDebugControl::SetAssemblyOptions(const list& options) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG res = utils::FlagsFromList<ULONG, AssemblyOption>(options);

	Check(intf->SetAssemblyOptions(res));
}

void CDebugControl::AddAssemblyOption(CDebugControl::AssemblyOption options) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG res = static_cast<ULONG>(options);

	Check(intf->AddAssemblyOptions(res));
}

void CDebugControl::RemoveAssemblyOption(CDebugControl::AssemblyOption options) const
{
	CComQIPtr<IDebugControl3> intf(m_intf);
	ULONG res = static_cast<ULONG>(options);

	Check(intf->RemoveAssemblyOptions(res));
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

	Check(m_intf->Disassemble(offset, static_cast<ULONG>(flags), const_cast<char*>(buf.c_str()), size, &size, &end));
	buf.resize(size-1);
	return make_tuple( str(buf), end);
}

ULONG64 CDebugControl::OutputDisassembly(ULONG64 offset, DisassembleFlag flags, CDebugControl::OutputControl target) const
{
	ULONG64 end;

	Check(m_intf->OutputDisassembly(static_cast<ULONG>(target), offset, static_cast<ULONG>(flags), &end));
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

	while(input.size() < 1024*1024) {
		hr = m_intf->Input(const_cast<char *>(input.c_str()), input.size(), &size);

		if (FAILED(hr))
			Check(hr);

		if (S_OK == hr) {
			input.resize(size);
			break;
		}
		input.resize(input.size() * 2);
	}
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

	for (int i=0; i<10; i++) {
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

	for (int i=0; i<10; i++) {
		szName[2] = '0' + i;

		if (0 == strcmp(szName, name.c_str())) {
			SetTextMacroBySlot(i, macro);
			return;
		}
	}

	// FIXME: is this the right thing to do?
	utils::RaiseException("macro name is not exists.", PyExc_KeyError);
}

const dict CDebugControl::GetTextReplacements(void) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);

	ULONG number;
	Check(intf->GetNumberTextReplacements(&number));

	char szAlias[MAX_MACRO_SIZE], szValue[MAX_MACRO_SIZE];
	ULONG nAlias, nValue;

	dict replacements;
	for (ULONG i=0; i<number; i++) {
		Check(intf->GetTextReplacement(NULL, i, szAlias, _countof(szAlias), &nAlias, szValue, _countof(szValue), &nValue));

		replacements[std::string(szAlias, nAlias-1)] = std::string(szValue, nValue-1);
	}
	return replacements;
}

const std::string CDebugControl::GetTextReplacement(const std::string& alias) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);

	char szValue[MAX_MACRO_SIZE];
	ULONG nValue = _countof(szValue);

	Check(intf->GetTextReplacement(alias.c_str(), 0, NULL, 0, NULL, szValue, _countof(szValue), &nValue));
	return std::string(szValue, nValue-1);
}

void CDebugControl::SetTextReplacement(const std::string& alias, const std::string& value) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);

	Check(intf->SetTextReplacement(alias.c_str(), value.c_str()));
}

void CDebugControl::RemoveTextReplacements(void) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);

	Check(intf->RemoveTextReplacements());
}

void CDebugControl::OutputTextReplacements(CDebugControl::OutputControl target) const
{
	CComQIPtr<IDebugControl2> intf(m_intf);
	ULONG res = static_cast<ULONG>(target);

	// FIXME: make DEBUG_OUT_TEXT_REPL_DEFAULT an enumeration
	Check(intf->OutputTextReplacements(res, DEBUG_OUT_TEXT_REPL_DEFAULT));
}

const list CDebugControl::GetStackFrames(ULONG64 FrameOffset, ULONG64 StackOffset, ULONG64 InstructionOffset) const
{
	DEBUG_STACK_FRAME frames[MAX_STACK_DEPTH];
	ULONG filled = _countof(frames);

	Check(m_intf->GetStackTrace(FrameOffset, StackOffset, InstructionOffset,
		frames, _countof(frames), &filled));

	list result;
	for (ULONG i=0; i<filled; i++)
		result.append(CStackFrame(frames[i]));
	return result;
}

/* CDebugControl::CProcessor */
CDebugControl::ProcessorType
CDebugControl::CProcessor::GetType(void) const
{
	return m_type;
}

const std::string
CDebugControl::CProcessor::GetName(void) const
{
	return GetNames().first;
}

const std::string
CDebugControl::CProcessor::GetAbbrev(void) const
{
	return GetNames().second;
}

const object
CDebugControl::CProcessor::Repr(const CProcessor& processor)
{
	return "(Processor %s : %s)" % make_tuple(processor.GetAbbrev().c_str(), processor.GetName().c_str());
}

/* CDebugControl::CEventFilter */
size_t
CDebugControl::CEventFilter::GetIndex(void) const
{
	return m_idx;
}

const std::string
CDebugControl::CEventFilter::GetText(void) const
{
	return m_text;
}

/*
const std::string CDebugControl::CEventFilter::GetCommand(void) const
void CDebugControl::CEventFilter::SetCommand(void) const

const std::string CDebugControl::CEventFilter::GetArgument(void) const;
void CDebugControl::CEventFilter::SetArgument(const std::string& argument) const;
virtual CDebugControl::BreakStatus CDebugControl::CEventFilter::GetStatus(void) const;
virtual void CDebugControl::CEventFilter::SetStatus(CDebugControl::BreakStatus status) const;
*/
const object
CDebugControl::CEventFilter::Repr(const CEventFilter& filter)
{
	return "(Event %s : %s)" % make_tuple(filter.GetText().c_str(), filter.GetStatus());
}

/* CDebugControl::CExceptionFilter */
const object
CDebugControl::CExceptionFilter::Repr(const CExceptionFilter& filter)
{
	return "(Exception %s - %s - %s)" % make_tuple(filter.GetText().c_str(), filter.GetStatus(), (filter.IsHandled() ? "handled" : "not handled"));
}

/* CDebugControl::CEvent */
ULONG
CDebugControl::CEvent::GetIndex(void) const
{
	return m_idx;
}

const object
CDebugControl::CEvent::Repr(const CDebugControl::CEvent& evt) const
{
	return "(Event %s)" % make_tuple(evt.GetName().c_str());
}

/* CDebugControl::CStackFrame */
ULONG64
CDebugControl::CStackFrame::GetInstructionOffset(void) const
{
	return m_frame.InstructionOffset;
}

ULONG64
CDebugControl::CStackFrame::GetReturnOffset(void) const
{
	return m_frame.ReturnOffset;
}

ULONG64
CDebugControl::CStackFrame::GetFrameOffset(void) const
{
	return m_frame.FrameOffset;
}

ULONG64
CDebugControl::CStackFrame::GetStackOffset(void) const
{
	return m_frame.StackOffset;
}

ULONG64
CDebugControl::CStackFrame::GetFuncTableEntry(void) const
{
	return m_frame.InstructionOffset;
}

const tuple
CDebugControl::CStackFrame::GetParams(void) const
{
	return make_tuple(m_frame.Params[0], m_frame.Params[1], m_frame.Params[2], m_frame.Params[3]);
}

bool
CDebugControl::CStackFrame::IsVirtual(void) const
{
	return m_frame.Virtual;
}

ULONG
CDebugControl::CStackFrame::GetFrameNumber(void) const
{
	return m_frame.FrameNumber;
}
