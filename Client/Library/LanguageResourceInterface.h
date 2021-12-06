#pragma once

#include <string>

enum LanguageID {
    /**
     * For selection language by system preference.
     */
    LanguageIDDefault,

    /**
     * In English.
     */
    LanguageIDEnglish,

    /**
     * In Japnese.
     */
    LanguageIDJapanese,
};

class LanguageResourceInterface {
public:
    /** Reload specified language resource.
     * @param lang can be specified language.
     * @return true if succeeded to load language resource. false if failed to load language resource.
     */
    virtual bool ReloadLanguageResource(LanguageID lang = LanguageIDDefault) = 0;

    /** Get string from lanaugage resource.
     * @param ids ID of string.
     * @param dst content of ids.
     * @param true if succeeded to get language resource. false if failed to get language resource.
     */
    virtual bool GetString(UINT ids, std::wstring *dst) = 0;

    virtual ~LanguageResourceInterface() = default;
};
