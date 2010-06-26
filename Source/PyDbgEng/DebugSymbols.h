#pragma once

#include <string>
#include <vector>

#include <DbgHelp.h>

#include "DebugObject.h"
#include "DebugControl.h"

class PYDBGENG_API CDebugSymbols 
  : public CDebugObject<IDebugSymbols>
{
  typedef CDebugObject<IDebugSymbols> __inherited;
public:
  static const size_t MAX_PATH_LENGTH = 4096;
  static const size_t MAX_NAME_LENGTH = 1024;
  
  enum SymbolOption
  {
    OPT_CASE_INSENSITIVE           = SYMOPT_CASE_INSENSITIVE,
    OPT_UNDNAME                    = SYMOPT_UNDNAME,
    OPT_DEFERRED_LOADS             = SYMOPT_DEFERRED_LOADS,
    OPT_NO_CPP                     = SYMOPT_NO_CPP,
    OPT_LOAD_LINES                 = SYMOPT_LOAD_LINES,
    OPT_OMAP_FIND_NEAREST          = SYMOPT_OMAP_FIND_NEAREST,
    OPT_LOAD_ANYTHING              = SYMOPT_LOAD_ANYTHING,
    OPT_IGNORE_CVREC               = SYMOPT_IGNORE_CVREC,
    OPT_NO_UNQUALIFIED_LOADS       = SYMOPT_NO_UNQUALIFIED_LOADS,
    OPT_FAIL_CRITICAL_ERRORS       = SYMOPT_FAIL_CRITICAL_ERRORS,
    OPT_EXACT_SYMBOLS              = SYMOPT_EXACT_SYMBOLS,
    OPT_ALLOW_ABSOLUTE_SYMBOLS     = SYMOPT_ALLOW_ABSOLUTE_SYMBOLS,
    OPT_IGNORE_NT_SYMPATH          = SYMOPT_IGNORE_NT_SYMPATH,
    OPT_INCLUDE_32BIT_MODULES      = SYMOPT_INCLUDE_32BIT_MODULES,
    OPT_PUBLICS_ONLY               = SYMOPT_PUBLICS_ONLY,
    OPT_NO_PUBLICS                 = SYMOPT_NO_PUBLICS,
    OPT_AUTO_PUBLICS               = SYMOPT_AUTO_PUBLICS,
    OPT_NO_IMAGE_SEARCH            = SYMOPT_NO_IMAGE_SEARCH,
    OPT_SECURE                     = SYMOPT_SECURE,
    OPT_NO_PROMPTS                 = SYMOPT_NO_PROMPTS,
    OPT_OVERWRITE                  = SYMOPT_OVERWRITE,
    OPT_IGNORE_IMAGEDIR            = SYMOPT_IGNORE_IMAGEDIR,
    OPT_FLAT_DIRECTORY             = SYMOPT_FLAT_DIRECTORY,
    OPT_FAVOR_COMPRESSED           = SYMOPT_FAVOR_COMPRESSED,
    OPT_ALLOW_ZERO_ADDRESS         = SYMOPT_ALLOW_ZERO_ADDRESS,
    OPT_DISABLE_SYMSRV_AUTODETECT  = SYMOPT_DISABLE_SYMSRV_AUTODETECT,
    OPT_DEBUG                      = SYMOPT_DEBUG                     
  };

  enum TypeOption
  {
    OPT_TYPE_DEFAULT       = 0,
    OPT_UNICODE_DISPLAY    = DEBUG_TYPEOPTS_UNICODE_DISPLAY,
    OPT_LONGSTATUS_DISPLAY = DEBUG_TYPEOPTS_LONGSTATUS_DISPLAY,
    OPT_FORCERADIX_OUTPUT  = DEBUG_TYPEOPTS_FORCERADIX_OUTPUT,
    OPT_MATCH_MAXSIZE      = DEBUG_TYPEOPTS_MATCH_MAXSIZE
  };

  class CType;

  class CModule : public __inherited
  {
    ULONG64 m_base;
    std::string m_imageName, m_moduleName, m_LoadedImageName;

    void Init(void); 

    const std::string GetModuleNameString(ULONG which) const;
  public:
    CModule(IUnknown *intf, ULONG idx, ULONG64 base = 0)
      : __inherited(intf), m_base(base)
    {
      if (!m_base)
        Check(m_intf->GetModuleByIndex(idx, &m_base));
    }

    ULONG64 GetBase(void) const { return m_base; }

    const std::string GetImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_IMAGE); }
    const std::string GetModuleName(void) const { return GetModuleNameString(DEBUG_MODNAME_MODULE); }
    const std::string GetLoadedImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_LOADED_IMAGE); }
    const std::string GetSymbolFileName(void) const { return GetModuleNameString(DEBUG_MODNAME_SYMBOL_FILE); }
    const std::string GetMappedImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_MAPPED_IMAGE); }

    const CType GetTypeByName(const std::string& name) const;

    void Reload(void) const { m_intf->Reload(GetModuleName().c_str()); }

    static const object Repr(const CModule& module) 
    {
      return "(Module %s @ %08x)" % make_tuple(module.GetModuleName().c_str(), module.GetBase());
    }
  };
 
  class CType : public __inherited
  {
    ULONG64 m_module;
    ULONG m_id;
  public:
    CType(IUnknown *intf, ULONG64 module, ULONG id)
      : __inherited(intf), m_module(module), m_id(id)
    {
    }

    const CModule GetModule(void) const { return CModule(m_intf, DEBUG_ANY_ID, m_module); }
    ULONG GetId(void) const { return m_id; }

    const std::string GetName(void) const;
    ULONG GetSize(void) const;

    const dict GetFields(void) const;

    const object ReadPhysical(ULONG64 offset) const;
    ULONG WritePhysical(ULONG64 offset, const object buffer) const;

    const object ReadVirtual(ULONG64 offset) const;
    ULONG WriteVirtual(ULONG64 offset, const object buffer) const;

    void OutputPhysical(ULONG64 offset, TypeOption options = OPT_TYPE_DEFAULT, OutputTarget target = TARGET_ALL_CLIENTS);
    void OutputVirtual(ULONG64 offset, TypeOption options = OPT_TYPE_DEFAULT, OutputTarget target = TARGET_ALL_CLIENTS);

    static const object Repr(const CType& type) 
    {
      return "(Type %s!%s)" % make_tuple(type.GetModule().GetModuleName().c_str(), type.GetName().c_str());
    }
  };

  class CSymbol : public CDebugObject<IDebugSymbols3>
  {
    typedef CDebugObject<IDebugSymbols3> __inherited;

    DEBUG_SYMBOL_ENTRY m_entry;
  public:
    CSymbol(IUnknown *intf, DEBUG_MODULE_AND_ID& id)
      : __inherited(intf)
    {
      Check(m_intf->GetSymbolEntryInformation(&id, &m_entry));
    }

    const std::string GetName(void) const;

    const CModule GetModule(void) const { return CModule(m_intf, DEBUG_ANY_ID, m_entry.ModuleBase); }
    ULONG64 GetModuleBase(void) const { return m_entry.ModuleBase; }
    ULONG64 GetId(void) const { return m_entry.Id; }

    ULONG64 GetOffset(void) const { return m_entry.Offset; }
    ULONG GetSize(void) const { return m_entry.Size; }
    ULONG GetTypeId(void) const { return m_entry.TypeId; }    
    ULONG GetToken(void) const { return m_entry.Token; }
    ULONG GetTag(void) const { return m_entry.Tag; }
    ULONG GetArg32(void) const { return m_entry.Arg32; }
    ULONG64 GetArg64(void) const { return m_entry.Arg64; }

    static const object Repr(const CSymbol& symbol) 
    {
      return "(Symbol %s!%s)" % make_tuple(symbol.GetModule().GetModuleName().c_str(), symbol.GetName().c_str());
    }
  };

  struct SymbolMatcher
  {
    CComQIPtr<IDebugSymbols> m_intf;
    ULONG64 m_handle;

    SymbolMatcher(IDebugSymbols * intf, const std::string& pattern)
      : m_intf(intf), m_handle(0)
    {
      Check(m_intf->StartSymbolMatch(const_cast<PSTR>(pattern.c_str()), &m_handle));
    }
    ~SymbolMatcher()
    {
      if (m_handle) Check(m_intf->EndSymbolMatch(m_handle));
    }

    bool Next(std::string& name, ULONG64& offset)
    {
      char szName[MAX_NAME_LENGTH];
      ULONG size = _countof(szName);

      HRESULT hr = m_intf->GetNextSymbolMatch(m_handle, szName, _countof(szName), &size, &offset);

      if (SUCCEEDED(hr))
      {
        name = std::string(szName, size-1);
      }
      else
      {
        name.clear();
        offset = 0;
      }

      return S_OK == hr;
    }
  };
public:
  CDebugSymbols(IUnknown *intf) : __inherited(intf)
  {
  }

  static void Export(void);

  const list GetSymbolOptions(void) const;
  void SetSymbolOptions(const list options) const;
  void AddSymbolOptions(SymbolOption option) const;
  void RemoveSymbolOptions(SymbolOption option) const;

  const list GetTypeOptions(void) const;
  void SetTypeOptions(const list options) const;
  void AddTypeOptions(TypeOption option) const;
  void RemoveTypeOptions(TypeOption option) const;

  const std::string GetImagePath(void) const;
  void SetImagePath(const std::string& path) const;
  void AppendImagePath(const std::string& path) const;

  const std::string GetSymbolPath(void) const;
  void SetSymbolPath(const std::string& path) const;
  void AppendSymbolPath(const std::string& path) const;

  const std::string GetSourcePath(void) const;
  void SetSourcePath(const std::string& path) const;
  void AppendSourcePath(const std::string& path) const;

  const dict GetLoadedModules(void) const;
  const dict GetUnloadedModules(void) const;

  const CModule GetModuleByName(const std::string& name, ULONG start = 0) const;
  const CModule GetModuleByOffset(ULONG64 offset, ULONG start = 0) const;

  const CType GetTypeByName(const std::string& name) const;
  const CType GetTypeByOffset(ULONG64 offset) const;

  ULONG64 GetOffsetByName(const std::string& name) const;
  const tuple GetNameByOffset(ULONG64 offset, LONG delta = 0) const;

  const dict GetSymbolOffsets(const std::string& pattern) const;
  const dict GetSymbolsByName(const std::string& pattern) const;
  const dict GetSymbolsByOffset(ULONG64 offset) const;
};

template <>
CDebugObject<IDebugSymbols>::operator IDebugSymbols*(void) const { return m_intf; }