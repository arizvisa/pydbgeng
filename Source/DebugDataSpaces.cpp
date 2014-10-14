#include "StdAfx.h"
#include "DebugDataSpaces.h"

#include <vector>

#include <rpc.h>

#include "DebugControl.h"

void CDebugDataSpaces::Export(void)
{
  enum_<CDebugDataSpaces::CacheType>("CacheType")
    .value("DEFAULT", CDebugDataSpaces::TYPE_DEFAULT)
    .value("CACHED", CDebugDataSpaces::TYPE_CACHED)
    .value("UNCACHED", CDebugDataSpaces::TYPE_UNCACHED)
    .value("WRITE_COMBINED", CDebugDataSpaces::TYPE_WRITE_COMBINED)
    ;

  enum_<CDebugDataSpaces::SearchFlag>("SearchFlag")
    .value("DEFAULT", CDebugDataSpaces::SEARCH_DEFAULT)
    .value("WRITABLE_ONLY", CDebugDataSpaces::SEARCH_WRITABLE_ONLY)
    ;

  enum_<CDebugDataSpaces::AddressType>("AddressType")
    .value("INVALID", CDebugDataSpaces::VSOURCE_INVALID)
    .value("DEBUGGEE", CDebugDataSpaces::VSOURCE_DEBUGGEE)
    .value("MAPPED_IMAGE", CDebugDataSpaces::VSOURCE_MAPPED_IMAGE)
    ;

  scope DebugDataSpaces = class_<CDebugDataSpaces>("DebugDataSpaces", no_init)
    .add_property("Virtual", &CDebugDataSpaces::GetVirtualDataSpace)
    .add_property("Physical", &CDebugDataSpaces::GetPhysicalDataSpace)
    .add_property("Msr", &CDebugDataSpaces::GetMsrDataSpace)
    .def("GetSystemBusDataSpace", &CDebugDataSpaces::GetSystemBusDataSpace)
    .def("GetControlDataSpace", &CDebugDataSpaces::GetControlDataSpace)
    .def("GetIoDataSpace", &CDebugDataSpaces::GetIoDataSpace)
    
    .def("GetTaggedData", &CDebugDataSpaces::GetTaggedData, 
      "the tagged data associated with a debugger session.")

    .def("VirtualToPhysical", &CDebugDataSpaces::VirtualToPhysical,
      "translates a location in the target's virtual address space into a physical memory address.")
    .def("GetTranslationPages", &CDebugDataSpaces::GetTranslationPages,
      "the physical addresses of the system paging structures at different levels of the paging hierarchy.")
    ;

  class_<CDebugDataSpaces::CHandle>("Handle", no_init)
    .add_property("Object", &CDebugDataSpaces::CHandle::GetObject)

    .add_property("TypeName", &CDebugDataSpaces::CHandle::GetTypeName,
      "The name of the object type.")
    .add_property("ObjectName", &CDebugDataSpaces::CHandle::GetObjectName,
      "The name of the object.")
    .add_property("Attributes", &CDebugDataSpaces::CHandle::GetAttributes,
      "A bit-set that contains the handle's attributes.")
    .add_property("GrantedAccess", &CDebugDataSpaces::CHandle::GetGrantedAccess,
      "A bit-set that specifies the access mask for the object that is represented by the handle.")
    .add_property("HandleCount", &CDebugDataSpaces::CHandle::GetHandleCount,
      "The number of handle references for the object.")
    .add_property("PointerCount", &CDebugDataSpaces::CHandle::GetPointerCount,
      "The number of pointer references for the object.")
    ;

  class_<CDebugDataSpaces::CDataSpace, boost::noncopyable>("DataSpace", no_init)
    .def("Read", pure_virtual(&CDebugDataSpaces::CDataSpace::Read), ("offset", "size", arg("type")=TYPE_DEFAULT),
      "reads memory from the target's address space.")
    .def("Write", pure_virtual(&CDebugDataSpaces::CDataSpace::Write), ("offset", "buffer", arg("type")=TYPE_DEFAULT),
      "writes data to the target's address space.")
    .def("Fill", pure_virtual(&CDebugDataSpaces::CDataSpace::Fill), ("offset", "size", "pattern"),
      "writes a pattern of bytes to the target's memory. The pattern is written repeatedly until the specified memory range is filled.")
    ;

  class_<CDebugDataSpaces::CVirtualDataSpace, bases<CDebugDataSpaces::CDataSpace> >("VirtualDataSpace", no_init)
    .def("ReadPointers", &CDebugDataSpaces::CVirtualDataSpace::ReadPointers, ("offset", arg("count")=1),
      "reading pointers from the target's virtual address space.")
    .def("WritePointers", &CDebugDataSpaces::CVirtualDataSpace::WritePointers, ("offset", "ptrs"),
      "writing pointers to the target's virtual address space.")

    .def("ReadMultiByteString", &CDebugDataSpaces::CVirtualDataSpace::ReadMultiByteString, 
      "reads a null-terminated, multibyte string from the target.")
    .def("ReadUnicodeString", &CDebugDataSpaces::CVirtualDataSpace::ReadUnicodeString,
      "reads a null-terminated, Unicode string from the target.")

    .def("Search", &CDebugDataSpaces::CVirtualDataSpace::Search, 
      ("offset", "length", "pattern", arg("flags")=SEARCH_DEFAULT, arg("granularity")=1),
      "searches the target's virtual memory for a specified pattern of bytes.")

    .def("GetOffsetInformation", &CDebugDataSpaces::CVirtualDataSpace::GetOffsetInformation,
      "provides general information about an address in a process's data space.")
    .def("GetValidRegion", &CDebugDataSpaces::CVirtualDataSpace::GetValidRegion,
      "locates the first valid region of memory in a specified memory range.")
    .def("GetNextDifferentlyValidOffset", &CDebugDataSpaces::CVirtualDataSpace::GetNextDifferentlyValidOffset,
      "returns the offset of the next address whose validity might be different from the validity of the specified address.")

    .def("ReadImageNtHeaders", &CDebugDataSpaces::CVirtualDataSpace::ReadImageNtHeaders,
      "returns the NT headers for the specified image loaded in the target.")
    ;

  class_<CDebugDataSpaces::CPhysicalDataSpace, bases<CDebugDataSpaces::CDataSpace> >("PhysicalDataSpace", no_init)
    ;

  class_<CDebugDataSpaces::CBusDataSpace, bases<CDebugDataSpaces::CDataSpace> >("BusDataSpace", no_init)
    .add_property("BusDataType", &CDebugDataSpaces::CBusDataSpace::GetBusDataType)
    .add_property("BusNumber", &CDebugDataSpaces::CBusDataSpace::GetBusNumber)
    .add_property("SlotNumber", &CDebugDataSpaces::CBusDataSpace::GetSlotNumber)
    ;

  class_<CDebugDataSpaces::CControlDataSpace, bases<CDebugDataSpaces::CDataSpace> >("ProcessorDataSpace", no_init)
    .add_property("Processor", &CDebugDataSpaces::CControlDataSpace::GetProcssor)

    .add_property("PCR", &CDebugDataSpaces::CControlDataSpace::GetPCR, 
      "the virtual address of the processor's Processor Control Region (PCR).")
    .add_property("PCB", &CDebugDataSpaces::CControlDataSpace::GetPCB,
      "the virtual address of the processor's Processor Control Block (PRCB).")
    .add_property("KTHREAD", &CDebugDataSpaces::CControlDataSpace::GetKTHREAD,
      "the virtual address of the KTHREAD structure for the system thread running on the processor.")
    .add_property("PageTable", &CDebugDataSpaces::CControlDataSpace::GetPageTable,
      "the virtual address of the base of the paging information owned by the operating system or the processor.")
    .add_property("Description", &CDebugDataSpaces::CControlDataSpace::GetProcessorDescription,
      "a description of the processor.")
    .add_property("Speed", &CDebugDataSpaces::CControlDataSpace::GetProcessorSpeed,
      "the speed of the processor in MHz.")
    ;

  class_<CDebugDataSpaces::CIoDataSpace, bases<CDebugDataSpaces::CDataSpace> >("IoDataSpace", no_init)
    .add_property("InterfaceType", &CDebugDataSpaces::CIoDataSpace::GetInterfaceType)
    .add_property("BusNumber", &CDebugDataSpaces::CIoDataSpace::GetBusNumber)
    .add_property("AddressSpace", &CDebugDataSpaces::CIoDataSpace::GetAddressSpace)
    ;

  class_<CDebugDataSpaces::CMsrDataSpace>("MsrDataSpace", no_init)
    .def("Read", &CDebugDataSpaces::CMsrDataSpace::Read, "reads a specified Model-Specific Register (MSR).")
    .def("Write", &CDebugDataSpaces::CMsrDataSpace::Write, "writes a value to the specified Model-Specific Register (MSR).")
    ;
  
  class_<CDebugDataSpaces::CTaggedDataSpace>("TaggedDataSpace", no_init)
    .add_property("Tag", &CDebugDataSpaces::CTaggedDataSpace::GetTagString)
    .add_property("Size", &CDebugDataSpaces::CTaggedDataSpace::GetSize)
    ;
}

const object CDebugDataSpaces::CVirtualDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
  if (TYPE_WRITE_COMBINED == type)
    RaiseException("The virtual memory don't support write-combined mode.", PyExc_NotImplementedError);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  if (TYPE_UNCACHED == type) {
    Check(m_intf->ReadVirtualUncached(offset, data, len, NULL));
  } else {
    Check(m_intf->ReadVirtual(offset, data, len, NULL));
  }
  return buffer;
}
ULONG CDebugDataSpaces::CVirtualDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
  if (TYPE_WRITE_COMBINED == type)
    RaiseException("The virtual memory don't support write-combined mode.", PyExc_NotImplementedError);

  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG written = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  if (TYPE_UNCACHED == type) {
    Check(m_intf->WriteVirtualUncached(offset, const_cast<LPVOID>(data), len, &written));
  } else {
    Check(m_intf->WriteVirtual(offset, const_cast<LPVOID>(data), len, &written));
  }
  return written;
}
ULONG CDebugDataSpaces::CVirtualDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG filled = 0;

  if (0 != ::PyObject_AsReadBuffer(pattern.ptr(), &data, &len))
    throw_error_already_set();

  Check(CComQIPtr<IDebugDataSpaces2>(m_intf)->FillVirtual(offset, size, const_cast<LPVOID>(data), len, &filled));

  return filled;
}

ULONG64 CDebugDataSpaces::CVirtualDataSpace::Search(ULONG64 offset, ULONG64 length, 
  const object& pattern, SearchFlag flags, ULONG granularity) const
{
  LPCVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsReadBuffer(pattern.ptr(), &data, &len))
    throw_error_already_set();

  ULONG64 match;

  if (SEARCH_DEFAULT == flags) {
    Check(m_intf->SearchVirtual(offset, length, const_cast<LPVOID>(data), len, granularity, &match));
  } else {
    Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->SearchVirtual2(offset, length, (ULONG) flags, const_cast<LPVOID>(data), len, granularity, &match));
  }
  return match;
}

const object CDebugDataSpaces::CPhysicalDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  if (TYPE_DEFAULT == type) {
    Check(m_intf->ReadPhysical(offset, data, len, NULL));
  } else {
    Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->ReadPhysical2(offset, (ULONG) type, data, len, NULL));
  }
  return buffer;
}
ULONG CDebugDataSpaces::CPhysicalDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG written = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  if (TYPE_DEFAULT == type) {
    Check(m_intf->WritePhysical(offset, const_cast<LPVOID>(data), len, &written));
  } else {
    Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->WritePhysical2(offset, 
      (ULONG) type, const_cast<LPVOID>(data), len, &written));
  }
  return written;
}
ULONG CDebugDataSpaces::CPhysicalDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG filled = 0;

  if (0 != ::PyObject_AsReadBuffer(pattern.ptr(), &data, &len))
    throw_error_already_set();

  Check(CComQIPtr<IDebugDataSpaces2>(m_intf)->FillPhysical(offset, size, const_cast<LPVOID>(data), len, &filled));

  return filled;
}

const list CDebugDataSpaces::CVirtualDataSpace::ReadPointers(ULONG64 offset, ULONG count) const
{
  std::vector<ULONG64> ptrs(count);

  Check(m_intf->ReadPointersVirtual(ptrs.size(), offset, &ptrs[0]));

  list result;

  for (size_t i=0; i<ptrs.size(); i++)
    result.append(ptrs[i]);

  return result;
}
void CDebugDataSpaces::CVirtualDataSpace::WritePointers(ULONG64 offset, const list& ptrs) const
{
  std::vector<ULONG64> buf(len(ptrs));

  for (size_t i=0; i<buf.size(); i++)
  {
    buf[i] = extract<ULONG64>(ptrs[i]);
  }

  Check(m_intf->WritePointersVirtual(buf.size(), offset, &buf[0]));
}

const std::string CDebugDataSpaces::CVirtualDataSpace::ReadMultiByteString(ULONG64 offset, ULONG maxBytes) const
{
  std::string str(maxBytes, '\0');
  ULONG len = 0;

  Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->ReadMultiByteStringVirtual(
    offset, str.size(), const_cast<PSTR>(str.c_str()), str.size(), &len));

  return str.substr(len-1);
}
const std::wstring CDebugDataSpaces::CVirtualDataSpace::ReadUnicodeString(ULONG64 offset, ULONG maxBytes) const
{
  std::wstring str(maxBytes/sizeof(wchar_t), L'\0');
  ULONG len = 0;

  Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->ReadUnicodeStringVirtualWide(
    offset, str.size()*sizeof(wchar_t), const_cast<PWSTR>(str.c_str()), str.size(), &len));

  return str.substr(len/sizeof(wchar_t)-1);
}

CDebugDataSpaces::AddressType CDebugDataSpaces::CVirtualDataSpace::GetOffsetInformation(ULONG64 offset) const
{
  ULONG type;

  Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->GetOffsetInformation(
    DEBUG_DATA_SPACE_VIRTUAL, DEBUG_OFFSINFO_VIRTUAL_SOURCE, offset, &type, sizeof(type), NULL));

  return static_cast<AddressType>(type);
}

const tuple CDebugDataSpaces::CVirtualDataSpace::GetValidRegion(ULONG64 base, ULONG size) const
{
  ULONG64 validBase;
  ULONG validSize;

  Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->GetValidRegionVirtual(base, size, &validBase, &validSize));

  return make_tuple(validBase, validSize);
}

ULONG64 CDebugDataSpaces::CVirtualDataSpace::GetNextDifferentlyValidOffset(ULONG64 offset) const
{
  ULONG64 next;

  Check(CComQIPtr<IDebugDataSpaces4>(m_intf)->GetNextDifferentlyValidOffsetVirtual(offset, &next));

  return next;
}

const object CDebugDataSpaces::CVirtualDataSpace::ReadImageNtHeaders(ULONG64 base) const
{
  object buffer(handle<>(::PyBuffer_New(sizeof(IMAGE_NT_HEADERS64))));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(CComQIPtr<IDebugDataSpaces3>(m_intf)->ReadImageNtHeaders(base, static_cast<PIMAGE_NT_HEADERS64>(data)));

  return buffer;
}

const object CDebugDataSpaces::CBusDataSpace::Read(ULONG offset, ULONG size, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The system bus data space don't support cache type.", PyExc_NotImplementedError);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->ReadBusData(m_uBusDataType, m_uBusNumber, m_uSlotNumber, offset, data, len, NULL));

  return buffer;
}
const object CDebugDataSpaces::CBusDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
    return this->Read((ULONG)offset, size, type);
}

ULONG CDebugDataSpaces::CBusDataSpace::Write(ULONG offset, const object& buffer, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The system bus data space don't support cache type.", PyExc_NotImplementedError);

  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG written = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->WriteBusData(m_uBusDataType, m_uBusNumber, m_uSlotNumber, offset, const_cast<LPVOID>(data), len, &written));

  return written;
}
ULONG CDebugDataSpaces::CBusDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
    return this->Write((ULONG)offset, buffer, type);
}
ULONG CDebugDataSpaces::CBusDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  RaiseException("The system bus don't support fill operation.", PyExc_NotImplementedError);

  return 0;
}

const object CDebugDataSpaces::CControlDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The processor data space don't support cache type.", PyExc_NotImplementedError);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->ReadControl(m_processor, offset, data, len, NULL));

  return buffer;
}
ULONG CDebugDataSpaces::CControlDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The processor data space don't support cache type.", PyExc_NotImplementedError);

  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG written = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->WriteControl(m_processor, offset, const_cast<LPVOID>(data), len, &written));

  return written;
}
ULONG CDebugDataSpaces::CControlDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  RaiseException("The processor data space don't support fill operation.", PyExc_NotImplementedError);

  return 0;
}

const dict CDebugDataSpaces::CControlDataSpace::GetProcessorDescription(void) const
{
  ULONG type;

  Check(CComQIPtr<IDebugControl>(m_intf)->GetActualProcessorType(&type));

  dict desc;

  desc["Type"] = static_cast<CDebugControl::ProcessorType>(type);

  DEBUG_PROCESSOR_IDENTIFICATION_ALL info = ReadSystemData<DEBUG_PROCESSOR_IDENTIFICATION_ALL>(DEBUG_DATA_PROCESSOR_IDENTIFICATION);

  switch(type)
  {
  case IMAGE_FILE_MACHINE_I386:
    {
      desc["Family"] = info.X86.Family;
      desc["Model"] = info.X86.Model;
      desc["Stepping"] = info.X86.Stepping;
      desc["VendorString"] = std::string(info.X86.VendorString);
      break;
    }
  case IMAGE_FILE_MACHINE_ARM:
    {
      desc["Model"] = info.Arm.Model;
      desc["Revision"] = info.Arm.Revision;
      desc["VendorString"] = info.Arm.VendorString;
      break;
    }
  case IMAGE_FILE_MACHINE_IA64:
    {
      desc["Model"] = info.Ia64.Model;
      desc["Revision"] = info.Ia64.Revision;
      desc["Family"] = info.Ia64.Family;      
      desc["ArchRev"] = info.Ia64.ArchRev;
      desc["VendorString"] = std::string(info.Ia64.VendorString);
      break;
    }
  case IMAGE_FILE_MACHINE_AMD64:
    {
      desc["Family"] = info.Amd64.Family;
      desc["Model"] = info.Amd64.Model;
      desc["Stepping"] = info.Amd64.Stepping;
      desc["VendorString"] = std::string(info.Amd64.VendorString);
      break;
    }
  case IMAGE_FILE_MACHINE_ALPHA:
    {
      desc["Type"] = info.Alpha.Type;
      desc["Revision"] = info.Alpha.Revision;
      break;
    }
  }

  return desc;
}

const object CDebugDataSpaces::CIoDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The I/O data space don't support cache type.", PyExc_NotImplementedError);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->ReadIo(m_uInterfaceType, m_uBusNumber, m_uAddressSpace, offset, data, len, NULL));

  return buffer;
}
ULONG CDebugDataSpaces::CIoDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The I/O data space don't support cache type.", PyExc_NotImplementedError);

  LPCVOID data = NULL;
  Py_ssize_t len = 0;
  ULONG written = 0;

  if (0 != ::PyObject_AsReadBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(m_intf->WriteIo(m_uInterfaceType, m_uBusNumber, m_uAddressSpace, offset, const_cast<LPVOID>(data), len, &written));

  return written;
}
ULONG CDebugDataSpaces::CIoDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  RaiseException("The I/O data space don't support fill operation.", PyExc_NotImplementedError);

  return 0;
}

ULONG64 CDebugDataSpaces::CMsrDataSpace::Read(ULONG msr) const
{
  ULONG64 value;

  Check(m_intf->ReadMsr(msr, &value));

  return value;
}
void CDebugDataSpaces::CMsrDataSpace::Write(ULONG msr, ULONG64 value) const
{
  Check(m_intf->WriteMsr(msr, value));
}

const object CDebugDataSpaces::CTaggedDataSpace::Read(ULONG offset, ULONG size, CacheType type) const
{
  if (TYPE_DEFAULT != type)
    RaiseException("The tagged data space don't support cache type.", PyExc_NotImplementedError);

  object buffer(handle<>(::PyBuffer_New(size)));

  LPVOID data = NULL;
  Py_ssize_t len = 0;

  if (0 != ::PyObject_AsWriteBuffer(buffer.ptr(), &data, &len))
    throw_error_already_set();

  Check(CComQIPtr<IDebugDataSpaces3>(m_intf)->ReadTagged(const_cast<LPGUID>(&m_tag), offset, data, len, NULL));

  return buffer;
}
const object CDebugDataSpaces::CTaggedDataSpace::Read(ULONG64 offset, ULONG size, CacheType type) const
{
    return this->Read((ULONG)offset, size, type);
}
ULONG CDebugDataSpaces::CTaggedDataSpace::Write(ULONG64 offset, const object& buffer, CacheType type) const
{
  RaiseException("The tagged data space don't support write operation.", PyExc_NotImplementedError);

  return 0;
}
ULONG CDebugDataSpaces::CTaggedDataSpace::Fill(ULONG64 offset, ULONG size, const object& pattern) const
{
  RaiseException("The tagged data space don't support fill operation.", PyExc_NotImplementedError);

  return 0;
}

const std::string CDebugDataSpaces::CTaggedDataSpace::GetTagString(void) const
{
  unsigned char *str;

  ::UuidToString(const_cast<GUID *>(&m_tag), &str);

  std::string result((LPSTR) str);

  ::RpcStringFree(&str);

  return result;
}

const DEBUG_HANDLE_DATA_BASIC CDebugDataSpaces::CHandle::GetBasicData(void) const
{
  DEBUG_HANDLE_DATA_BASIC data;

  Check(m_intf->ReadHandleData(m_object, DEBUG_HANDLE_DATA_TYPE_BASIC, &data, sizeof(data), NULL));

  return data;
}

const std::string CDebugDataSpaces::CHandle::GetTypeName(void) const
{
  char szName[256];
  ULONG nName = _countof(szName);

  Check(m_intf->ReadHandleData(m_object, DEBUG_HANDLE_DATA_TYPE_TYPE_NAME, szName, nName, &nName));

  return std::string(szName, nName-1);
}
const std::string CDebugDataSpaces::CHandle::GetObjectName(void) const
{
  char szName[1024];
  ULONG nName = _countof(szName);

  Check(m_intf->ReadHandleData(m_object, DEBUG_HANDLE_DATA_TYPE_OBJECT_NAME, szName, nName, &nName));

  return std::string(szName, nName-1);
}
ULONG CDebugDataSpaces::CHandle::GetHandleCount(void) const
{
  ULONG count;

  Check(m_intf->ReadHandleData(m_object, DEBUG_HANDLE_DATA_TYPE_HANDLE_COUNT, &count, sizeof(count), NULL));

  return count;
}

ULONG64 CDebugDataSpaces::VirtualToPhysical(ULONG64 address) const
{
  ULONG64 physical;

  Check(CComQIPtr<IDebugDataSpaces2>(m_intf)->VirtualToPhysical(address, &physical));

  return physical;
}

const list CDebugDataSpaces::GetTranslationPages(ULONG64 address) const
{
  static const size_t MAX_PAGE_DEPTH = 4;

  ULONG64 offsets[MAX_PAGE_DEPTH];
  ULONG levels = _countof(offsets);

  Check(CComQIPtr<IDebugDataSpaces2>(m_intf)->GetVirtualTranslationPhysicalOffsets(
    address, offsets, levels, &levels));

  list result;

  for (ULONG i=0; i<levels; i++)
    result.append(offsets[i]);

  return result;
}

const dict CDebugDataSpaces::GetTaggedData(void) const
{
  CComQIPtr<IDebugDataSpaces3> intf(m_intf);

  ULONG64 handle;

  Check(intf->StartEnumTagged(&handle));

  GUID tag;
  ULONG size;

  dict tags;

  while (S_OK == intf->GetNextTagged(handle, &tag, &size))
  {
    CTaggedDataSpace data(this, tag, size);

    tags[data.GetTagString()] = data;
  }

  Check(intf->EndEnumTagged(handle));

  return tags;
}