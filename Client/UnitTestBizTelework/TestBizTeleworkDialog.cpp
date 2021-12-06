#include "pch.h"

#include <mutex>
#include <thread>
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <BizTeleworkDialog.h>
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

TEST(testBizTeleworkDialog, Constructor)
{
    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_START_WORKING)), _))
        .WillOnce(Return(true));
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_FINISH_WORKING)), _))
        .WillOnce(Return(true));
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_COMMENT)), _))
        .WillOnce(Return(true));

    MockBizTeleworkDialogDependency dependency;
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));
    EXPECT_CALL(dependency, GetRequest())
        .WillOnce(Return(ByMove(nullptr)));

    WinAppForTest app([&dependency]()
    {
        auto mw = std::make_unique<BizTeleworkDialog>(dependency, nullptr);
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
