#include "pch.h"
#include <mutex>
#include <thread>
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <BizTeleworkAuthenticationDialog.h>
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

TEST(testBizTeleworkAuthenticationDialog, Constructor)
{
    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_BUTTON_LOGIN)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_CUE_BANNER_COMPANY_CODE)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_CUE_BANNER_ACTIVE_CODE)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_STATIC_ENTER_URL)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_STATIC_AUTH_EXPLANATION)), _))
        .WillOnce(Return(true));

    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_STATIC_AUTH_SUCCESSFUL)), _))
        .WillOnce(Return(true));

    MockBizTeleworkDialogDependency dependency;
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));

    WinAppForTest app([&dependency]()
        {
            ActiveInformation activeInfor;
            auto mw = std::make_unique<BizTeleworkAuthenticationDialog>(dependency, nullptr, activeInfor);
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
