#ifndef __DebugControl_h
#define __DebugControl_h
#pragma once

#include <boost/python.hpp>
#include <dbgeng.h>

using namespace boost::python;

#include "DebugOutput.h"
#include "DebugBreakpoint.h"

/** Class declarations **/
class CDebugControl : public CDebugObject<IDebugControl>
{
	typedef CDebugObject<IDebugControl> DebugInterface;
	static const size_t MAX_MACRO_SIZE = 1024;
	static const size_t MAX_STACK_DEPTH = 64;

public:
	enum class OutputControl : ULONG {
		OUTPUT_THIS_CLIENT = DEBUG_OUTCTL_THIS_CLIENT,
		OUTPUT_ALL_CLIENTS = DEBUG_OUTCTL_ALL_CLIENTS,
		OUTPUT_ALL_OTHER_CLIENTS = DEBUG_OUTCTL_ALL_OTHER_CLIENTS,
		OUTPUT_IGNORE = DEBUG_OUTCTL_IGNORE,
		OUTPUT_LOG_ONLY = DEBUG_OUTCTL_LOG_ONLY,
		OUTPUT_NOT_LOGGED = DEBUG_OUTCTL_NOT_LOGGED,
		OUTPUT_OVERRIDE_MASK = DEBUG_OUTCTL_OVERRIDE_MASK,
		OUTPUT_DML = DEBUG_OUTCTL_DML,
		OUTPUT_AMBIENT_DML = DEBUG_OUTCTL_AMBIENT_DML,
		OUTPUT_AMBIENT_TEXT = DEBUG_OUTCTL_AMBIENT_TEXT,
	};

	enum class EngineOption : ULONG {
		ENGOPT_IGNORE_DBGHELP_VERSION = DEBUG_ENGOPT_IGNORE_DBGHELP_VERSION,
		ENGOPT_IGNORE_EXTENSION_VERSIONS = DEBUG_ENGOPT_IGNORE_EXTENSION_VERSIONS,
		ENGOPT_ALLOW_NETWORK_PATHS = DEBUG_ENGOPT_ALLOW_NETWORK_PATHS,
		ENGOPT_DISALLOW_NETWORK_PATHS = DEBUG_ENGOPT_DISALLOW_NETWORK_PATHS,
		ENGOPT_NETWORK_PATHS = DEBUG_ENGOPT_NETWORK_PATHS,
		ENGOPT_IGNORE_LOADER_EXCEPTIONS = DEBUG_ENGOPT_IGNORE_LOADER_EXCEPTIONS,
		ENGOPT_INITIAL_BREAK = DEBUG_ENGOPT_INITIAL_BREAK,
		ENGOPT_INITIAL_MODULE_BREAK = DEBUG_ENGOPT_INITIAL_MODULE_BREAK,
		ENGOPT_FINAL_BREAK = DEBUG_ENGOPT_FINAL_BREAK,
		ENGOPT_NO_EXECUTE_REPEAT = DEBUG_ENGOPT_NO_EXECUTE_REPEAT,
		ENGOPT_FAIL_INCOMPLETE_INFORMATION = DEBUG_ENGOPT_FAIL_INCOMPLETE_INFORMATION,
		ENGOPT_ALLOW_READ_ONLY_BREAKPOINTS = DEBUG_ENGOPT_ALLOW_READ_ONLY_BREAKPOINTS,
		ENGOPT_SYNCHRONIZE_BREAKPOINTS = DEBUG_ENGOPT_SYNCHRONIZE_BREAKPOINTS,
		ENGOPT_DISALLOW_SHELL_COMMANDS = DEBUG_ENGOPT_DISALLOW_SHELL_COMMANDS,
		ENGOPT_KD_QUIET_MODE = DEBUG_ENGOPT_KD_QUIET_MODE,
		ENGOPT_DISABLE_MANAGED_SUPPORT = DEBUG_ENGOPT_DISABLE_MANAGED_SUPPORT,
		ENGOPT_DISABLE_MODULE_SYMBOL_LOAD = DEBUG_ENGOPT_DISABLE_MODULE_SYMBOL_LOAD,
		ENGOPT_DISABLE_EXECUTION_COMMANDS = DEBUG_ENGOPT_DISABLE_EXECUTION_COMMANDS,
		ENGOPT_DISALLOW_IMAGE_FILE_MAPPING = DEBUG_ENGOPT_DISALLOW_IMAGE_FILE_MAPPING,
		ENGOPT_PREFER_DML = DEBUG_ENGOPT_PREFER_DML,
	};

	enum class ExecutionStatus : ULONG {
		STATUS_NO_CHANGE = DEBUG_STATUS_NO_CHANGE,
		STATUS_GO = DEBUG_STATUS_GO,
		STATUS_GO_HANDLED = DEBUG_STATUS_GO_HANDLED,
		STATUS_GO_NOT_HANDLED = DEBUG_STATUS_GO_NOT_HANDLED,
		STATUS_STEP_OVER = DEBUG_STATUS_STEP_OVER,
		STATUS_STEP_INTO = DEBUG_STATUS_STEP_INTO,
		STATUS_BREAK = DEBUG_STATUS_BREAK,
		STATUS_NO_DEBUGGEE = DEBUG_STATUS_NO_DEBUGGEE,
		STATUS_STEP_BRANCH = DEBUG_STATUS_STEP_BRANCH,
		STATUS_IGNORE_EVENT = DEBUG_STATUS_IGNORE_EVENT,
		STATUS_RESTART_REQUESTED = DEBUG_STATUS_RESTART_REQUESTED,
		STATUS_REVERSE_GO = DEBUG_STATUS_REVERSE_GO,
		STATUS_REVERSE_STEP_BRANCH = DEBUG_STATUS_REVERSE_STEP_BRANCH,
		STATUS_REVERSE_STEP_OVER = DEBUG_STATUS_REVERSE_STEP_OVER,
		STATUS_REVERSE_STEP_INTO = DEBUG_STATUS_REVERSE_STEP_INTO,
		STATUS_OUT_OF_SYNC = DEBUG_STATUS_OUT_OF_SYNC,
		STATUS_WAIT_INPUT = DEBUG_STATUS_WAIT_INPUT,
		STATUS_TIMEOUT = DEBUG_STATUS_TIMEOUT,
	};

	enum class DebuggeeClass : ULONG {
		CLASS_UNINITIALIZED = DEBUG_CLASS_UNINITIALIZED,
		CLASS_KERNEL = DEBUG_CLASS_KERNEL,
		CLASS_USER = DEBUG_CLASS_USER_WINDOWS,
		CLASS_IMAGE_FILE = DEBUG_CLASS_IMAGE_FILE,
	};

	enum class DumpType : ULONG {
		DUMP_SMALL = DEBUG_DUMP_SMALL,
		DUMP_DEFAULT = DEBUG_DUMP_DEFAULT,
		DUMP_FULL = DEBUG_DUMP_FULL,
		DUMP_IMAGE_FILE = DEBUG_DUMP_IMAGE_FILE,
		DUMP_TRACE_LOG = DEBUG_DUMP_TRACE_LOG,
		DUMP_WINDOWS_CE = DEBUG_DUMP_WINDOWS_CE,
	};

	enum class UserDebuggee : ULONG {
		USER_PROCESS = DEBUG_USER_WINDOWS_PROCESS,
		USER_PROCESS_SERVER = DEBUG_USER_WINDOWS_PROCESS_SERVER,
		USER_IDNA = DEBUG_USER_WINDOWS_IDNA,
		USER_SMALL_DUMP = DEBUG_USER_WINDOWS_SMALL_DUMP,
		USER_DUMP = DEBUG_USER_WINDOWS_DUMP,
		USER_DUMP_WINDOWS_CE = DEBUG_USER_WINDOWS_DUMP_WINDOWS_CE,
	};

	enum class KernelDebuggee : ULONG {
		KERNEL_CONNECTION = DEBUG_KERNEL_CONNECTION,
		KERNEL_LOCAL = DEBUG_KERNEL_LOCAL,
		KERNEL_EXDI_DRIVER = DEBUG_KERNEL_EXDI_DRIVER,
		KERNEL_IDNA = DEBUG_KERNEL_IDNA,
		KERNEL_INSTALL_DRIVER = DEBUG_KERNEL_INSTALL_DRIVER,

		KERNEL_SMALL_DUMP = DEBUG_KERNEL_SMALL_DUMP,
		KERNEL_DUMP = DEBUG_KERNEL_DUMP,
		KERNEL_FULL_DUMP = DEBUG_KERNEL_FULL_DUMP,

		KERNEL_TRACE_LOG = DEBUG_KERNEL_TRACE_LOG,
	};

	enum class NumberRadix : ULONG {
		RADIX_OCT = 8,
		RADIX_DEC = 10,
		RADIX_HEX = 16,
	};

	enum class CodeLevel : ULONG {
		LEVEL_SOURCE = DEBUG_LEVEL_SOURCE,
		LEVEL_ASSEMBLY = DEBUG_LEVEL_ASSEMBLY,
	};

	enum class ErrorLevel : ULONG {
		LEVEL_ERROR = SLE_ERROR,
		LEVEL_MINORERROR = SLE_MINORERROR,
		LEVEL_WARNING = SLE_WARNING,
	};

	enum class ExecuteFlag : ULONG {
		EXECUTE_DEFAULT = DEBUG_EXECUTE_DEFAULT,
		EXECUTE_ECHO = DEBUG_EXECUTE_ECHO,
		EXECUTE_NOT_LOGGED = DEBUG_EXECUTE_NOT_LOGGED,
		EXECUTE_NO_REPEAT = DEBUG_EXECUTE_NO_REPEAT,
	};

	enum class ExpressionSyntax : ULONG {
		EXPR_MASM = DEBUG_EXPR_MASM,
		EXPR_CPLUSPLUS = DEBUG_EXPR_CPLUSPLUS,
	};

	enum class AssemblyOption : ULONG {
		ASMOPT_DEFAULT = DEBUG_ASMOPT_DEFAULT,
		ASMOPT_VERBOSE = DEBUG_ASMOPT_VERBOSE,
		ASMOPT_NO_CODE_BYTES = DEBUG_ASMOPT_NO_CODE_BYTES,
		ASMOPT_IGNORE_OUTPUT_WIDTH = DEBUG_ASMOPT_IGNORE_OUTPUT_WIDTH,
		ASMOPT_SOURCE_LINE_NUMBER = DEBUG_ASMOPT_SOURCE_LINE_NUMBER,
	};

	enum class DisassembleFlag : ULONG {
		DISASM_DEFAULT = 0,
		DISASM_EFFECTIVE_ADDRESS = DEBUG_DISASM_EFFECTIVE_ADDRESS,
		DISASM_MATCHING_SYMBOLS = DEBUG_DISASM_MATCHING_SYMBOLS,
		DISASM_SOURCE_LINE_NUMBER = DEBUG_DISASM_SOURCE_LINE_NUMBER,
		DISASM_SOURCE_FILE_NAME = DEBUG_DISASM_SOURCE_FILE_NAME,
	};

	enum class InterruptType : ULONG {
		INTR_ACTIVE = DEBUG_INTERRUPT_ACTIVE,
		INTR_PASSIVE = DEBUG_INTERRUPT_PASSIVE,
		INTR_EXIT = DEBUG_INTERRUPT_EXIT,
	};

	enum class PlatformId : ULONG {
		ID_WIN32S = VER_PLATFORM_WIN32s,
		ID_WINDOWS = VER_PLATFORM_WIN32_WINDOWS,
		ID_NT = VER_PLATFORM_WIN32_NT,
	};

	enum class ProcessorType : ULONG {
		TYPE_I386 = IMAGE_FILE_MACHINE_I386,
		TYPE_ARM = IMAGE_FILE_MACHINE_ARM,
		TYPE_IA64 = IMAGE_FILE_MACHINE_IA64,
		TYPE_AMD64 = IMAGE_FILE_MACHINE_AMD64,
		TYPE_ALPHA = IMAGE_FILE_MACHINE_ALPHA,
		TYPE_EBC = IMAGE_FILE_MACHINE_EBC,
	};

	class CProcessor : public DebugInterface {
		ProcessorType m_type;

		const std::pair<const std::string, const std::string> GetNames(void) const;
	public:
		CProcessor(const CDebugControl* owner, ProcessorType type)
			: DebugInterface(owner->GetInterface()), m_type(type)
		{ }

		ProcessorType GetType(void) const;
		const std::string GetName(void) const;
		const std::string GetAbbrev(void) const;
		static const object Repr(const CProcessor& processor);
	};

	enum class BreakStatus : ULONG {
		FILTER_BREAK = DEBUG_FILTER_BREAK,
		FILTER_SECOND_CHANCE_BREAK = DEBUG_FILTER_SECOND_CHANCE_BREAK,
		FILTER_OUTPUT = DEBUG_FILTER_OUTPUT,
		FILTER_IGNORE = DEBUG_FILTER_IGNORE,
		FILTER_REMOVE = DEBUG_FILTER_REMOVE,
	};

	class CEventFilter : public DebugInterface {
	protected:
		size_t m_idx;
		std::string m_text;

	public:
		CEventFilter(const CDebugControl* owner, size_t idx, const std::string& text)
			: DebugInterface(owner->GetInterface()), m_idx(idx), m_text(text)
		{ }

		size_t GetIndex(void) const;
		const std::string GetText(void) const;

		const std::string GetCommand(void) const;
		void SetCommand(const std::string& command) const;

		const std::string GetArgument(void) const;
		void SetArgument(const std::string& argument) const;

		virtual BreakStatus GetStatus(void) const;
		virtual void SetStatus(BreakStatus status) const;

		static const object Repr(const CEventFilter& filter);
	};

	class CExceptionFilter : public CEventFilter {
	public:
		CExceptionFilter(const CDebugControl* owner, size_t idx, const std::string& text)
			: CEventFilter(owner, idx, text)
		{ }

		const std::string GetSecondCommand(void) const;
		void SetSecondCommand(const std::string& command) const;

		virtual BreakStatus GetStatus(void) const;
		virtual void SetStatus(BreakStatus status) const;

		bool IsHandled(void) const;
		void SetHandled(bool handled) const;

		static const object Repr(const CExceptionFilter& filter);
	};

	class CEvent : public DebugInterface {
		ULONG m_idx;
	public:
		CEvent(const CDebugControl* owner, size_t idx)
			: DebugInterface(owner->GetInterface()), m_idx(idx)
		{}

		ULONG GetIndex(void) const;
		const std::string GetName(void) const;

		const object Repr(const CEvent& evt) const;
	};

	class CStackFrame {
		DEBUG_STACK_FRAME m_frame;
	public:
		CStackFrame(const DEBUG_STACK_FRAME& frame)
			: m_frame(frame)
		{ }

		ULONG64 GetInstructionOffset(void) const;
		ULONG64 GetReturnOffset(void) const;
		ULONG64 GetFrameOffset(void) const;
		ULONG64 GetStackOffset(void) const;
		ULONG64 GetFuncTableEntry(void) const;
		const tuple GetParams(void) const;
		bool IsVirtual(void) const;
		ULONG GetFrameNumber(void) const;
	};
public:
	CDebugControl(IUnknown* control) : DebugInterface(control)
	{}

	static void Export(void);

	ULONG GetPageSize(void) const;
	bool IsPointer64Bit(void) const;

	const dict GetVersionInformation(void) const;
	void OutputVersionInformation(CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	const list GetEngineOptions(void) const;
	void SetEngineOptions(const list& options) const;
	void AddEngineOption(EngineOption option) const;
	void RemoveEngineOption(EngineOption option) const;

	ExecutionStatus GetExecutionStatus(void) const;
	void SetExecutionStatus(ExecutionStatus status) const;

	const tuple GetDebuggeeType(void) const;

	CodeLevel GetCodeLevel(void) const;
	void SetCodeLevel(CodeLevel level) const;
	NumberRadix GetRadix(void) const;
	void SetRadix(NumberRadix radix) const;

	const std::pair<ULONG, ULONG> GetSystemErrorControl(void) const;

	ErrorLevel GetSystemOutputLevel(void) const { return (ErrorLevel) GetSystemErrorControl().first; }
	ErrorLevel GetSystemBreakLevel(void) const { return (ErrorLevel) GetSystemErrorControl().second; }

	void SetSystemOutputLevel(ErrorLevel level) const;
	void SetSystemBreakLevel(ErrorLevel level) const;

	ProcessorType GetActualProcessorType(void) const;
	ProcessorType GetEffectiveProcessorType(void) const;
	void SetEffectiveProcessorType(ProcessorType type) const;
	ProcessorType GetExecutingProcessorType(void) const;

	object GetCurrentTimeDate(void) const;
	object GetCurrentSystemUpTime(void) const;

	const dict GetEventFilters(void) const;
	const dict GetExceptionFilters(void) const;

	const dict GetSupportedProcessorTypes(void) const;
	const dict GetPossibleExecutingProcessorTypes(void) const;

	const list GetBreakpoints(void) const;

	CDebugBreakpoint AddBreakpoint(CDebugBreakpoint::BreakpointType type=CDebugBreakpoint::BreakpointType::BREAKPOINT_CODE, ULONG id=DEBUG_ANY_ID);

	void Output(const std::string& text, CDebugOutput::OutputMask mask=CDebugOutput::OutputMask::MASK_NORMAL, CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	const list GetEvents(void) const;
	const CEvent GetCurrentEvent(void) const;

	ULONG WaitForEvent(ULONG timeout=INFINITE) const;

	bool HasInterrupt(void) const;
	void SetInterrupt(InterruptType type=InterruptType::INTR_ACTIVE) const;

	ULONG GetInterruptTimeout(void) const;
	void SetInterruptTimeout(ULONG seconds) const;

	ExpressionSyntax GetExpressionSyntax(void) const;
	void SetExpressionSyntax(ExpressionSyntax syntax) const;
	const dict GetExpressionSyntaxNames(void) const;
	void SetExpressionSyntaxByName(const std::string& abbrev) const;

	const object Evaluate(const std::string& expression, ULONG type = DEBUG_VALUE_INVALID) const;

	void Execute(const std::string& command, ExecuteFlag flags=ExecuteFlag::EXECUTE_DEFAULT, CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;
	void ExecuteCommandFile(const std::string& commandFile, ExecuteFlag flags=ExecuteFlag::EXECUTE_DEFAULT, CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	const list GetAssemblyOptions(void) const;
	void SetAssemblyOptions(const list& options) const;
	void AddAssemblyOption(AssemblyOption options) const;
	void RemoveAssemblyOption(AssemblyOption options) const;

	ULONG64 Assemble(ULONG64 offset, const std::string& instr) const;
	const tuple Disassemble(ULONG64 offset, DisassembleFlag flags=DisassembleFlag::DISASM_DEFAULT, size_t bufSize=4096) const;
	ULONG64 OutputDisassembly(ULONG64 offset, DisassembleFlag flags=DisassembleFlag::DISASM_DEFAULT, CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	ULONG64 GetDisassembleEffectiveOffset(void) const;
	ULONG64 GetNearInstruction(ULONG64 offset, LONG delta = 0) const;
	ULONG64 GetReturnOffset(void) const;

	const std::string Input(void) const;
	void ReturnInput(const std::string& input) const;

	const dict GetTextMacros(void) const;
	void SetTextMacroBySlot(ULONG slot, const std::string& macro) const;
	void SetTextMacroByName(const std::string& name, const std::string& macro) const;

	const dict GetTextReplacements(void) const;
	const std::string GetTextReplacement(const std::string& alias) const;
	void SetTextReplacement(const std::string& alias, const std::string& value) const;
	void RemoveTextReplacements(void) const;
	void OutputTextReplacements(CDebugControl::OutputControl target=CDebugControl::OutputControl::OUTPUT_ALL_CLIENTS) const;

	const list GetStackFrames(ULONG64 FrameOffset = 0, ULONG64 StackOffset = 0, ULONG64 InstructionOffset = 0) const;
};

/* FIXME
template <>
CDebugObject<IDebugControl>::operator IDebugControl*(void) const { return m_intf; }
*/

#endif
