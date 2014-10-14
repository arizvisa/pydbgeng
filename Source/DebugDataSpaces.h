#pragma once

#include "DebugObject.h"

class CDebugDataSpaces : public CDebugObject<IDebugDataSpaces>
{
  typedef CDebugObject<IDebugDataSpaces> __inherited;
public:
  class CHandle : public CDebugObject<IDebugDataSpaces2>
  {
    typedef CDebugObject<IDebugDataSpaces2> __inherited;

    ULONG64 m_object;

    const DEBUG_HANDLE_DATA_BASIC GetBasicData(void) const;
  public:
    CHandle(IUnknown *intf, ULONG64 object) 
      : __inherited(intf), m_object(object)
    {      
    }

    ULONG64 GetObject(void) const { return m_object; }

    const std::string GetTypeName(void) const;
    const std::string GetObjectName(void) const;
    ULONG GetAttributes(void) const { return GetBasicData().Attributes; }
    ULONG GetGrantedAccess(void) const { return GetBasicData().GrantedAccess; }
    ULONG GetHandleCount(void) const;
    ULONG GetPointerCount(void) const { return GetBasicData().PointerCount; }

    static const object Repr(const CHandle& handle)
    {
      return "(Handle %08x %s:%s)" % make_tuple(handle.GetObject(), 
        handle.GetTypeName().c_str(), handle.GetObjectName().c_str());
    }
  };

  enum CacheType
  {
    TYPE_DEFAULT        = DEBUG_PHYSICAL_DEFAULT,
    TYPE_CACHED         = DEBUG_PHYSICAL_CACHED,
    TYPE_UNCACHED       = DEBUG_PHYSICAL_UNCACHED,
    TYPE_WRITE_COMBINED = DEBUG_PHYSICAL_WRITE_COMBINED 
  };

  enum SearchFlag
  {
    SEARCH_DEFAULT        = DEBUG_VSEARCH_DEFAULT,
    SEARCH_WRITABLE_ONLY  = DEBUG_VSEARCH_WRITABLE_ONLY
  };

  enum AddressType
  {
    VSOURCE_INVALID = DEBUG_VSOURCE_INVALID,
    VSOURCE_DEBUGGEE = DEBUG_VSOURCE_DEBUGGEE,
    VSOURCE_MAPPED_IMAGE = DEBUG_VSOURCE_MAPPED_IMAGE 
  };

  class CDataSpace : public __inherited
  {
  protected:
    CDataSpace(const CDebugDataSpaces *owner) 
      : __inherited(owner->GetInterface())
    {
    }
  public:
    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const = 0;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const = 0;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const = 0;
  };

  class CVirtualDataSpace : public CDataSpace
  {
  public:
    CVirtualDataSpace(const CDebugDataSpaces *owner) : CDataSpace(owner)
    {

    }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;

    const list ReadPointers(ULONG64 offset, ULONG count = 1) const;
    void WritePointers(ULONG64 offset, const list& ptrs) const;

    const std::string ReadMultiByteString(ULONG64 offset, ULONG maxBytes) const;
    const std::wstring ReadUnicodeString(ULONG64 offset, ULONG maxBytes) const;

    ULONG64 Search(ULONG64 offset, ULONG64 length, const object& pattern, 
      SearchFlag flags = SEARCH_DEFAULT, ULONG granularity = 1) const;    

    AddressType GetOffsetInformation(ULONG64 offset) const;
    const tuple GetValidRegion(ULONG64 base, ULONG size) const;
    ULONG64 GetNextDifferentlyValidOffset(ULONG64 offset) const;

    const object ReadImageNtHeaders(ULONG64 base) const;
  };

  class CPhysicalDataSpace : public CDataSpace
  {
  public:
    CPhysicalDataSpace(const CDebugDataSpaces *owner) : CDataSpace(owner)
    {

    }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;
  };

  class CBusDataSpace : public CDataSpace
  {
    ULONG m_uBusDataType, m_uBusNumber, m_uSlotNumber;
  public:
    CBusDataSpace(const CDebugDataSpaces *owner, ULONG BusDataType, ULONG BusNumber, ULONG SlotNumber) 
      : CDataSpace(owner), m_uBusDataType(BusDataType), m_uBusNumber(BusNumber), m_uSlotNumber(SlotNumber)
    {

    }

    ULONG GetBusDataType(void) const { return m_uBusDataType; }
    ULONG GetBusNumber(void) const { return m_uBusNumber; }
    ULONG GetSlotNumber(void) const { return m_uSlotNumber; }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;
    ULONG Write(ULONG offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    const object Read(ULONG offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
  };

  class CControlDataSpace : public CDataSpace
  {
    ULONG m_processor;

    template <typename T>
    T ReadSystemData(ULONG index) const
    {
      T value;

      Check(m_intf->ReadProcessorSystemData(m_processor, index, &value, sizeof(T), NULL));

      return value;
    }
  public:
    CControlDataSpace(const CDebugDataSpaces *owner, ULONG processor) 
      : CDataSpace(owner), m_processor(processor)
    {

    }

    ULONG GetProcssor(void) const { return m_processor; }

    ULONG64 GetPCR(void) const { return ReadSystemData<ULONG64>(DEBUG_DATA_KPCR_OFFSET); }
    ULONG64 GetPCB(void) const { return ReadSystemData<ULONG64>(DEBUG_DATA_KPRCB_OFFSET); }
    ULONG64 GetKTHREAD(void) const { return ReadSystemData<ULONG64>(DEBUG_DATA_KTHREAD_OFFSET); }
    ULONG64 GetPageTable(void) const { return ReadSystemData<ULONG64>(DEBUG_DATA_BASE_TRANSLATION_VIRTUAL_OFFSET); }
    const dict GetProcessorDescription(void) const;
    ULONG GetProcessorSpeed(void) const { return ReadSystemData<ULONG>(DEBUG_DATA_PROCESSOR_SPEED); }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;
  };

  class CIoDataSpace : public CDataSpace
  {
    ULONG m_uInterfaceType, m_uBusNumber, m_uAddressSpace;
  public:
    CIoDataSpace(const CDebugDataSpaces *owner, ULONG InterfaceType, ULONG BusNumber, ULONG AddressSpace) 
      : CDataSpace(owner), m_uInterfaceType(InterfaceType), m_uBusNumber(BusNumber), m_uAddressSpace(AddressSpace)
    {

    }

    ULONG GetInterfaceType(void) const { return m_uInterfaceType; }
    ULONG GetBusNumber(void) const { return m_uBusNumber; }
    ULONG GetAddressSpace(void) const { return m_uAddressSpace; }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;
  };

  class CMsrDataSpace : public __inherited
  {
  public:
    CMsrDataSpace(const CDebugDataSpaces *owner) 
      : __inherited(owner->GetInterface())
    {

    }

    ULONG64 Read(ULONG msr) const;
    void Write(ULONG msr, ULONG64 value) const;
  };

  class CTaggedDataSpace : public CDataSpace
  {
    GUID m_tag;
    ULONG m_size;
  public:
    CTaggedDataSpace(const CDebugDataSpaces *owner, const GUID& tag, ULONG size) 
      : CDataSpace(owner), m_tag(tag), m_size(size)
    {

    }

    const GUID GetTag(void) const { return m_tag; }
    const std::string GetTagString(void) const;
    ULONG GetSize(void) const { return m_size; }

    virtual const object Read(ULONG64 offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Write(ULONG64 offset, const object& buffer, CacheType type = TYPE_DEFAULT) const;
    virtual ULONG Fill(ULONG64 offset, ULONG size, const object& pattern) const;

    const object Read(ULONG offset, ULONG size, CacheType type = TYPE_DEFAULT) const;
  };
public:
  CDebugDataSpaces(IUnknown *intf) : __inherited(intf)
  {

  }

  static void Export(void);

  const CVirtualDataSpace GetVirtualDataSpace(void) const { return CVirtualDataSpace(this); }
  const CPhysicalDataSpace GetPhysicalDataSpace(void) const { return CPhysicalDataSpace(this); }
  const CBusDataSpace GetSystemBusDataSpace(ULONG BusDataType, ULONG BusNumber, ULONG SlotNumber) const { return CBusDataSpace(this, BusDataType, BusNumber, SlotNumber); }
  const CControlDataSpace GetControlDataSpace(ULONG processor) const { return CControlDataSpace(this, processor); }
  const CIoDataSpace GetIoDataSpace(ULONG InterfaceType, ULONG BusNumber, ULONG AddressSpace) const { return CIoDataSpace(this, InterfaceType, BusNumber, AddressSpace); }
  const CMsrDataSpace GetMsrDataSpace(void) const { return CMsrDataSpace(this); }
  const dict GetTaggedData(void) const;

  ULONG64 VirtualToPhysical(ULONG64 address) const;
  const list GetTranslationPages(ULONG64 address) const;
};

//template <> CDebugObject<IDebugDataSpaces>::operator IDebugDataSpaces*(void) const { return m_intf; }