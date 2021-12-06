#include "stdafx.h"
#include "gtest/gtest.h"
#include <LanguageResource.h>
#include <Language.h>

TEST(testLanguageResource, Singleton)
{
    LanguageResourceInterface *lang = LanguageResource::Singleton();
    ASSERT_NE(nullptr, lang);
    ASSERT_EQ(lang, LanguageResource::Singleton());
}

TEST(testLanguageResource, English)
{
    LanguageResourceInterface *lang = LanguageResource::Singleton();
    std::wstring languageString;

    EXPECT_TRUE(lang->ReloadLanguageResource(LanguageIDEnglish));
    EXPECT_TRUE(lang->GetString(IDS_LANGUAGE, &languageString));
    EXPECT_EQ(L"en", languageString);
}

TEST(testLanguageResource, Japanese)
{
    LanguageResourceInterface *lang = LanguageResource::Singleton();
    std::wstring languageString;

    EXPECT_TRUE(lang->ReloadLanguageResource(LanguageIDJapanese));
    EXPECT_TRUE(lang->GetString(IDS_LANGUAGE, &languageString));
    EXPECT_EQ(L"ja", languageString);
}
