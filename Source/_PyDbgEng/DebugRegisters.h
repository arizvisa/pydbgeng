#ifndef __DebugRegisters_h
#define __DebugRegisters_h
#pragma once

#include <string>
#include <vector>

#include <boost/python.hpp>
#include <dbgeng.h>

using namespace boost::python;

/** Class declarations **/
class CDebugRegisters : public CDebugObject<IDebugRegisters>
{
public:
	enum class RegisterSource : ULONG {
		DEBUGGEE = DEBUG_REGSRC_DEBUGGEE,
		EXPLICIT = DEBUG_REGSRC_EXPLICIT,
		FRAME = DEBUG_REGSRC_FRAME,
	};

	class CAbstractRegister : public DebugInterface
	{
	protected:
		size_t m_idx;
		std::string m_name;

		CAbstractRegister(const CDebugRegisters *owner, size_t idx, const std::string& name)
			: DebugInterface(owner->GetInterface()), m_idx(idx), m_name(name)
		{ }

		virtual void GetValue(DEBUG_VALUE& value) const = 0;
		virtual void SetValue(DEBUG_VALUE& value) const = 0;

	public:
		size_t GetIndex(void) const { return m_idx; }
		const std::string GetName(void) const { return m_name; }

		const object GetValue(void) const;
		void SetValue(const object buffer) const;

		operator long() const;
		operator double() const;
	};

	class CDebugRegister : public CAbstractRegister
	{
		utils::ValueType m_type;

		size_t m_master, m_length, m_shift;
		ULONG64 m_mask;

		friend class CDebugRegisters;
	public:
		CDebugRegister(const CDebugRegisters *owner, size_t idx, const std::string& name, utils::ValueType type)
			: CAbstractRegister(owner, idx, name),
				m_type(type), m_master(0), m_length(0), m_shift(0), m_mask(0)
		{ }

		utils::ValueType GetType(void) const { return m_type; }

		size_t GetSubregMaster(void) const { return m_master; }
		size_t GetSubregLength(void) const { return m_length; }
		ULONG64 GetSubregMask(void) const { return m_mask; }
		size_t GetSubregShift(void) const { return m_shift; }

		virtual void GetValue(DEBUG_VALUE& value) const { Check(m_intf->GetValue(m_idx, &value)); }
		virtual void SetValue(DEBUG_VALUE& value) const { Check(m_intf->SetValue(m_idx, &value)); }
	};

	class CPseudoRegister : public CAbstractRegister
	{
		ULONG64 m_module;
		ULONG m_id;
	public:
		CPseudoRegister(const CDebugRegisters *owner, size_t idx, const std::string& name, ULONG64 module, ULONG id)
			: CAbstractRegister(owner, idx, name), m_module(module), m_id(id)
		{ }

		ULONG64 GetTypeModule(void) const { return m_module; }
		ULONG GetTypeId(void) const { return m_id; }

		virtual void GetValue(DEBUG_VALUE& value) const {
			CComQIPtr<IDebugRegisters2> regs(m_intf);
			Check(regs->GetPseudoValues(DEBUG_REGSRC_DEBUGGEE, 1, NULL, m_idx, &value));
		}
		virtual void SetValue(DEBUG_VALUE& value) const {
			CComQIPtr<IDebugRegisters2> regs(m_intf);
			Check(regs->SetPseudoValues(DEBUG_REGSRC_DEBUGGEE, 1, NULL, m_idx, &value));
		}
	};
public:
	CDebugRegisters(IUnknown *registers) : DebugInterface(registers)
	{ }

	static void Export(void);

	const dict GetRegisters(void) const;
	const dict GetPseudos(void) const;

	ULONG64 GetStackOffset(void) const;
	ULONG64 GetFrameOffset(void) const;
	ULONG64 GetInstructionOffset(void) const;
};

#endif
