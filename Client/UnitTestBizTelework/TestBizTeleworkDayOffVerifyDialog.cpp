#include "pch.h"
#include <mutex>
#include <thread>
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <BizTeleworkDayOffVerifyDialog.h>
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

TEST(testBizTeleworkDayOffVerifyDialog, Constructor)
{
    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_BUTTON_OFF_DAY)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_BUTTON_DO_WORKING)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_STATIC_TAKE_BREAK_QUESTION)), _))
        .WillOnce(Return(true));

    MockBizTeleworkDialogDependency dependency;
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));

    WinAppForTest app([&dependency]()
    {
        auto mw = std::make_unique<BizTeleworkDayOffVerifyDialog>(dependency, nullptr);
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
