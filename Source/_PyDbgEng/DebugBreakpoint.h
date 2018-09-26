#ifndef __DebugBreakpoint_h
#define __DebugBreakpoint_h
#pragma once

class CDebugClient;

class CDebugBreakpoint : public CDebugObjectOwned<IDebugBreakpoint> {

public:
	enum class BreakpointType : ULONG {
		BREAKPOINT_CODE = DEBUG_BREAKPOINT_CODE,
		BREAKPOINT_DATA = DEBUG_BREAKPOINT_DATA,
		BREAKPOINT_TIME = DEBUG_BREAKPOINT_TIME,
	};

	enum class BreakpointFlag : ULONG {
		BREAKPOINT_GO_ONLY = DEBUG_BREAKPOINT_GO_ONLY,
		BREAKPOINT_DEFERRED = DEBUG_BREAKPOINT_DEFERRED,
		BREAKPOINT_ENABLED = DEBUG_BREAKPOINT_ENABLED,
		BREAKPOINT_ADDER_ONLY = DEBUG_BREAKPOINT_ADDER_ONLY,
		BREAKPOINT_ONE_SHOT = DEBUG_BREAKPOINT_ONE_SHOT,
	};

	enum class AccessType : ULONG {
		BREAK_READ = DEBUG_BREAK_READ,
		BREAK_WRITE = DEBUG_BREAK_WRITE,
		BREAK_READ_WRITE = DEBUG_BREAK_READ | DEBUG_BREAK_WRITE,
		BREAK_EXECUTE = DEBUG_BREAK_EXECUTE,
		BREAK_IO = DEBUG_BREAK_IO,
	};

public:
	void Export(void);

	CDebugBreakpoint(CDebugControl* const owner, size_t idx)
		: DebugInterface(owner)
	{
		Check(m_owner->GetBreakpointByIndex(idx, &m_intf));
	}

	CDebugBreakpoint(const CDebugControl* owner, size_t idx)
		: DebugInterface(owner)
	{
		Check(m_owner->GetBreakpointByIndex(idx, &m_intf));
	}

	CDebugBreakpoint(CDebugControl* const owner, IDebugBreakpoint* bp)
		: DebugInterface(owner, bp)
	{}

	CDebugBreakpoint(const CDebugControl* owner, IDebugBreakpoint* bp)
		: DebugInterface(owner, bp)
	{}

	bool operator ==(const CDebugBreakpoint& ctrl) const { return m_intf.p == ctrl.m_intf.p; }

	ULONG GetId(void) const;
	const tuple GetType(void) const;
	const CDebugClient& GetOwner(void) const;

	const list GetFlags(void) const;
	void SetFlags(list flags) const;
	void AddFlags(BreakpointFlag flags) const;
	void RemoveFlags(BreakpointFlag flags) const;

	ULONG64 GetOffset(void) const;
	void SetOffset(ULONG64 offset) const;
	const std::string GetOffsetExpression(void) const;
	void SetOffsetExpression(const std::string& expression) const;

	ULONG GetPassCount(void) const;
	void SetPassCount(ULONG count) const;
	ULONG GetCurrentPassCount(void) const;

	ULONG GetMatchThreadId(void) const;
	void SetMatchThreadId(ULONG id) const;

	const std::string GetCommand(void) const;
	void SetCommand(const std::string& command) const;

	const tuple GetDataParameters(void) const;
	void SetDataParameters(tuple params) const;

	static const object Repr(const CDebugBreakpoint& breakpoint);

	void Disable(void);
	void Enable(void);
	HRESULT Remove(void);
};

#endif
