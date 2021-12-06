#include <memory>
#include "stdafx.h"
#include "Log.h"
#include "LanguageResource.h"


namespace
{

DEFINE_LOGGER_FILE_NAME;
const wchar_t languageEn[] = L"LanguageEn.dll";
const wchar_t languageJa[] = L"LanguageJa.dll";

std::unique_ptr<LanguageResource> singleton = nullptr;

}

LanguageResourceInterface *LanguageResource::Singleton()
{
    if (singleton == nullptr)
    {
        singleton = std::unique_ptr<LanguageResource>(new LanguageResource());
    }

    return singleton.get();
}
bool LanguageResource::ReloadLanguageResource(LanguageID lang)
{
    std::lock_guard<std::timed_mutex> lock(m_MutextForLanguageModule);

    if (m_LanguageModule)
    {
        FreeLibrary(m_LanguageModule);
        m_LanguageModule = NULL;
    }

    const wchar_t *ln;
    switch (lang)
    {
    case LanguageIDEnglish:
        ln = languageEn;
        break;
    case LanguageIDJapanese:
        ln = languageJa;
        break;
    default:
        ln = languageEn;
        break;
    }

    m_LanguageModule = LoadLibrary(ln);
    if (m_LanguageModule == NULL)
    {
        LOG_WARN("LoadLibrary(%S) failed. %lu", ln, GetLastError());
        return false;
    }

    return true;
}

bool LanguageResource::GetString(UINT ids, std::wstring *dst)
{
    std::lock_guard<std::timed_mutex> lock(m_MutextForLanguageModule);

    if (!m_LanguageModule)
    {
        LOG_WARN("The languageModule is null. Singleton() should be called.");
        return false;
    }
    wchar_t *tmp = NULL;
    int size = LoadString(m_LanguageModule, ids, reinterpret_cast<LPWSTR>(&tmp), 0);

    if (!size)
    {
        LOG_WARN("LoadString(%u) failed. %lu", ids, GetLastError());
        return false;
    }

    *dst = std::wstring(tmp, size);
    return true;
}
