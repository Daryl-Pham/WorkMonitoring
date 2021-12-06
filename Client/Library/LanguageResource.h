#pragma once

#include "LanguageResourceInterface.h"
#include <mutex>

class LanguageResource : public LanguageResourceInterface
{
public:
    static LanguageResourceInterface *Singleton();

    bool ReloadLanguageResource(LanguageID lang) override;
    bool GetString(UINT ids, std::wstring *dst) override;
    virtual ~LanguageResource() = default;

private:
    LanguageResource() = default;
    HMODULE m_LanguageModule;
    std::timed_mutex m_MutextForLanguageModule;
};
