#include "pch.h"

#include <mutex>
#include <thread>
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <BizTeleworkOnDutyDialog.h>
#include <BizTeleworkDialogDependency.h>
#include <Language.h>
#include <Windows.h>
#include "WinAppForTest.h"
#include "Mock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Eq;

TEST(testBizTeleworkOnDutyDialog_NotRunOnCI, Constructor)
{
    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_BUTTON_END_WORK)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_STATIC_OPERATING_TIME_LABEL)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_TWENTY_FOUR_HOURS_OR_MORE)), _))
        .WillOnce(Return(true));

    MockBizTeleworkDialogDependency dependency;
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));

    WinAppForTest app([&dependency]()
        {
            auto mw = std::make_unique<BizTeleworkOnDutyDialog>(dependency, 0, nullptr);
            return mw;
        }, true);

    auto timeoutThread = std::thread([&app]()
        {
            Sleep(1000);
            ::PostMessage(app.m_pMainWnd->GetSafeHwnd(), WM_CLOSE, 0, 0);
        });
    timeoutThread.detach();
    ::AfxWinMain(GetModuleHandle(NULL), NULL, 0, 0);
}

TEST(testBizTeleworkOnDutyDialog, GetOperatingTimeContentType)
{
    MockBizTeleworkDialogDependency dependency;
    std::unique_ptr<BizTeleworkOnDutyDialog> mw = std::make_unique<BizTeleworkOnDutyDialog>(dependency, 0, nullptr);
    TypeOperatingContent type = TypeOperatingContent::NORMAL_TYPE;
    type = mw->GetOperatingTimeContentType(-38);
    EXPECT_EQ(TypeOperatingContent::PAST_TYPE, type);

    type = mw->GetOperatingTimeContentType(38);
    EXPECT_EQ(TypeOperatingContent::NORMAL_TYPE, type);

    type = mw->GetOperatingTimeContentType(86400);
    EXPECT_EQ(TypeOperatingContent::TWENTY_FOUR_TYPE, type);

    type = TypeOperatingContent::PAST_TYPE;
    type = mw->GetOperatingTimeContentType(86403);
    EXPECT_EQ(TypeOperatingContent::TWENTY_FOUR_TYPE, type);
}

TEST(testBizTeleworkOnDutyDialog, GetOperatingTimeContent)
{
    BizTeleworkDialogDependency dependency;
    dependency.GetLanguageResource()->ReloadLanguageResource(LanguageIDJapanese);
    std::unique_ptr<BizTeleworkOnDutyDialog> mw = std::make_unique<BizTeleworkOnDutyDialog>(dependency, 0, nullptr);

    const std::wstring contentPast = L"0時間0分";
    const std::wstring contentTwentyFour = L"24時間以上";
    const std::wstring contentNormal = L"0時間4分";

    std::wstring operatingTimeContent = L"";

    mw->GetOperatingTimeContent(TypeOperatingContent::PAST_TYPE, 86500, &operatingTimeContent);
    EXPECT_EQ(0, operatingTimeContent.compare(contentPast));

    mw->GetOperatingTimeContent(TypeOperatingContent::TWENTY_FOUR_TYPE, -356, &operatingTimeContent);
    EXPECT_EQ(0, operatingTimeContent.compare(contentTwentyFour));

    mw->GetOperatingTimeContent(TypeOperatingContent::NORMAL_TYPE, 240, &operatingTimeContent);
    EXPECT_EQ(0, operatingTimeContent.compare(contentNormal));

    EXPECT_FALSE(mw->GetOperatingTimeContent(static_cast<TypeOperatingContent>(4), 240, &operatingTimeContent));

}

TEST(testBizTeleworkOnDutyDialog, DelayTime)
{
    MockBizTeleworkDialogDependency dependency;
    std::unique_ptr<BizTeleworkOnDutyDialog> mw = std::make_unique<BizTeleworkOnDutyDialog>(dependency, 10, nullptr);
    EXPECT_EQ(5, mw->GetUnixStartTime());
}
