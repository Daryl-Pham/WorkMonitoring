#pragma once

#include <Windows.h>
#include <sdkddkver.h>
#include <WinUser.h>
#include <ShObjIdl.h>
#include <wrl/implements.h>
#include <wrl/event.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <ShlObj.h>
#include <roapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <winstring.h>
#include <string.h>
#include <vector>
#include <map>

using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;

class ToastNotificationHandlerInterface
{
public:
    enum class ToastDismissalReasonEnum
    {
        UserCanceled = ToastDismissalReason::ToastDismissalReason_UserCanceled,
        ApplicationHidden = ToastDismissalReason::ToastDismissalReason_ApplicationHidden,
        TimedOut = ToastDismissalReason::ToastDismissalReason_TimedOut
    };

    enum class TypeEventHandlerEnum
    {
        Actived = 0,
        Dismissed = 1,
        Failed = 2
    };

    virtual ~ToastNotificationHandlerInterface() = default;
    virtual bool ToastActivated() const = 0;
    virtual bool ToastActivated(int actionIndex) const = 0;
    virtual bool ToastDismissed(ToastDismissalReasonEnum state) const = 0;
    virtual bool ToastFailed() const = 0;
    virtual bool SetToastNotificationHandler(HWND hWnd) = 0;
};

class ToastNotificationTemplate
{
public:
    enum class Duration { System, Short, Long };
    enum class AudioOption { Default = 0, Silent, Loop };
    enum class TextField { FirstLine = 0, SecondLine = 1, ThirdLine = 2 };
    enum class ToastNotificationTemplateType
    {
        ImageAndText01 = ToastTemplateType::ToastTemplateType_ToastImageAndText01,
        ImageAndText02 = ToastTemplateType::ToastTemplateType_ToastImageAndText02,
        ImageAndText03 = ToastTemplateType::ToastTemplateType_ToastImageAndText03,
        ImageAndText04 = ToastTemplateType::ToastTemplateType_ToastImageAndText04,
        Text01 = ToastTemplateType::ToastTemplateType_ToastText01,
        Text02 = ToastTemplateType::ToastTemplateType_ToastText02,
        Text03 = ToastTemplateType::ToastTemplateType_ToastText03,
        Text04 = ToastTemplateType::ToastTemplateType_ToastText04,
    };
    enum class AudioSystemFile
    {
        DefaultSound,
        IM,
        Mail,
        Reminder,
        SMS,
        Alarm,
        Alarm2,
        Alarm3,
        Alarm4,
        Alarm5,
        Alarm6,
        Alarm7,
        Alarm8,
        Alarm9,
        Alarm10,
        Call,
        Call1,
        Call2,
        Call3,
        Call4,
        Call5,
        Call6,
        Call7,
        Call8,
        Call9,
        Call10,
    };

    explicit ToastNotificationTemplate(const ToastNotificationTemplateType& type = ToastNotificationTemplateType::Text01);
    ToastNotificationTemplate(const ToastNotificationTemplate& other);
    ToastNotificationTemplate& operator=(const ToastNotificationTemplate& other);
    ToastNotificationTemplate(ToastNotificationTemplate&& fp) noexcept;
    ToastNotificationTemplate const& operator=(ToastNotificationTemplate&& fp);
    ~ToastNotificationTemplate();

    void SetFirstLine(const std::wstring& text);
    void SetSecondLine(const std::wstring& text);
    void SetThirdLine(const std::wstring& text);
    void SetTextField(const std::wstring& txt, TextField pos);
    void SetAttributionText(const std::wstring& attributionText);
    void SetImagePath(const std::wstring& imgPath);
    void SetAudioPath(ToastNotificationTemplate::AudioSystemFile audio);
    void SetAudioPath(const std::wstring& audioPath);
    void SetAudioOption(ToastNotificationTemplate::AudioOption audioOption);
    void SetDuration(Duration duration);
    void SetExpiration(INT64 millisecondsFromNow);
    void AddAction(const std::wstring& label);

    std::size_t TextFieldsCount() const;
    std::size_t ActionsCount() const;
    bool HasImage() const;
    const std::vector<std::wstring>& TextFields() const;
    const std::wstring GetStringTextField(TextField pos) const;
    const std::wstring ActionLabel(std::size_t pos) const;
    const std::wstring& ImagePath() const;
    const std::wstring& AudioPath() const;
    const std::wstring& AttributionText() const;
    INT64 Expiration() const;
    ToastNotificationTemplateType GetType() const;
    AudioOption GetAudioOption() const;
    Duration GetDuration() const;

private:
    std::vector<std::wstring> m_TextFields{};
    std::vector<std::wstring> m_Actions{};
    std::wstring m_ImagePath{};
    std::wstring m_AudioPath{};
    std::wstring m_AttributionText{};
    INT64 m_Expiration{ 0 };  // milisecond
    AudioOption m_AudioOption{ AudioOption::Default };
    ToastNotificationTemplateType m_Type{ ToastNotificationTemplateType::Text01 };
    Duration m_Duration{ Duration::System };
};

class TeleworkToastNotification
{
public:
    enum class ToastError
    {
        NoError = 0,
        NotInitialized,
        SystemNotSupported,
        ShellLinkNotCreated,
        InvalidAppUserModelID,
        InvalidParameters,
        InvalidHandler,
        NotDisplayed,
        UnknownError
    };

    enum class ShortcutResult
    {
        ShortcutUnChanged = 0,
        ShortcutWasChanged = 1,
        ShortcutWasCreated = 2,
        ShortcutMissingParameters = -1,
        ShortcutIncompatibleOS = -2,
        ShortcutCOMInitFailure = -3,
        ShortcutCreateFailed = -4
    };

    TeleworkToastNotification();
    TeleworkToastNotification(const TeleworkToastNotification& other);
    TeleworkToastNotification& operator=(const TeleworkToastNotification& other);
    TeleworkToastNotification(TeleworkToastNotification&& fp) noexcept;
    TeleworkToastNotification const& operator=(TeleworkToastNotification&& fp);
    virtual ~TeleworkToastNotification();
    static TeleworkToastNotification* GetInstance();
    static bool IsCompatible();
    static bool IsSupportingModernFeatures();
    static std::wstring BuildAUMI(
        const std::wstring& companyName,
        const std::wstring& productName,
        const std::wstring& subProduct = std::wstring(),
        const std::wstring& versionInformation = std::wstring());
    static const std::wstring& strerror(ToastError error);
    virtual bool Initialize(ToastError* error = nullptr);
    virtual bool IsInitialized() const;
    virtual bool HideToast(INT64 id);
    virtual INT64 ShowToast(const ToastNotificationTemplate& toast, ToastNotificationHandlerInterface* handler, ToastError* error = nullptr);
    virtual void Clear();
    virtual ShortcutResult CreateShortcut();

    const std::wstring& AppName() const;
    const std::wstring& AppUserModelId() const;
    void SetAppUserModelId(const std::wstring& aumi);
    void SetAppName(const std::wstring& appName);

protected:
    bool                                        m_IsInitialized{ false };
    bool                                        m_HasCoInitialized{ false };
    std::wstring                                m_AppName{};
    std::wstring                                m_Aumi{};
    std::map<INT64, ComPtr<IToastNotification>> m_Buffer{};

    HRESULT ValidateShellLinkHelper(bool* wasChanged);
    HRESULT CreateShellLinkHelper();
    HRESULT SetImageFieldHelper(IXmlDocument* xml, const std::wstring& path);
    HRESULT SetAudioFieldHelper(IXmlDocument* xml,
                                const std::wstring& path,
                                ToastNotificationTemplate::AudioOption option = ToastNotificationTemplate::AudioOption::Default);
    HRESULT SetTextFieldHelper(IXmlDocument* xml, const std::wstring& text, UINT32 pos);
    HRESULT SetAttributionTextFieldHelper(IXmlDocument* xml, const std::wstring& text);
    HRESULT AddDurationHelper(IXmlDocument* xml, const std::wstring& duration);
    ComPtr<IToastNotifier> Notifier(bool* succeded) const;
    void SetError(ToastError* error, ToastError value);
    HRESULT RegisterComServer();
};
