#include "iwebbrowser2/browser_feature.h"

#include <windows.h>
#include <shlwapi.h>

#include <assert.h>

#include <string>
#include <vector>

namespace ie {

namespace {

const wchar_t kFeaturePath[] = L"Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\";

std::vector<std::string> StringSplit(const std::string &text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

LSTATUS GetStringFromReg(HKEY key,
                         const std::string& key_name,
                         std::string* value) {
  assert(value);
  value->clear();

  char buffer[512];
  DWORD buffer_size = sizeof(buffer);
  LSTATUS status = RegQueryValueExA(key, key_name.c_str(), 0, NULL,
                                    (LPBYTE)buffer, &buffer_size);
  if (ERROR_SUCCESS == status) {
    *value = buffer;
  }
  return status;
}

void SetBrowserFeature(const std::wstring& feature,
                       const wchar_t* exe_name,
                       DWORD value) {
  std::wstring path = kFeaturePath + feature;
  HKEY hkey;
  LSTATUS status =
      RegCreateKeyEx(HKEY_CURRENT_USER, path.c_str(), 0, NULL,
                     REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
  if (status != ERROR_SUCCESS) {
    return;
  }

  RegSetValueEx(hkey, exe_name, 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
  RegCloseKey(hkey);
}

void DeleteBrowserFeatureKey(const std::wstring& feature,
                             const wchar_t* exe_name) {
  assert(exe_name);

  std::wstring path = kFeaturePath + feature;
  HKEY hkey;
  LSTATUS status = RegOpenKey(HKEY_CURRENT_USER, path.c_str(), &hkey);
  if (status != ERROR_SUCCESS) {
    return;
  }

  RegDeleteValue(hkey, exe_name);
  RegCloseKey(hkey);
}

DWORD GetLatestIEVersion() {
  const wchar_t kPath[] = L"SOFTWARE\\Microsoft\\Internet Explorer";

  HKEY hkey;
  LSTATUS status = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, kPath, 0, KEY_QUERY_VALUE, &hkey);
  if (status != ERROR_SUCCESS) {
    return 0;
  }

  DWORD result = 0;
  do {
    std::string ie_version;
    status = GetStringFromReg(hkey, "svcVersion", &ie_version);
    if (status != ERROR_SUCCESS) {
      status = GetStringFromReg(hkey, "version", &ie_version);
    }
    if (status != ERROR_SUCCESS || ie_version.empty()) {
      break;
    }

    std::vector<std::string> version_vec = StringSplit(ie_version, '.');
    if (version_vec.empty()) {
      break;
    }

    int major_version = std::atoi(version_vec[0].c_str());
    switch (major_version) {
      case 6:
        result = 6000;
        break;
      case 7:
        result = 7000;
        break;
      case 8:
        result = 8000;
        break;
      case 9:
        result = 9000;
        break;
      case 10:
        result = 10000;
        break;
      case 11:
        result = 11000;
        break;
      default:
        break;
    }
  } while (false);

  RegCloseKey(hkey);
  return result;
}

}  // namespace

void SetIEFeature() {
  DWORD ie_version = GetLatestIEVersion();
  if (ie_version == 0) {
    return;
  }

  wchar_t exe_path[MAX_PATH] = { L'\0' };
  if (GetModuleFileName(NULL, exe_path, MAX_PATH - 1) == 0 ||
      exe_path[0] == L'\0') {
    return;
  }

  wchar_t* exe_name = PathFindFileName(exe_path);
  if (!exe_name) {
    return;
  }

  // 使用最新IE版本
  SetBrowserFeature(L"FEATURE_BROWSER_EMULATION", exe_name, ie_version);

  SetBrowserFeature(L"FEATURE_AJAX_CONNECTIONEVENTS", exe_name, 1);
  SetBrowserFeature(L"FEATURE_GPU_RENDERING", exe_name, 1);
  SetBrowserFeature(L"FEATURE_IVIEWOBJECTDRAW_DMLT9_WITH_GDI", exe_name, 1);
  SetBrowserFeature(L"FEATURE_NINPUT_LEGACYMODE", exe_name, 1);
  SetBrowserFeature(L"FEATURE_DISABLE_NAVIGATION_SOUNDS", exe_name, 1);
  SetBrowserFeature(L"FEATURE_SCRIPTURL_MITIGATION", exe_name, 1);
  SetBrowserFeature(L"FEATURE_SPELLCHECKING", exe_name, 0);
  SetBrowserFeature(L"FEATURE_STATUS_BAR_THROTTLING", exe_name, 1);
  SetBrowserFeature(L"FEATURE_VALIDATE_NAVIGATE_URL", exe_name, 1);
  SetBrowserFeature(L"FEATURE_WEBOC_DOCUMENT_ZOOM", exe_name, 0);
  SetBrowserFeature(L"FEATURE_WEBOC_POPUPMANAGEMENT", exe_name, 0);
  SetBrowserFeature(L"FEATURE_ADDON_MANAGEMENT", exe_name, 0);
  SetBrowserFeature(L"FEATURE_WEBSOCKET", exe_name, 1);
  SetBrowserFeature(L"FEATURE_WINDOW_RESTRICTIONS", exe_name, 0);
}

void RemoveIEFeature() {
  wchar_t module_filename[MAX_PATH];
  ZeroMemory(module_filename, MAX_PATH * sizeof(wchar_t));

  if (GetModuleFileName(NULL, module_filename, MAX_PATH) <= 0 ||
      module_filename[0] == L'\0') {
    return;
  }
  wchar_t* filename = ::PathFindFileName(module_filename);
  if (!filename) {
    return;
  }

  DeleteBrowserFeatureKey(L"FEATURE_ACTIVEX_REPURPOSEDETECTION", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ADDON_MANAGEMENT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_AJAX_CONNECTIONEVENTS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ALIGNED_TIMERS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ALLOW_HIGHFREQ_TIMERS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_BEHAVIORS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_BLOCK_CROSS_PROTOCOL_FILE_NAVIGATION",
                          filename);
  DeleteBrowserFeatureKey(L"FEATURE_BLOCK_LMZ_IMG", filename);
  DeleteBrowserFeatureKey(L"FEATURE_BLOCK_LMZ_OBJECT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_BLOCK_LMZ_SCRIPT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_BROWSER_EMULATION", filename);
  DeleteBrowserFeatureKey(L"FEATURE_DISABLE_ISO_2022_JP_SNIFFING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_DISABLE_ISO_2022_JP_SNIFFING_V2", filename);
  DeleteBrowserFeatureKey(L"FEATURE_DISABLE_MK_PROTOCOL", filename);
  DeleteBrowserFeatureKey(L"FEATURE_DISABLE_SQM_UPLOAD_FOR_APP", filename);
  DeleteBrowserFeatureKey(L"Feature_Enable_Compat_Logging", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ENABLE_DYNAMIC_OBJECT_CACHING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ENABLE_SCRIPT_PASTE_URLACTION_IF_PROMPT",
                          filename);
  DeleteBrowserFeatureKey(L"FEATURE_ENABLE_WEB_CONTROL_VISUALS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ENFORCE_BSTR", filename);
  DeleteBrowserFeatureKey(L"FEATURE_FEEDS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_FORCE_DISABLE_UNTRUSTEDPROTOCOL", filename);
  DeleteBrowserFeatureKey(L"FEATURE_GPU_RENDERING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_HIGH_RESOLUTION_AWARE", filename);
  DeleteBrowserFeatureKey(L"FEATURE_HTTP_USERNAME_PASSWORD_DISABLE", filename);
  DeleteBrowserFeatureKey(L"FEATURE_INTERNET_SHELL_FOLDERS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_IVIEWOBJECTDRAW_DMLT9_WITH_GDI", filename);
  DeleteBrowserFeatureKey(L"FEATURE_LAYOUT9_QUIRKS_EMULATION", filename);
  DeleteBrowserFeatureKey(L"FEATURE_LEGACY_DISPPARAMS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_LOCALMACHINE_LOCKDOWN", filename);
  DeleteBrowserFeatureKey(L"FEATURE_MAXCONNECTIONSPER1_0SERVER", filename);
  DeleteBrowserFeatureKey(L"FEATURE_MAXCONNECTIONSPERSERVER", filename);
  DeleteBrowserFeatureKey(L"FEATURE_MIME_HANDLING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_MIME_SNIFFING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_NINPUT_LEGACYMODE", filename);
  DeleteBrowserFeatureKey(L"FEATURE_OBJECT_CACHING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_PAINT_INSIDE_WMPAINT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_PRIVATE_FONT_SETTING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_PROTOCOL_LOCKDOWN", filename);
  DeleteBrowserFeatureKey(L"FEATURE_RESTRICT_ABOUT_PROTOCOL_IE7", filename);
  DeleteBrowserFeatureKey(L"FEATURE_SAFE_BINDTOOBJECT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_SCRIPTURL_MITIGATION", filename);
  DeleteBrowserFeatureKey(L"FEATURE_SECURITYBAND", filename);
  DeleteBrowserFeatureKey(L"FEATURE_SHOW_APP_PROTOCOL_WARN_DIALOG", filename);
  DeleteBrowserFeatureKey(L"FEATURE_TABBED_BROWSING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_USE_LEGACY_JSCRIPT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_USE_QME_FOR_TOPLEVEL_DOCS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_USE_SECURITY_THUNKS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_USE_WEBOC_OMNAVIGATOR_IMPLEMENTATION",
                          filename);
  DeleteBrowserFeatureKey(L"FEATURE_VALIDATE_NAVIGATE_URL", filename);
  DeleteBrowserFeatureKey(L"FEATURE_VIEWLINKEDWEBOC_IS_UNSAFE", filename);
  DeleteBrowserFeatureKey(L"FEATURE_WEBOC_DOCUMENT_ZOOM", filename);
  DeleteBrowserFeatureKey(L"FEATURE_WINDOW_RESTRICTIONS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_XSSFILTER", filename);
  DeleteBrowserFeatureKey(L"FEATURE_ZONE_ELEVATION", filename);
  DeleteBrowserFeatureKey(L"FEATURE_DISABLE_NAVIGATION_SOUNDS", filename);
  DeleteBrowserFeatureKey(L"FEATURE_SPELLCHECKING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_STATUS_BAR_THROTTLING", filename);
  DeleteBrowserFeatureKey(L"FEATURE_WEBOC_POPUPMANAGEMENT", filename);
  DeleteBrowserFeatureKey(L"FEATURE_WEBSOCKET", filename);
}

}  // namespace ie
