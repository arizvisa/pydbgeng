#include "StdAfx.h"
#include "DebugRegisters.h"

void CDebugRegisters::Export(void)
{
  enum_<CDebugRegisters::RegisterSource>("RegisterSource")
    .value("DEBUGGEE", CDebugRegisters::RegisterSource::DEBUGGEE)
    .value("EXPLICIT", CDebugRegisters::RegisterSource::EXPLICIT)
    .value("FRAME",    CDebugRegisters::RegisterSource::FRAME)
    ;

  scope DebugRegisters = class_<CDebugRegisters>("DebugRegisters", no_init)
    .add_property("Registers", &CDebugRegisters::GetRegisters)
    .add_property("Pseudos", &CDebugRegisters::GetPseudos)
    .add_property("StackOffset", &CDebugRegisters::GetStackOffset)
    .add_property("FrameOffset", &CDebugRegisters::GetFrameOffset)
    .add_property("InstructionOffset", &CDebugRegisters::GetInstructionOffset)
    ;

  class_<CDebugRegisters::CDebugRegister>("DebugRegister", no_init)
    .add_property("Index", &CDebugRegisters::CDebugRegister::GetIndex)
    .add_property("Name", &CDebugRegisters::CDebugRegister::GetName)
    .add_property("Type", &CDebugRegisters::CDebugRegister::GetType)
    .add_property("Value", &CDebugRegisters::CDebugRegister::GetValue, &CDebugRegisters::CDebugRegister::SetValue)
    .def(int_(self))
    .def(float_(self))
    ;

  implicitly_convertible<CDebugRegisters::CDebugRegister, int>();
  implicitly_convertible<CDebugRegisters::CDebugRegister, double>();

  class_<CDebugRegisters::CPseudoRegister>("PseudoRegister", no_init)
    .add_property("Index", &CDebugRegisters::CPseudoRegister::GetIndex)
    .add_property("Name", &CDebugRegisters::CPseudoRegister::GetName)
    .add_property("TypeModule", &CDebugRegisters::CPseudoRegister::GetTypeModule)
    .add_property("TypeId", &CDebugRegisters::CPseudoRegister::GetTypeId)
    .add_property("Value", &CDebugRegisters::CPseudoRegister::GetValue, &CDebugRegisters::CDebugRegister::SetValue)
    .def(int_(self))
    .def(float_(self))
    ;

  implicitly_convertible<CDebugRegisters::CPseudoRegister, int>();
  implicitly_convertible<CDebugRegisters::CPseudoRegister, double>();
}

CDebugRegisters::CAbstractRegister::operator long(void) const
{
  DEBUG_VALUE value;
  GetValue(value);

  switch (value.Type) {
  case DEBUG_VALUE_INT8:
    return value.I8;
  case DEBUG_VALUE_INT16:
    return value.I16;
  case DEBUG_VALUE_INT32:
    return value.I32;
  }

  utils::RaiseException("Cannot convert the value of register to int type.", PyExc_ValueError);
  return 0;
}

CDebugRegisters::CAbstractRegister::operator double() const
{
  DEBUG_VALUE value;
  GetValue(value);

  switch (value.Type) {
  case DEBUG_VALUE_INT8:
    return value.I8;
  case DEBUG_VALUE_INT16:
    return value.I16;
  case DEBUG_VALUE_INT32:
    return value.I32;
  case DEBUG_VALUE_INT64:
    return value.I64;
  case DEBUG_VALUE_FLOAT32:
    return value.F32;
  case DEBUG_VALUE_FLOAT64:
    return value.F64;
  }

  utils::RaiseException("Cannot convert the value of register to float type.", PyExc_ValueError);
  return 0;
}

const object CDebugRegisters::CAbstractRegister::GetValue(void) const
{
  DEBUG_VALUE value;
  GetValue(value);
  return utils::ToObject(value);
}
void CDebugRegisters::CAbstractRegister::SetValue(const object buffer) const
{
  DEBUG_VALUE value;

  GetValue(value);
  LPCVOID data = NULL; Py_ssize_t len = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  memcpy(&value, data, len);
  SetValue(value);
}

const dict CDebugRegisters::GetRegisters(void) const
{
  ULONG number = 0;
  Check(m_intf->GetNumberRegisters(&number));

  dict regs; char name[256];
  DEBUG_REGISTER_DESCRIPTION desc;

  for (size_t i=0; i<number; i++) {
    ULONG size = _countof(name);
    Check(m_intf->GetDescription(i, name, size, &size, &desc));

    CDebugRegister reg(this, i, std::string(name, size-1), (utils::ValueType) desc.Type);

    if (DEBUG_REGISTER_SUB_REGISTER == (desc.Flags & DEBUG_REGISTER_SUB_REGISTER)) {
      reg.m_master = desc.SubregMaster;
      reg.m_length = desc.SubregLength;
      reg.m_shift = desc.SubregShift;
      reg.m_mask = desc.SubregMask;
    }
    regs[reg.GetName()] = reg;
  }
  return regs;
}

const dict CDebugRegisters::GetPseudos(void) const
{
  CComQIPtr<IDebugRegisters2> regs2(m_intf);
  ULONG number = 0;

  Check(regs2->GetNumberPseudoRegisters(&number));

  dict regs;

  char name[256];
  ULONG64 module;
  ULONG id;

  for (size_t i=0; i<number; i++) {
    ULONG size = _countof(name);
    HRESULT hr = regs2->GetPseudoDescription(i, name, size, &size, &module, &id);

    if (SUCCEEDED(hr)) {
      CPseudoRegister reg(this, i, std::string(name, size-1), module, id);
      regs[reg.GetName()] = reg;
    }
  }
  return regs;
}

ULONG64 CDebugRegisters::GetStackOffset(void) const
{
  ULONG64 offset = 0;
  Check(m_intf->GetStackOffset(&offset));
  return offset;
}

ULONG64 CDebugRegisters::GetFrameOffset(void) const
{
  ULONG64 offset = 0;
  Check(m_intf->GetFrameOffset(&offset));
  return offset;
}

ULONG64 CDebugRegisters::GetInstructionOffset(void) const
{
  ULONG64 offset = 0;
  Check(m_intf->GetInstructionOffset(&offset));
  return offset;
}
