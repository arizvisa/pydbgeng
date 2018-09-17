#ifndef __FileVersion_h
#define __FileVersion_h
#pragma once

#include <string>

#include <Windows.h>
#pragma comment( lib, "Version.lib" )

namespace util
{
  struct VS_VERSIONINFO {
    WORD  wLength;
    WORD  wValueLength;
    WORD  wType;
    WCHAR szKey[1];
  };

  struct LANGANDCODEPAGE {
    WORD wLanguage;
    WORD wCodePage;
  };

  class CFileVersion
  {
    VS_VERSIONINFO *m_lpVerInfo;
    VS_FIXEDFILEINFO *m_lpFileInfo;

    LANGANDCODEPAGE m_defaultLangAndCP;

    void GetFileVerionInfo(const char *szFileName);
    void GuessTranslationId(void);

    static const std::string FormatVersionString(const ULARGE_INTEGER ver);
    const std::string GetStringValue(const char *szName) const;
  public:
    CFileVersion(HMODULE hModule = NULL);
    CFileVersion(const char *szFileName);
    ~CFileVersion(void);

    ULARGE_INTEGER GetFileVersionValue(void) const;
    const std::string GetFileVersionString(void) const { return FormatVersionString(GetFileVersionValue()); }
    ULARGE_INTEGER GetProductVersionValue(void) const;
    const std::string GetProductVersionString(void) const { return FormatVersionString(GetProductVersionValue()); }

    const std::string GetComments(void) const { return GetStringValue("Comments"); }
    const std::string GetCompanyName(void) const { return GetStringValue("CompanyName"); }
    const std::string GetFileDescription(void) const { return GetStringValue("FileDescription"); }
    const std::string GetFileVersion(void) const { return GetStringValue("FileVersion"); }
    const std::string GetInternalName(void) const { return GetStringValue("InternalName"); }
    const std::string GetLegalCopyright(void) const { return GetStringValue("LegalCopyright"); }
    const std::string GetLegalTrademarks(void) const { return GetStringValue("LegalTrademarks"); }
    const std::string GetOriginalFilename(void) const { return GetStringValue("OriginalFilename"); }
    const std::string GetProductName(void) const { return GetStringValue("ProductName"); }
    const std::string GetProductVersion(void) const { return GetStringValue("ProductVersion"); }
    const std::string GetPrivateBuild(void) const { return GetStringValue("PrivateBuild"); }
    const std::string GetSpecialBuild(void) const { return GetStringValue("SpecialBuild"); }
  };
}
#endif
