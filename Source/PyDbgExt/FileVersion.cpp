#include "StdAfx.h"
#include "FileVersion.h"

namespace util
{
  CFileVersion::CFileVersion(HMODULE hModule) : m_lpVerInfo(NULL), m_lpFileInfo(NULL)
  {
    char szFileName[MAX_PATH];

    szFileName[::GetModuleFileName(hModule, szFileName, _countof(szFileName)-1)] = 0;

    GetFileVerionInfo(szFileName);
    GuessTranslationId();
  }

  CFileVersion::CFileVersion(const char *szFileName) : m_lpVerInfo(NULL), m_lpFileInfo(NULL)
  {
    GetFileVerionInfo(szFileName);
    GuessTranslationId();
  }

  void CFileVersion::GetFileVerionInfo(const char *szFileName)
  {
    DWORD dwHandle, dwVerInfoSize = ::GetFileVersionInfoSize(szFileName, &dwHandle);  
    
    m_lpVerInfo = (VS_VERSIONINFO *) new char[dwVerInfoSize];

    if (::GetFileVersionInfo(szFileName, dwHandle, dwVerInfoSize, m_lpVerInfo))
    {
      UINT dwSize = 0;

      if (!::VerQueryValue(m_lpVerInfo, "\\", (LPVOID *) &m_lpFileInfo, &dwSize))
      {
        m_lpFileInfo = NULL;
      }
    }
    else
    {
      delete m_lpVerInfo;
      m_lpVerInfo = NULL;
    }
  }

  void CFileVersion::GuessTranslationId(void)
  {
    LANGANDCODEPAGE *lpTranslate = NULL;

    UINT dwSize = 0;

    if (::VerQueryValue(m_lpVerInfo, "\\VarFileInfo\\Translation", (LPVOID *) &lpTranslate, &dwSize))
    {
      static LANGID LANGID_USER = ::GetUserDefaultLangID(), 
        LANGID_SYSTEM = ::GetSystemDefaultLangID(),
        LANGID_NEUTRAL = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        LANGID_ENGLISH = MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL);

      enum MatchLevel
      {
        Null,
        First,
        EnglishMajorLanguage,
        EnglishLanguage,
        NeutralMajorLanguage,
        NeutralLanguage,
        SystemMajorLanguage,
        SystemLanguage,
        UserMajorLanguage,
        UserLanguage
      } level = Null;

      for (size_t i=0; i<dwSize/sizeof(LANGANDCODEPAGE); i++)
      {
        MatchLevel curLevel = First;

        if (lpTranslate[i].wLanguage == LANGID_USER)
        {
          curLevel = UserLanguage;
        }
        else if (PRIMARYLANGID(lpTranslate[i].wLanguage) == PRIMARYLANGID(LANGID_USER))
        {
          curLevel = UserMajorLanguage;
        }
        else if (lpTranslate[i].wLanguage == LANGID_SYSTEM)
        {
          curLevel = SystemLanguage;
        }
        else if (PRIMARYLANGID(lpTranslate[i].wLanguage) == PRIMARYLANGID(LANGID_SYSTEM))
        {
          curLevel = SystemMajorLanguage;
        }
        else if (lpTranslate[i].wLanguage == LANGID_NEUTRAL)
        {
          curLevel = NeutralLanguage;
        }
        else if (PRIMARYLANGID(lpTranslate[i].wLanguage) == PRIMARYLANGID(LANGID_NEUTRAL))
        {
          curLevel = NeutralMajorLanguage;
        }
        else if (lpTranslate[i].wLanguage == LANGID_ENGLISH)
        {
          curLevel = EnglishLanguage;
        }
        else if (PRIMARYLANGID(lpTranslate[i].wLanguage) == PRIMARYLANGID(LANGID_ENGLISH))
        {
          curLevel = EnglishMajorLanguage;
        }

        if (curLevel > level)
        {
          m_defaultLangAndCP = lpTranslate[i];
        }
      }
    }
  }

  CFileVersion::~CFileVersion(void)
  {
    if (m_lpVerInfo) delete m_lpVerInfo;
  }

  ULARGE_INTEGER CFileVersion::GetFileVersionValue(void) const
  {
    ULARGE_INTEGER ver;

    ver.HighPart = m_lpFileInfo->dwFileVersionMS;
    ver.LowPart = m_lpFileInfo->dwFileVersionLS;

    return ver;
  }

  const std::string CFileVersion::FormatVersionString(const ULARGE_INTEGER ver) 
  {
    char buf[64];

    _snprintf_s(buf, sizeof(buf), _countof(buf)-1, "%d.%d.%d.%d", 
      ver.HighPart >> 16, ver.HighPart & 0xFFFF,
      ver.LowPart >> 16, ver.LowPart & 0xFFFF);

    return buf;
  }

  ULARGE_INTEGER CFileVersion::GetProductVersionValue(void) const
  {
    ULARGE_INTEGER ver;

    ver.HighPart = m_lpFileInfo->dwProductVersionMS;
    ver.LowPart = m_lpFileInfo->dwProductVersionLS;

    return ver;
  }

  const std::string CFileVersion::GetStringValue(const char *szName) const
  {
    char szSubBlock[1024];

    _snprintf_s(szSubBlock, sizeof(szSubBlock), _countof(szSubBlock)-1, "\\StringFileInfo\\%04X%04X\\%s",
      m_defaultLangAndCP.wLanguage, m_defaultLangAndCP.wCodePage, szName);

    char *szValue = NULL;
    UINT nSize = 0;

    if (::VerQueryValue(m_lpVerInfo, szSubBlock, (LPVOID *) &szValue, &nSize))
      return std::string(szValue, nSize-1);

    return "";
  }
} // namespace util