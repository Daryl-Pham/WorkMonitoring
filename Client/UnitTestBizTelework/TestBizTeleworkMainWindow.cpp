#include "pch.h"
#include <thread>
#include <BizTeleworkMainWindow.h>
#include <Language.h>
#include "WinAppForTest.h"
#include "Mock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Eq;


TEST(testBizTeleworkMainWindow, Constructor)
{
    std::unique_ptr<MockNotifyIcon> ni(new MockNotifyIcon());
    EXPECT_CALL(*ni, Show())
        .WillOnce(Return(true));
    testing::Mock::AllowLeak(ni.get());

    std::unique_ptr<MockWindowsHook> wh(new MockWindowsHook());
    EXPECT_CALL(*wh, SetHookForMouse(_, _))
        .WillOnce(Return(ByMove(TRUE)));
    EXPECT_CALL(*wh, SetHookForKeyboard(_, _))
        .WillOnce(Return(TRUE));

    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_NOTIFY_ICON_RUNNING)), _))
        .WillOnce(Return(true));

    std::unique_ptr<MockCallContextForTimeCardsToday> cc(new MockCallContextForTimeCardsToday());
    EXPECT_CALL(*cc, Wait())
        .WillOnce(Return(ByMove(TRUE)));
    EXPECT_CALL(*cc, Body())
        .WillOnce(Return(L"{\"status\":\"on_duty\",\"started_at\":\"2020-07-17T08:48:16Z\"}"));
    EXPECT_CALL(*cc, StatusCode())
        .WillOnce(Return(200));

    std::unique_ptr<MockRequest> mr(new MockRequest());
    EXPECT_CALL(*mr, CallWindowsTimeCardsToday())
        .WillOnce(Return(ByMove(std::move(cc))));

    std::unique_ptr<MockAppConfig> mac(new MockAppConfig());
    EXPECT_CALL(*mac, SetCopiedAuthentication(_))
        .WillOnce(Return(true));

    std::unique_ptr<MockAuthenHelper> mah(new MockAuthenHelper());
    EXPECT_CALL(*mah, SetCompanyGuid(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mah, SetAgentGuid(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mah, GetAuthenInforFromLocalMachine(_, _, _))
        .WillOnce(Return(true));
    EXPECT_CALL(*mah, SetApiUrl(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mah, DeleteAuthenticationFromLocalMachine())
        .WillOnce(Return(true));

    std::unique_ptr<MockToastNotificationHandler> mtnh(new MockToastNotificationHandler());
    EXPECT_CALL(*mtnh, SetToastNotificationHandler(_))
        .WillOnce(Return(true));

    MockBizTeleworkMainWindowDependency dependency;
    EXPECT_CALL(dependency, GetNotifyIcon(_, _, _, _, _))
        .WillOnce(Return(ByMove(std::move(ni))));
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));
    EXPECT_CALL(dependency, GetWindowsHook())
        .WillOnce(Return(ByMove(std::move(wh))));
    EXPECT_CALL(dependency, GetApiUrl())
        .WillOnce(Return(L""));
    EXPECT_CALL(dependency, IsActive())
        .WillOnce(Return(true));
    EXPECT_CALL(dependency, GetRequest(_))
        .WillOnce(Return(ByMove(std::move(mr))));
    EXPECT_CALL(dependency, GetAppConfig())
        .WillOnce(Return(ByMove(std::move(mac))));
    EXPECT_CALL(dependency, GetAuthenHelper())
        .WillOnce(Return(ByMove(std::move(mah))));
    EXPECT_CALL(dependency, CreateProductKeyAtRegistryCurrentUser())
        .WillOnce(Return(true));
    EXPECT_CALL(dependency, IsCopiedAuthenticationToCurrentUser())
        .WillOnce(Return(false));
    EXPECT_CALL(dependency, GetToastNotificationHandler())
        .WillOnce(Return(ByMove(std::move(mtnh))));

    WinAppForTest app([&dependency, &ni, &wh]()
    {
        auto mw = std::make_unique<BizTeleworkMainWindow>(dependency);
        EXPECT_TRUE(mw->Init(dependency));
        EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(wh.get()));
        return std::move(mw);
    });

    auto timeoutThread = std::thread([&]()
    {
        Sleep(1000);
        ::PostMessage(app.m_pMainWnd->GetSafeHwnd(), WM_QUIT, 0, 0);
    });
    timeoutThread.detach();

    ::AfxWinMain(GetModuleHandle(NULL), NULL, 0, 0);
}

TEST(testBizTeleworkMainWindow, FailedToShowNotifyIcon)
{
    std::unique_ptr<MockNotifyIcon> ni(new MockNotifyIcon());
    EXPECT_CALL(*ni, Show())
        .WillOnce(Return(false));
    testing::Mock::AllowLeak(ni.get());

    MockLanguageResource lr;
    EXPECT_CALL(lr, GetString(Matcher<UINT>(Eq(IDS_NOTIFY_ICON_RUNNING)), _))
        .WillOnce(Return(true));

    MockBizTeleworkMainWindowDependency dependency;
    EXPECT_CALL(dependency, GetNotifyIcon(_, _, _, _, _))
        .WillOnce(Return(ByMove(std::move(ni))));
    EXPECT_CALL(dependency, GetLanguageResource())
        .WillOnce(Return(&lr));
    EXPECT_CALL(dependency, GetWindowsHook())
        .WillOnce(Return(ByMove(nullptr)));
    EXPECT_CALL(dependency, GetApiUrl())
        .WillOnce(Return(L""));

    WinAppForTest app([&dependency]()
        {
            auto mw = std::make_unique<BizTeleworkMainWindow>(dependency);
            EXPECT_FALSE(mw->Init(dependency));
            return std::move(mw);
        });

    auto timeoutThread = std::thread([&]()
        {
            Sleep(1000);
            ::PostMessage(app.m_pMainWnd->GetSafeHwnd(), WM_QUIT, 0, 0);
        });
    timeoutThread.detach();

    ::AfxWinMain(GetModuleHandle(NULL), NULL, 0, 0);
}
