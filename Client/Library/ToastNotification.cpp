#include "stdafx.h"
#include "ToastNotification.h"
#include <memory>
#include <assert.h>
#include <unordered_map>
#include <array>
#include "Log.h"
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>
#include "NotificationActivationCallback.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "RuntimeObject.lib")

namespace DllImporter
{
    DEFINE_LOGGER_FILE_NAME;

    typedef HRESULT(_stdcall* SetCurrentProcessExplicitAppUserModelIDFunc)(PCWSTR);
    typedef HRESULT(_stdcall* PropVariantToStringFunc)(REFPROPVARIANT, PWSTR, UINT);
    typedef HRESULT(_stdcall* RoGetActivationFactoryFunc)(HSTRING, REFIID, void**);
    typedef HRESULT(_stdcall* WindowsCreateStringReferenceFunc)(PCWSTR, UINT32, HSTRING_HEADER*, HSTRING*);
    typedef PCWSTR(_stdcall* WindowsGetStringRawBufferFunc)(HSTRING, UINT32*);
    typedef HRESULT(_stdcall* WindowsDeleteStringFunc)(HSTRING);

    SetCurrentProcessExplicitAppUserModelIDFunc GetSetCurrentProcessExplicitAppUserModelIDFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"SHELL32.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        SetCurrentProcessExplicitAppUserModelIDFunc func =
            reinterpret_cast<SetCurrentProcessExplicitAppUserModelIDFunc>(GetProcAddress(mod, "SetCurrentProcessExplicitAppUserModelID"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    PropVariantToStringFunc GetPropVariantToStringFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"PROPSYS.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        PropVariantToStringFunc func = reinterpret_cast<PropVariantToStringFunc>(GetProcAddress(mod, "PropVariantToString"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    RoGetActivationFactoryFunc GetRoGetActivationFactoryFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"COMBASE.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        RoGetActivationFactoryFunc func = reinterpret_cast<RoGetActivationFactoryFunc>(GetProcAddress(mod, "RoGetActivationFactory"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    WindowsCreateStringReferenceFunc GetWindowsCreateStringReferenceFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"COMBASE.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        WindowsCreateStringReferenceFunc func = reinterpret_cast<WindowsCreateStringReferenceFunc>(GetProcAddress(mod, "WindowsCreateStringReference"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    WindowsGetStringRawBufferFunc GetWindowsGetStringRawBufferFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"COMBASE.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        WindowsGetStringRawBufferFunc func = reinterpret_cast<WindowsGetStringRawBufferFunc>(GetProcAddress(mod, "WindowsGetStringRawBuffer"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            FreeLibrary(mod);
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    WindowsDeleteStringFunc GetWindowsDeleteStringFunc(HMODULE* hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"COMBASE.DLL");
        if (!mod)
        {
            LOG_WARN("LoadLibrary() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        WindowsDeleteStringFunc func = reinterpret_cast<WindowsDeleteStringFunc>(GetProcAddress(mod, "WindowsDeleteString"));
        if (!func)
        {
            LOG_WARN("GetProcAddress() failed. 0x%08x", GetLastError());
            if (!FreeLibrary(mod))
            {
                LOG_WARN("FreeLibrary() failed.");
            }
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }

    static SetCurrentProcessExplicitAppUserModelIDFunc s_SetCurrentProcessExplicitAppUserModelIDFunc;
    static PropVariantToStringFunc s_PropVariantToStringFunc;
    static RoGetActivationFactoryFunc s_RoGetActivationFactoryFunc;
    static WindowsCreateStringReferenceFunc s_WindowsCreateStringReferenceFunc;
    static WindowsGetStringRawBufferFunc s_WindowsGetStringRawBufferFunc;
    static WindowsDeleteStringFunc s_WindowsDeleteStringFunc;

    template<class T>
    HRESULT _1_GetActivationFactory(HSTRING activatableClassId, T** factory)
    {
        return s_RoGetActivationFactoryFunc(activatableClassId, IID_INS_ARGS(factory));
    }

    template<typename T>
    inline HRESULT Wrap_GetActivationFactory(HSTRING activatableClassId, Details::ComPtrRef<T> factory) noexcept
    {
        return _1_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
    }

    inline HRESULT Initialize()
    {
        HMODULE hUser32 = NULL;
        s_SetCurrentProcessExplicitAppUserModelIDFunc = GetSetCurrentProcessExplicitAppUserModelIDFunc(&hUser32);
        if (!s_SetCurrentProcessExplicitAppUserModelIDFunc)
        {
            return E_FAIL;
        }

        s_PropVariantToStringFunc = GetPropVariantToStringFunc(&hUser32);
        if (!s_PropVariantToStringFunc)
        {
            return E_FAIL;
        }

        s_RoGetActivationFactoryFunc = GetRoGetActivationFactoryFunc(&hUser32);
        if (!s_RoGetActivationFactoryFunc)
        {
            return E_FAIL;
        }

        s_WindowsCreateStringReferenceFunc = GetWindowsCreateStringReferenceFunc(&hUser32);
        if (!s_WindowsCreateStringReferenceFunc)
        {
            return E_FAIL;
        }

        s_WindowsGetStringRawBufferFunc = GetWindowsGetStringRawBufferFunc(&hUser32);
        if (!s_WindowsGetStringRawBufferFunc)
        {
            return E_FAIL;
        }

        s_WindowsDeleteStringFunc = GetWindowsDeleteStringFunc(&hUser32);
        if (!s_WindowsDeleteStringFunc)
        {
            return E_FAIL;
        }

        FreeLibrary(hUser32);
        return S_OK;
    }
}
// namespace DllImporter

class WinToastStringWrapper
{
public:
    WinToastStringWrapper(PCWSTR stringRef, UINT32 length) noexcept
    {
        DllImporter::s_WindowsCreateStringReferenceFunc(stringRef, length, &m_Header, &m_Hstring);
    }

    explicit WinToastStringWrapper(const std::wstring& stringRef) noexcept
    {
        DllImporter::s_WindowsCreateStringReferenceFunc(stringRef.c_str(), static_cast<UINT32>(stringRef.length()), &m_Header, &m_Hstring);
    }

    WinToastStringWrapper(const WinToastStringWrapper& other)
    {
        m_Header = other.m_Header;
        ::WindowsDuplicateString(other.m_Hstring, &m_Hstring);
    }
    WinToastStringWrapper& operator=(const WinToastStringWrapper& other)
    {
        m_Header = other.m_Header;
        ::WindowsDuplicateString(other.m_Hstring, &m_Hstring);
        return *this;
    }
    WinToastStringWrapper(WinToastStringWrapper&& fp) noexcept
    {
        m_Header = fp.m_Header;
        ::WindowsDuplicateString(fp.m_Hstring, &m_Hstring);
        ::WindowsDeleteString(fp.m_Hstring);
    }
    WinToastStringWrapper const& operator=(WinToastStringWrapper&& tsw) noexcept
    {
        WinToastStringWrapper temp(std::move(tsw));
        std::swap(temp.m_Header, m_Header);
        std::swap(temp.m_Hstring, m_Hstring);
        return *this;
    }

    ~WinToastStringWrapper()
    {
        DllImporter::s_WindowsDeleteStringFunc(m_Hstring);
    }

    inline HSTRING Get() const noexcept
    {
        return m_Hstring;
    }

private:
    HSTRING m_Hstring;
    HSTRING_HEADER m_Header;
};

class InternalDateTime : public IReference<DateTime>
{
public:
    static INT64 Now()
    {
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        return ((((INT64)now.dwHighDateTime) << 32) | now.dwLowDateTime);
    }

    explicit InternalDateTime(DateTime dateTime) : m_DateTime(dateTime) {}

    explicit InternalDateTime(INT64 millisecondsFromNow)
    {
        m_DateTime.UniversalTime = Now() + millisecondsFromNow * 10000;
    }

    virtual ~InternalDateTime() = default;

    operator INT64()
    {
        return m_DateTime.UniversalTime;
    }

    HRESULT STDMETHODCALLTYPE get_Value(DateTime* dateTime) override
    {
        *dateTime = m_DateTime;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject)
    {
        if (!ppvObject)
        {
            return E_POINTER;
        }
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IReference<DateTime>))
        {
            *ppvObject = static_cast<IUnknown*>(static_cast<IReference<DateTime>*>(this));
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        return 1;
    }

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return 2;
    }

    HRESULT STDMETHODCALLTYPE GetIids(ULONG*, IID**)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING*)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel*)
    {
        return E_NOTIMPL;
    }

protected:
    DateTime m_DateTime;
};

class DECLSPEC_UUID("BD8EC9B3-CAEB-465A-B5C0-2ABA5D5839D1") CToastActivator
    WrlFinal : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, INotificationActivationCallback, FtmBase>
{
public:
    HRESULT STDMETHODCALLTYPE Activate(
        _In_ LPCWSTR /*appUserModelId*/,
        _In_ LPCWSTR /*invokedArgs*/,
        /*_In_reads_(dataCount)*/ const NOTIFICATION_USER_INPUT_DATA* /*data*/,
        ULONG /*dataCount*/) override
    {
        return S_OK;
    }
};
CoCreatableClass(CToastActivator);

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* DefaultShellLinksPath = L"\\Microsoft\\Windows\\Start Menu\\Programs\\";

    const wchar_t* DefaultLinkFormat = L".lnk";

    #define STATUS_SUCCESS (0x00000000)

    const wchar_t* CLSIDToastActivator = L"BD8EC9B3-CAEB-465A-B5C0-2ABA5D5839D1";

    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    inline RTL_OSVERSIONINFOW GetRealOSVersion()
    {
        HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
        if (hMod)
        {
            RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
            if (fxPtr != nullptr)
            {
                RTL_OSVERSIONINFOW rovi = { 0 };
                rovi.dwOSVersionInfoSize = sizeof(rovi);
                if (STATUS_SUCCESS == fxPtr(&rovi))
                {
                    return rovi;
                }
            }
        }
        RTL_OSVERSIONINFOW rovi = { 0 };
        return rovi;
    }

    inline HRESULT DefaultExecutablePath(wchar_t* path, DWORD nSize = MAX_PATH)
    {
        DWORD written = GetModuleFileNameExW(GetCurrentProcess(), nullptr, path, nSize);
        return (written > 0) ? S_OK : E_FAIL;
    }

    inline HRESULT DefaultShellLinksDirectory(wchar_t* path, DWORD nSize = MAX_PATH)
    {
        DWORD written = GetEnvironmentVariableW(L"APPDATA", path, nSize);
        HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
        if (SUCCEEDED(hr))
        {
            errno_t result = wcscat_s(path, nSize, DefaultShellLinksPath);
            hr = (result == 0) ? S_OK : E_INVALIDARG;
        }
        return hr;
    }

    inline HRESULT DefaultShellLinkPath(const std::wstring& appname, wchar_t* path, DWORD nSize = MAX_PATH)
    {
        HRESULT hr = DefaultShellLinksDirectory(path, nSize);
        if (SUCCEEDED(hr))
        {
            const std::wstring appLink(appname + DefaultLinkFormat);
            errno_t result = wcscat_s(path, nSize, appLink.c_str());
            hr = (result == 0) ? S_OK : E_INVALIDARG;
        }
        return hr;
    }

    inline PCWSTR AsString(HSTRING hstring)
    {
        PCWSTR r = DllImporter::s_WindowsGetStringRawBufferFunc(hstring, nullptr);
        return r;
    }

    inline HRESULT SetNodeStringValue(const std::wstring& string, IXmlNode* node, IXmlDocument* xml)
    {
        ComPtr<IXmlText> textNode;
        HRESULT hr = xml->CreateTextNode(WinToastStringWrapper(string).Get(), &textNode);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> stringNode;
            hr = textNode.As(&stringNode);
            if (SUCCEEDED(hr))
            {
                ComPtr<IXmlNode> appendedChild;
                hr = node->AppendChild(stringNode.Get(), &appendedChild);
            }
        }
        return hr;
    }

    HRESULT SetEventHandlers(IToastNotification* notification, std::shared_ptr<ToastNotificationHandlerInterface> eventHandler, INT64 expirationTime)
    {
        EventRegistrationToken activatedToken, dismissedToken, failedToken;
        HRESULT hr = notification->add_Activated(
            Callback < Implements < RuntimeClassFlags<ClassicCom>,
            ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, IInspectable* >> >(
                [eventHandler](IToastNotification*, IInspectable* inspectable)
                {
                    IToastActivatedEventArgs* activatedEventArgs;
                    HRESULT hr = inspectable->QueryInterface(&activatedEventArgs);
                    if (SUCCEEDED(hr))
                    {
                        HSTRING argumentsHandle;
                        hr = activatedEventArgs->get_Arguments(&argumentsHandle);
                        if (SUCCEEDED(hr))
                        {
                            PCWSTR arguments = AsString(argumentsHandle);
                            if (arguments && *arguments)
                            {
                                eventHandler->ToastActivated(static_cast<int>(wcstol(arguments, nullptr, 10)));
                                return S_OK;
                            }
                        }
                    }
                    eventHandler->ToastActivated();
                    return S_OK;
                }).Get(), &activatedToken);

        if (SUCCEEDED(hr))
        {
            hr = notification->add_Dismissed(Callback < Implements < RuntimeClassFlags<ClassicCom>,
                ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ToastDismissedEventArgs* >> >(
                    [eventHandler, expirationTime](IToastNotification*, IToastDismissedEventArgs* e)
                    {
                        ToastDismissalReason reason;
                        if (SUCCEEDED(e->get_Reason(&reason)))
                        {
                            if (reason == ToastDismissalReason_UserCanceled && expirationTime && InternalDateTime::Now() >= expirationTime)
                                reason = ToastDismissalReason_TimedOut;
                            eventHandler->ToastDismissed(static_cast<ToastNotificationHandlerInterface::ToastDismissalReasonEnum>(reason));
                        }
                        return S_OK;
                    }).Get(), &dismissedToken);

            if (SUCCEEDED(hr))
            {
                hr = notification->add_Failed(Callback < Implements < RuntimeClassFlags<ClassicCom>,
                    ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ToastFailedEventArgs* >> >(
                        [eventHandler](IToastNotification*, IToastFailedEventArgs*)
                        {
                            eventHandler->ToastFailed();
                            return S_OK;
                        }).Get(), &failedToken);
            }
        }

        return hr;
    }

    inline HRESULT AddAttribute(IXmlDocument* xml, const std::wstring& name, IXmlNamedNodeMap* attributeMap)
    {
        ComPtr<ABI::Windows::Data::Xml::Dom::IXmlAttribute> srcAttribute;
        HRESULT hr = xml->CreateAttribute(WinToastStringWrapper(name).Get(), &srcAttribute);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> node;
            hr = srcAttribute.As(&node);
            if (SUCCEEDED(hr))
            {
                ComPtr<IXmlNode> pNode;
                hr = attributeMap->SetNamedItem(node.Get(), &pNode);
            }
        }

        return hr;
    }

    inline HRESULT CreateElement(IXmlDocument* xml,
                                 const std::wstring& rootNode,
                                 const std::wstring& elementName,
                                 const std::vector<std::wstring>& attributeNames)
    {
        ComPtr<IXmlNodeList> rootList;
        HRESULT hr = xml->GetElementsByTagName(WinToastStringWrapper(rootNode).Get(), &rootList);
        if (FAILED(hr))
        {
            LOG_WARN("xml->GetElementsByTagName() failed.");
            return hr;
        }

        ComPtr<IXmlNode> root;
        hr = rootList->Item(0, &root);
        if (FAILED(hr))
        {
            LOG_WARN("rootList->Item() failed.");
            return hr;
        }

        ComPtr<ABI::Windows::Data::Xml::Dom::IXmlElement> audioElement;
        hr = xml->CreateElement(WinToastStringWrapper(elementName).Get(), &audioElement);
        if (FAILED(hr))
        {
            LOG_WARN("xml->CreateElement() failed.");
            return hr;
        }

        ComPtr<IXmlNode> audioNodeTmp;
        hr = audioElement.As(&audioNodeTmp);
        if (FAILED(hr))
        {
            LOG_WARN("audioElement.As() audioNodeTmp failed.");
            return hr;
        }

        ComPtr<IXmlNode> audioNode;
        hr = root->AppendChild(audioNodeTmp.Get(), &audioNode);
        if (FAILED(hr))
        {
            LOG_WARN("root->AppendChild() audioNodeTmp failed.");
            return hr;
        }

        ComPtr<IXmlNamedNodeMap> attributes;
        hr = audioNode->get_Attributes(&attributes);
        if (FAILED(hr))
        {
            LOG_WARN("audioNode->get_Attributes() failed.");
            return hr;
        }

        for (const auto& it : attributeNames)
        {
            hr = AddAttribute(xml, it, attributes.Get());
        }

        return hr;
    }
}  // namespace

TeleworkToastNotification* TeleworkToastNotification::GetInstance()
{
    static TeleworkToastNotification instance;
    return &instance;
}

TeleworkToastNotification::TeleworkToastNotification() :
    m_IsInitialized(false),
    m_HasCoInitialized(false)
{
    if (!IsCompatible())
    {
        LOG_WARN("Info: Your system is not compatible with this library.");
    }
}

TeleworkToastNotification::TeleworkToastNotification(const TeleworkToastNotification& other)
{
    m_IsInitialized = other.m_IsInitialized;
    m_HasCoInitialized = other.m_HasCoInitialized;
    m_AppName = other.m_AppName;
    m_Aumi = other.m_Aumi;
    m_Buffer = other.m_Buffer;
}

TeleworkToastNotification& TeleworkToastNotification::operator=(const TeleworkToastNotification& other)
{
    m_IsInitialized = other.m_IsInitialized;
    m_HasCoInitialized = other.m_HasCoInitialized;
    m_AppName = other.m_AppName;
    m_Aumi = other.m_Aumi;
    m_Buffer = other.m_Buffer;
    return *this;
}

TeleworkToastNotification::TeleworkToastNotification(TeleworkToastNotification&& fp) noexcept
{
    m_IsInitialized = fp.m_IsInitialized;
    m_HasCoInitialized = fp.m_HasCoInitialized;
    m_AppName = fp.m_AppName;
    m_Aumi = fp.m_Aumi;
    m_Buffer = fp.m_Buffer;
}

TeleworkToastNotification const& TeleworkToastNotification::operator=(TeleworkToastNotification&& fp)
{
    TeleworkToastNotification temp(std::move(fp));
    m_IsInitialized = temp.m_IsInitialized;
    m_HasCoInitialized = temp.m_HasCoInitialized;
    m_AppName = temp.m_AppName;
    m_Aumi = temp.m_Aumi;
    m_Buffer = temp.m_Buffer;
    return *this;
}

TeleworkToastNotification::~TeleworkToastNotification()
{
    if (FAILED(Module<OutOfProc>::GetModule().UnregisterObjects()))
    {
        LOG_WARN("UN-register Object COM failed.");
    }

    if (m_HasCoInitialized)
    {
        CoUninitialize();
    }
}

void TeleworkToastNotification::SetAppName(const std::wstring& appName)
{
    m_AppName = appName;
}

void TeleworkToastNotification::SetAppUserModelId(const std::wstring& aumi)
{
    m_Aumi = aumi;
}

bool TeleworkToastNotification::IsCompatible()
{
    return (DllImporter::Initialize() == S_OK);
}

bool TeleworkToastNotification::IsSupportingModernFeatures()
{
    constexpr auto MinimumSupportedVersion = 6;
    return GetRealOSVersion().dwMajorVersion > MinimumSupportedVersion;
}

std::wstring TeleworkToastNotification::BuildAUMI(
    const std::wstring& companyName,
    const std::wstring& productName,
    const std::wstring& subProduct,
    const std::wstring& versionInformation)
{
    std::wstring aumi = companyName;
    aumi += L"." + productName;
    if (subProduct.length() > 0)
    {
        aumi += L"." + subProduct;
        if (versionInformation.length() > 0)
        {
            aumi += L"." + versionInformation;
        }
    }

    if (aumi.length() > SCHAR_MAX)
    {
        LOG_WARN("Info: max size allowed for AUMI: 128 characters.");
    }

    return aumi;
}

const std::wstring& TeleworkToastNotification::strerror(ToastError error)
{
    static const std::unordered_map<ToastError, std::wstring> Labels =
    {
        {ToastError::NoError, L"No error. The process was executed correctly"},
        {ToastError::NotInitialized, L"The library has not been initialized"},
        {ToastError::SystemNotSupported, L"The OS does not support WinToast"},
        {ToastError::ShellLinkNotCreated, L"The library was not able to create a Shell Link for the app"},
        {ToastError::InvalidAppUserModelID, L"The AUMI is not a valid one"},
        {ToastError::InvalidParameters, L"The parameters used to configure the library are not valid normally because an invalid AUMI or App Name"},
        {ToastError::NotDisplayed, L"The toast was created correctly but WinToast was not able to display the toast"},
        {ToastError::UnknownError, L"Unknown error"}
    };

    const auto iter = Labels.find(error);
    assert(iter != Labels.end());
    return iter->second;
}

TeleworkToastNotification::ShortcutResult TeleworkToastNotification::CreateShortcut()
{
    if (m_Aumi.empty() || m_AppName.empty())
    {
        LOG_WARN("App User Model Id or Appname is empty.");
        return ShortcutResult::ShortcutMissingParameters;
    }

    if (!IsCompatible())
    {
        LOG_WARN("Your OS is not compatible with this library.");
        return ShortcutResult::ShortcutIncompatibleOS;
    }

    if (!m_HasCoInitialized)
    {
        HRESULT initHr = CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED);
        if (initHr != RPC_E_CHANGED_MODE)
        {
            if (FAILED(initHr) && initHr != S_FALSE)
            {
                LOG_WARN("Error on COM library initialization!");
                return ShortcutResult::ShortcutCOMInitFailure;
            }
            else
            {
                m_HasCoInitialized = true;
            }
        }
    }

    bool wasChanged;
    HRESULT hr = ValidateShellLinkHelper(&wasChanged);
    if (SUCCEEDED(hr))
    {
        return wasChanged ? ShortcutResult::ShortcutWasChanged : ShortcutResult::ShortcutUnChanged;
    }

    hr = CreateShellLinkHelper();
    return SUCCEEDED(hr) ? ShortcutResult::ShortcutWasChanged : ShortcutResult::ShortcutCreateFailed;
}

bool TeleworkToastNotification::Initialize(TeleworkToastNotification::ToastError* error)
{
    m_IsInitialized = false;
    SetError(error, ToastError::NoError);

    if (!IsCompatible())
    {
        SetError(error, ToastError::SystemNotSupported);
        LOG_WARN("Error: system not supported.");
        return false;
    }


    if (m_Aumi.empty() || m_AppName.empty())
    {
        SetError(error, ToastError::InvalidParameters);
        LOG_WARN("Error while initializing, did you set up a valid AUMI and App name?");
        return false;
    }

    if (static_cast<int>(CreateShortcut()) < 0)
    {
        SetError(error, ToastError::ShellLinkNotCreated);
        LOG_WARN("Error while attaching the AUMI to the current proccess =(");
        return false;
    }

    if (FAILED(DllImporter::s_SetCurrentProcessExplicitAppUserModelIDFunc(m_Aumi.c_str())))
    {
        SetError(error, ToastError::InvalidAppUserModelID);
        LOG_WARN("Error while attaching the AUMI to the current proccess =(");
        return false;
    }

    if (FAILED(RegisterComServer()))
    {
        LOG_WARN("Register COM Server failed.");
        return false;
    }

    if (FAILED(Module<OutOfProc>::GetModule().RegisterObjects()))
    {
        LOG_WARN("Register Object COM failed.");
        return false;
    }

    m_IsInitialized = true;
    return m_IsInitialized;
}

bool TeleworkToastNotification::IsInitialized() const
{
    return m_IsInitialized;
}

const std::wstring& TeleworkToastNotification::AppName() const
{
    return m_AppName;
}

const std::wstring& TeleworkToastNotification::AppUserModelId() const
{
    return m_Aumi;
}

HRESULT TeleworkToastNotification::ValidateShellLinkHelper(bool* wasChanged)
{
    WCHAR path[MAX_PATH] = { L'\0' };
    HRESULT hr = DefaultShellLinkPath(m_AppName, path);
    if (FAILED(hr))
    {
        LOG_WARN("DefaultShellLinkPath() failed.");
        return hr;
    }

    // Check if the file exist
    DWORD attr = GetFileAttributesW(path);
    if (attr >= 0xFFFFFFF)
    {
        LOG_WARN("GetFileAttributesW() failed.");
        return E_FAIL;
    }

    // Let's load the file as shell link to validate.
    // - Create a shell link
    // - Create a persistant file
    // - Load the path as data for the persistant file
    // - Read the property AUMI and validate with the current
    // - Review if AUMI is equal.
    ComPtr<IShellLink> shellLink;
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    if (FAILED(hr))
    {
        LOG_WARN("CoCreateInstance() failed.");
        return hr;
    }

    ComPtr<IPersistFile> persistFile;
    hr = shellLink.As(&persistFile);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink.As() failed.");
        return hr;
    }

    hr = persistFile->Load(path, STGM_READWRITE);
    if (FAILED(hr))
    {
        LOG_WARN("persistFile->Load() failed.");
        return hr;
    }

    ComPtr<IPropertyStore> propertyStore;
    hr = shellLink.As(&propertyStore);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink.As() propertyStore failed.");
        return hr;
    }

    PROPVARIANT appIdPropVar;
    hr = propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("propertyStore->GetValue() PKEY_AppUserModel_ID failed.");
        return hr;
    }

    WCHAR AUMI[MAX_PATH];
    hr = DllImporter::s_PropVariantToStringFunc(appIdPropVar, AUMI, MAX_PATH);
    *wasChanged = false;
    if (FAILED(hr) || m_Aumi != AUMI)
    {
        LOG_INFO("Will update App User Model Id (AUMI).");
        // AUMI Changed for the same app, let's update the current value!
        *wasChanged = true;
        hr = PropVariantClear(&appIdPropVar);
        if (FAILED(hr))
        {
            LOG_WARN("PropVariantClear() appIdPropVar failed.");
            return hr;
        }

        hr = InitPropVariantFromString(m_Aumi.c_str(), &appIdPropVar);
        if (FAILED(hr))
        {
            LOG_WARN("InitPropVariantFromString() appIdPropVar failed.");
            return hr;
        }

        hr = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
        if (FAILED(hr))
        {
            LOG_WARN("propertyStore->SetValue() PKEY_AppUserModel_ID failed.");
            return hr;
        }

        hr = propertyStore->Commit();
        if (FAILED(hr) || FAILED(hr = persistFile->IsDirty()))
        {
            LOG_WARN("propertyStore->Commit() failed. Or, persistFile->IsDirty() failed.");
            return hr;
        }

        hr = persistFile->Save(path, TRUE);
        if (FAILED(hr))
        {
            LOG_WARN("persistFile->Save() failed.");
            return hr;
        }
    }

    hr = PropVariantClear(&appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("PropVariantClear() failed.");
    }

    return hr;
}

HRESULT TeleworkToastNotification::CreateShellLinkHelper()
{
    WCHAR   exePath[MAX_PATH]{ L'\0' };
    WCHAR   slPath[MAX_PATH]{ L'\0' };
    HRESULT hr;
    hr = DefaultShellLinkPath(m_AppName, slPath);
    if (FAILED(hr))
    {
        LOG_WARN("DefaultShellLinkPath() failed.");
        return hr;
    }

    hr = DefaultExecutablePath(exePath);
    if (FAILED(hr))
    {
        LOG_WARN("DefaultExecutablePath() failed.");
        return hr;
    }

    ComPtr<IShellLinkW> shellLink;
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    if (FAILED(hr))
    {
        LOG_WARN("CoCreateInstance() failed.");
        return hr;
    }

    hr = shellLink->SetPath(exePath);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink->SetPath failed.");
        return hr;
    }

    hr = shellLink->SetArguments(L"");
    if (FAILED(hr))
    {
        LOG_WARN("shellLink->SetArguments failed.");
        return hr;
    }

    hr = shellLink->SetWorkingDirectory(exePath);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink->SetWorkingDirectory failed.");
        return hr;
    }

    ComPtr<IPropertyStore> propertyStore;
    hr = shellLink.As(&propertyStore);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink.As() propertyStore failed.");
        return hr;
    }

    PROPVARIANT appIdPropVar;
    hr = InitPropVariantFromString(m_Aumi.c_str(), &appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("InitPropVariantFromString() failed.");
        return hr;
    }

    hr = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("propertyStore->SetValue() PKEY_AppUserModel_ID failed.");
        return hr;
    }

    UUID UUIDToastActivator;
    if (UuidFromStringW((RPC_WSTR)CLSIDToastActivator, &UUIDToastActivator) != RPC_S_OK)
    {
        LOG_WARN("UuidFromStringW() failed.");
        return RPC_S_INVALID_STRING_UUID;
    }

    hr = PropVariantClear(&appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("PropVariantClear() appIdPropVar failed.");
        return hr;
    }
    appIdPropVar.vt = VT_CLSID;
    appIdPropVar.puuid = const_cast<CLSID*>(&UUIDToastActivator);
    hr = propertyStore->SetValue(PKEY_AppUserModel_ToastActivatorCLSID, appIdPropVar);
    if (FAILED(hr))
    {
        LOG_WARN("propertyStore->SetValue() PKEY_AppUserModel_ToastActivatorCLSID failed.");
        return hr;
    }

    if (propertyStore->Commit() == FALSE)
    {
        LOG_WARN("propertyStore->Commit() failed.");
    }

    ComPtr<IPersistFile> persistFile;
    hr = shellLink.As(&persistFile);
    if (FAILED(hr))
    {
        LOG_WARN("shellLink.As() persistFile failed.");
        return hr;
    }

    hr = persistFile->Save(slPath, TRUE);
    if (FAILED(hr))
    {
        LOG_WARN("persistFile->Save() persistFile failed.");
    }

    return hr;
}

INT64 TeleworkToastNotification::ShowToast(const ToastNotificationTemplate& toast, ToastNotificationHandlerInterface* handler, ToastError* error)
{
    SetError(error, ToastError::NoError);
    INT64 id = -1;
    if (!IsInitialized())
    {
        SetError(error, ToastError::NotInitialized);
        LOG_WARN("Error when launching the toast. WinToast is not initialized.");
        return id;
    }
    if (!handler)
    {
        SetError(error, ToastError::InvalidHandler);
        LOG_WARN("Error when launching the toast. Handler cannot be nullptr.");
        return id;
    }

    ComPtr<IToastNotificationManagerStatics> notificationManager;
    HRESULT hr = DllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(),
                                                        &notificationManager);
    if (FAILED(hr))
    {
        LOG_WARN("DllImporter::Wrap_GetActivationFactory() failed.");
        return -1;
    }

    ComPtr<IToastNotifier> notifier;
    hr = notificationManager->CreateToastNotifierWithId(WinToastStringWrapper(m_Aumi).Get(), &notifier);
    if (FAILED(hr))
    {
        LOG_WARN("notificationManager->CreateToastNotifierWithId() failed.");
        return -1;
    }

    ComPtr<IToastNotificationFactory> notificationFactory;;
    hr = DllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(),
                                                &notificationFactory);
    if (FAILED(hr))
    {
        LOG_WARN("DllImporter::Wrap_GetActivationFactory() failed.");
        return -1;
    }

    ComPtr<IXmlDocument> xmlDocument;
    hr = notificationManager->GetTemplateContent(ToastTemplateType(toast.GetType()), &xmlDocument);
    if (FAILED(hr))
    {
        LOG_WARN("notificationManager->GetTemplateContent() failed.");
        return -1;
    }

    for (std::size_t i = 0, fieldsCount = toast.TextFieldsCount(); i < fieldsCount && SUCCEEDED(hr); i++)
    {
        hr = SetTextFieldHelper(xmlDocument.Get(),
            toast.GetStringTextField(static_cast<ToastNotificationTemplate::TextField>(i)),
            static_cast<UINT32>(i));
    }

    if (IsSupportingModernFeatures())
    {
        if (!toast.AttributionText().empty())
        {
            SetAttributionTextFieldHelper(xmlDocument.Get(), toast.AttributionText());
        }

        if (!toast.AudioPath().empty() || toast.GetAudioOption() != ToastNotificationTemplate::AudioOption::Default)
        {
            SetAudioFieldHelper(xmlDocument.Get(), toast.AudioPath(), toast.GetAudioOption());
        }

        if (toast.GetDuration() != ToastNotificationTemplate::Duration::System)
        {
            AddDurationHelper(xmlDocument.Get(),
                              (toast.GetDuration() == ToastNotificationTemplate::Duration::Short) ? L"short" : L"long");
        }
    }
    else
    {
        LOG_ERROR("Modern features (Actions/Sounds/Attributes) not supported in this os version");
    }

    hr = toast.HasImage() ? SetImageFieldHelper(xmlDocument.Get(), toast.ImagePath()) : S_OK;
    if (FAILED(hr))
    {
        LOG_WARN("SetImageFieldHelper() failed.");
    }

    ComPtr<IToastNotification> notification;
    hr = notificationFactory->CreateToastNotification(xmlDocument.Get(), &notification);
    if (FAILED(hr))
    {
        LOG_WARN("notificationFactory->CreateToastNotification() failed.");
        return -1;
    }

    INT64 expiration = 0;
    INT64 relativeExpiration = toast.Expiration();
    if (relativeExpiration > 0)
    {
        InternalDateTime expirationDateTime(relativeExpiration);
        expiration = expirationDateTime;
        hr = notification->put_ExpirationTime(&expirationDateTime);
    }
    if (FAILED(hr))
    {
        LOG_WARN("notification->put_ExpirationTime() failed.");
    }

    hr = SetEventHandlers(notification.Get(), std::shared_ptr<ToastNotificationHandlerInterface>(handler), expiration);
    if (FAILED(hr))
    {
        LOG_WARN("SetEventHandlers() failed.");
        SetError(error, ToastError::InvalidHandler);
    }

    GUID guid;
    hr = CoCreateGuid(&guid);
    if (FAILED(hr))
    {
        LOG_WARN("CoCreateGuid() failed.");
        return -1;
    }

    id = guid.Data1;
    m_Buffer[id] = notification;
    hr = notifier->Show(notification.Get());
    if (FAILED(hr))
    {
        LOG_WARN("notifier->Show() failed.");
        SetError(error, ToastError::NotDisplayed);
    }

    return FAILED(hr) ? -1 : id;
}

ComPtr<IToastNotifier> TeleworkToastNotification::Notifier(bool* succeded) const
{
    ComPtr<IToastNotificationManagerStatics> notificationManager;
    ComPtr<IToastNotifier> notifier;
    HRESULT hr = DllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(),
                                                        &notificationManager);

    if (SUCCEEDED(hr))
    {
        hr = notificationManager->CreateToastNotifierWithId(WinToastStringWrapper(m_Aumi).Get(), &notifier);
    }

    *succeded = SUCCEEDED(hr);
    return notifier;
}

bool TeleworkToastNotification::HideToast(INT64 id)
{
    if (!IsInitialized()) {
        LOG_WARN("Error when hiding the toast. TeleworkToastNotification is not initialized.");
        return false;
    }

    if (m_Buffer.find(id) != m_Buffer.end())
    {
        auto succeded = false;
        auto notify = Notifier(&succeded);
        if (succeded)
        {
            auto result = notify->Hide(m_Buffer[id].Get());
            m_Buffer.erase(id);
            return SUCCEEDED(result);
        }
    }
    return false;
}

void TeleworkToastNotification::Clear()
{
    auto succeded = false;
    auto notify = Notifier(&succeded);
    if (succeded)
    {
        auto end = m_Buffer.end();
        for (auto it = m_Buffer.begin(); it != end; ++it)
        {
            notify->Hide(it->second.Get());
        }
        m_Buffer.clear();
    }
}

HRESULT TeleworkToastNotification::SetAttributionTextFieldHelper(IXmlDocument* xml, const std::wstring& text)
{
    HRESULT hr = CreateElement(xml, L"binding", L"text", { L"placement" });
    if (FAILED(hr))
    {
        LOG_WARN("CreateElement() failed.");
        return hr;
    }

    ComPtr<IXmlNodeList> nodeList;
    hr = xml->GetElementsByTagName(WinToastStringWrapper(L"text").Get(), &nodeList);
    if (FAILED(hr))
    {
        LOG_WARN("xml->GetElementsByTagName() failed.");
        return hr;
    }

    UINT32 nodeListLength;
    hr = nodeList->get_Length(&nodeListLength);
    if (FAILED(hr))
    {
        LOG_WARN("nodeList->get_Length() nodeListLength failed.");
        return hr;
    }

    for (UINT32 i = 0; i < nodeListLength; i++)
    {
        ComPtr<IXmlNode> textNode;
        hr = nodeList->Item(i, &textNode);
        if (FAILED(hr))
        {
            LOG_WARN("nodeList->Item() %d failed.", i);
            continue;
        }

        ComPtr<IXmlNamedNodeMap> attributes;
        hr = textNode->get_Attributes(&attributes);
        if (FAILED(hr))
        {
            LOG_WARN("textNode->get_Attributes() %d failed.", i);
            continue;
        }

        ComPtr<IXmlNode> editedNode;
        hr = attributes->GetNamedItem(WinToastStringWrapper(L"placement").Get(), &editedNode);
        if (FAILED(hr) || !editedNode)
        {
            LOG_WARN("attributes->GetNamedItem() %d failed. Or editedNode NULL.", i);
            continue;
        }

        hr = SetNodeStringValue(L"attribution", editedNode.Get(), xml);
        if (FAILED(hr))
        {
            LOG_WARN("SetNodeStringValue() failed, %d", i);
            continue;
        }

        return SetTextFieldHelper(xml, text, i);
    }

    return hr;
}

HRESULT TeleworkToastNotification::AddDurationHelper(IXmlDocument* xml, const std::wstring& duration)
{
    ComPtr<IXmlNodeList> nodeList;
    HRESULT hr = xml->GetElementsByTagName(WinToastStringWrapper(L"toast").Get(), &nodeList);
    if (FAILED(hr))
    {
        LOG_WARN("xml->GetElementsByTagName() failed.");
        return hr;
    }

    UINT32 length;
    hr = nodeList->get_Length(&length);
    if (FAILED(hr))
    {
        LOG_WARN("nodeList->get_Length() failed.");
        return hr;
    }

    ComPtr<IXmlNode> toastNode;
    hr = nodeList->Item(0, &toastNode);
    if (FAILED(hr))
    {
        LOG_WARN("nodeList->Item() failed.");
        return hr;
    }

    ComPtr<IXmlElement> toastElement;
    hr = toastNode.As(&toastElement);
    if (FAILED(hr))
    {
        LOG_WARN("toastNode.As() toastElement failed.");
        return hr;
    }

    hr = toastElement->SetAttribute(WinToastStringWrapper(L"duration").Get(),
                                    WinToastStringWrapper(duration).Get());
    if (FAILED(hr))
    {
        LOG_WARN("toastElement->SetAttribute() failed.");
    }

    return hr;
}

HRESULT TeleworkToastNotification::SetTextFieldHelper(IXmlDocument* xml, const std::wstring& text, UINT32 pos)
{
    ComPtr<IXmlNodeList> nodeList;
    HRESULT hr = xml->GetElementsByTagName(WinToastStringWrapper(L"text").Get(), &nodeList);
    if (FAILED(hr))
    {
        LOG_WARN("xml->GetElementsByTagName() failed.");
        return hr;
    }

    ComPtr<IXmlNode> node;
    hr = nodeList->Item(pos, &node);
    if (FAILED(hr))
    {
        LOG_WARN("st->Item() failed.");
        return hr;
    }

    hr = SetNodeStringValue(text, node.Get(), xml);
    if (FAILED(hr))
    {
        LOG_WARN("SetNodeStringValue() failed.");
    }

    return hr;
}

HRESULT TeleworkToastNotification::SetImageFieldHelper(IXmlDocument* xml, const std::wstring& path)
{
    wchar_t imagePath[MAX_PATH] = L"file:///";
    HRESULT hr = StringCchCatW(imagePath, MAX_PATH, path.c_str());
    if (FAILED(hr))
    {
        LOG_WARN("StringCchCatW() failed.");
        return hr;
    }

    ComPtr<IXmlNodeList> nodeList;
    hr = xml->GetElementsByTagName(WinToastStringWrapper(L"image").Get(), &nodeList);
    if (FAILED(hr))
    {
        LOG_WARN("xml->GetElementsByTagName() failed.");
        return hr;
    }

    ComPtr<IXmlNode> node;
    hr = nodeList->Item(0, &node);
    if (FAILED(hr))
    {
        LOG_WARN("nodeList->Item() failed.");
        return hr;
    }

    ComPtr<IXmlNamedNodeMap> attributes;
    hr = node->get_Attributes(&attributes);
    if (FAILED(hr))
    {
        LOG_WARN("node->get_Attributes() failed.");
        return hr;
    }

    ComPtr<IXmlNode> editedNode;
    hr = attributes->GetNamedItem(WinToastStringWrapper(L"src").Get(), &editedNode);
    if (FAILED(hr))
    {
        LOG_WARN("attributes->GetNamedItem() failed.");
        return hr;
    }

    hr = SetNodeStringValue(imagePath, editedNode.Get(), xml);
    if (FAILED(hr))
    {
        LOG_WARN("SetNodeStringValue() failed.");
    }

    return hr;
}

HRESULT TeleworkToastNotification::SetAudioFieldHelper(IXmlDocument* xml, const std::wstring& path, ToastNotificationTemplate::AudioOption option)
{
    std::vector<std::wstring> attrs;
    if (!path.empty())
    {
        attrs.push_back(L"src");
    }

    if (option == ToastNotificationTemplate::AudioOption::Loop)
    {
        attrs.push_back(L"loop");
    }

    if (option == ToastNotificationTemplate::AudioOption::Silent)
    {
        attrs.push_back(L"silent");
    }

    HRESULT hr = CreateElement(xml, L"toast", L"audio", attrs);
    if (FAILED(hr))
    {
        LOG_WARN("CreateElement() failed.");
        return hr;
    }

    ComPtr<IXmlNodeList> nodeList;
    hr = xml->GetElementsByTagName(WinToastStringWrapper(L"audio").Get(), &nodeList);
    if (FAILED(hr))
    {
        LOG_WARN("xml->GetElementsByTagName() failed.");
        return hr;
    }

    ComPtr<IXmlNode> node;
    hr = nodeList->Item(0, &node);
    if (FAILED(hr))
    {
        LOG_WARN("nodeList->Item() failed.");
        return hr;
    }

    ComPtr<IXmlNamedNodeMap> attributes;
    hr = node->get_Attributes(&attributes);
    if (FAILED(hr))
    {
        LOG_WARN("node->get_Attributes() failed.");
        return hr;
    }

    ComPtr<IXmlNode> editedNode;
    if (!path.empty())
    {
        hr = attributes->GetNamedItem(WinToastStringWrapper(L"src").Get(), &editedNode);
        if (SUCCEEDED(hr))
        {
            hr = SetNodeStringValue(path, editedNode.Get(), xml);
        }
    }

    if (SUCCEEDED(hr))
    {
        switch (option)
        {
        case ToastNotificationTemplate::AudioOption::Loop:
            hr = attributes->GetNamedItem(WinToastStringWrapper(L"loop").Get(), &editedNode);
            if (SUCCEEDED(hr))
            {
                hr = SetNodeStringValue(L"true", editedNode.Get(), xml);
            }
            break;
        case ToastNotificationTemplate::AudioOption::Silent:
            hr = attributes->GetNamedItem(WinToastStringWrapper(L"silent").Get(), &editedNode);
            if (SUCCEEDED(hr))
            {
                hr = SetNodeStringValue(L"true", editedNode.Get(), xml);
            }
            break;
        default:
            break;
        }
    }

    return hr;
}

void TeleworkToastNotification::SetError(ToastError* error, ToastError value)
{
    if (error)
    {
        *error = value;
    }
}

HRESULT TeleworkToastNotification::RegisterComServer()
{
    // Turn the GUID into a string
    OLECHAR* clsidOlechar;
    if (StringFromCLSID(__uuidof(CToastActivator), &clsidOlechar) != S_OK)
    {
        LOG_WARN("StringFromCLSID() failed.");
        return E_FAIL;
    }
    std::wstring clsidStr(clsidOlechar);
    ::CoTaskMemFree(clsidOlechar);

    // Create the subkey
    std::wstring subKey = LR"(SOFTWARE\Classes\CLSID\)" + clsidStr + LR"(\LocalServer32)";

    WCHAR   exePath[MAX_PATH]{ L'\0' };
    DefaultExecutablePath(exePath);
    std::wstring exePathStr(exePath);

    exePathStr = exePathStr + L" notification";

    DWORD dataSize = static_cast<DWORD>((exePathStr.length() + 1) * sizeof(WCHAR));

    // Register the EXE for the COM server
    return HRESULT_FROM_WIN32(::RegSetKeyValue(
        HKEY_CURRENT_USER,
        subKey.c_str(),
        nullptr,
        REG_SZ,
        reinterpret_cast<const BYTE*>(exePathStr.c_str()),
        dataSize));
}

ToastNotificationTemplate::ToastNotificationTemplate(const ToastNotificationTemplateType& type)
    : m_Type(type)
{
    static constexpr std::size_t TextFieldsCount[] = { 1, 2, 2, 3, 1, 2, 2, 3 };
    m_TextFields = std::vector<std::wstring>(TextFieldsCount[static_cast<int>(type)], L"");
}

ToastNotificationTemplate::~ToastNotificationTemplate()
{
    m_TextFields.clear();
}

void ToastNotificationTemplate::SetTextField(const std::wstring& txt, ToastNotificationTemplate::TextField pos)
{
    const auto position = static_cast<std::size_t>(pos);
    if (position >= m_TextFields.size())
    {
        LOG_WARN("ToastNotificationTemplate::SetTextField failed: text field over third line.");
        return;
    }
    m_TextFields[position] = txt;
}

ToastNotificationTemplate::ToastNotificationTemplate(const ToastNotificationTemplate& other)
{
    m_TextFields = other.m_TextFields;
    m_Actions = other.m_Actions;
    m_ImagePath = other.m_ImagePath;
    m_AudioPath = other.m_AudioPath;
    m_AttributionText = other.m_AttributionText;
    m_Expiration = other.m_Expiration;
    m_AudioOption = other.m_AudioOption;
    m_Type = other.m_Type;
    m_Duration = other.m_Duration;
}

ToastNotificationTemplate& ToastNotificationTemplate::operator=(const ToastNotificationTemplate& other)
{
    m_TextFields = other.m_TextFields;
    m_Actions = other.m_Actions;
    m_ImagePath = other.m_ImagePath;
    m_AudioPath = other.m_AudioPath;
    m_AttributionText = other.m_AttributionText;
    m_Expiration = other.m_Expiration;
    m_AudioOption = other.m_AudioOption;
    m_Type = other.m_Type;
    m_Duration = other.m_Duration;
    return *this;
}

ToastNotificationTemplate::ToastNotificationTemplate(ToastNotificationTemplate&& fp) noexcept
{
    m_TextFields = fp.m_TextFields;
    m_Actions = fp.m_Actions;
    m_ImagePath = fp.m_ImagePath;
    m_AudioPath = fp.m_AudioPath;
    m_AttributionText = fp.m_AttributionText;
    m_Expiration = fp.m_Expiration;
    m_AudioOption = fp.m_AudioOption;
    m_Type = fp.m_Type;
    m_Duration = fp.m_Duration;
}

ToastNotificationTemplate const& ToastNotificationTemplate::operator=(ToastNotificationTemplate&& fp)
{
    ToastNotificationTemplate temp(std::move(fp));
    std::swap(temp.m_TextFields, fp.m_TextFields);
    std::swap(temp.m_Actions, fp.m_Actions);
    std::swap(temp.m_ImagePath, fp.m_ImagePath);
    std::swap(temp.m_AudioPath, fp.m_AudioPath);
    std::swap(temp.m_AttributionText, fp.m_AttributionText);
    std::swap(temp.m_Expiration, fp.m_Expiration);
    std::swap(temp.m_AudioOption, fp.m_AudioOption);
    std::swap(temp.m_Type, fp.m_Type);
    std::swap(temp.m_Duration, fp.m_Duration);
    return *this;
}

void ToastNotificationTemplate::SetImagePath(const std::wstring& imgPath)
{
    m_ImagePath = imgPath;
}

void ToastNotificationTemplate::SetAudioPath(const std::wstring& audioPath)
{
    m_AudioPath = audioPath;
}

void ToastNotificationTemplate::SetAudioPath(AudioSystemFile file)
{
    static const std::unordered_map<AudioSystemFile, std::wstring> Files =
    {
        {AudioSystemFile::DefaultSound, L"ms-winsoundevent:Notification.Default"},
        {AudioSystemFile::IM, L"ms-winsoundevent:Notification.IM"},
        {AudioSystemFile::Mail, L"ms-winsoundevent:Notification.Mail"},
        {AudioSystemFile::Reminder, L"ms-winsoundevent:Notification.Reminder"},
        {AudioSystemFile::SMS, L"ms-winsoundevent:Notification.SMS"},
        {AudioSystemFile::Alarm, L"ms-winsoundevent:Notification.Looping.Alarm"},
        {AudioSystemFile::Alarm2, L"ms-winsoundevent:Notification.Looping.Alarm2"},
        {AudioSystemFile::Alarm3, L"ms-winsoundevent:Notification.Looping.Alarm3"},
        {AudioSystemFile::Alarm4, L"ms-winsoundevent:Notification.Looping.Alarm4"},
        {AudioSystemFile::Alarm5, L"ms-winsoundevent:Notification.Looping.Alarm5"},
        {AudioSystemFile::Alarm6, L"ms-winsoundevent:Notification.Looping.Alarm6"},
        {AudioSystemFile::Alarm7, L"ms-winsoundevent:Notification.Looping.Alarm7"},
        {AudioSystemFile::Alarm8, L"ms-winsoundevent:Notification.Looping.Alarm8"},
        {AudioSystemFile::Alarm9, L"ms-winsoundevent:Notification.Looping.Alarm9"},
        {AudioSystemFile::Alarm10, L"ms-winsoundevent:Notification.Looping.Alarm10"},
        {AudioSystemFile::Call, L"ms-winsoundevent:Notification.Looping.Call"},
        {AudioSystemFile::Call1, L"ms-winsoundevent:Notification.Looping.Call1"},
        {AudioSystemFile::Call2, L"ms-winsoundevent:Notification.Looping.Call2"},
        {AudioSystemFile::Call3, L"ms-winsoundevent:Notification.Looping.Call3"},
        {AudioSystemFile::Call4, L"ms-winsoundevent:Notification.Looping.Call4"},
        {AudioSystemFile::Call5, L"ms-winsoundevent:Notification.Looping.Call5"},
        {AudioSystemFile::Call6, L"ms-winsoundevent:Notification.Looping.Call6"},
        {AudioSystemFile::Call7, L"ms-winsoundevent:Notification.Looping.Call7"},
        {AudioSystemFile::Call8, L"ms-winsoundevent:Notification.Looping.Call8"},
        {AudioSystemFile::Call9, L"ms-winsoundevent:Notification.Looping.Call9"},
        {AudioSystemFile::Call10, L"ms-winsoundevent:Notification.Looping.Call10"},
    };
    const auto iter = Files.find(file);
    if (iter == Files.end())
    {
        LOG_WARN("ToastNotificationTemplate::SetAudioPath failed. Can't find audio file.");
        return;
    }
    m_AudioPath = iter->second;
}

void ToastNotificationTemplate::SetAudioOption(ToastNotificationTemplate::AudioOption audioOption)
{
    m_AudioOption = audioOption;
}

void ToastNotificationTemplate::SetFirstLine(const std::wstring& text)
{
    SetTextField(text, TextField::FirstLine);
}

void ToastNotificationTemplate::SetSecondLine(const std::wstring& text)
{
    SetTextField(text, TextField::SecondLine);
}

void ToastNotificationTemplate::SetThirdLine(const std::wstring& text)
{
    SetTextField(text, TextField::ThirdLine);
}

void ToastNotificationTemplate::SetDuration(Duration duration)
{
    m_Duration = duration;
}

void ToastNotificationTemplate::SetExpiration(INT64 millisecondsFromNow)
{
    m_Expiration = millisecondsFromNow;
}

void ToastNotificationTemplate::SetAttributionText(const std::wstring& attributionText)
{
    m_AttributionText = attributionText;
}

void ToastNotificationTemplate::AddAction(const std::wstring& label)
{
    m_Actions.push_back(label);
}

std::size_t ToastNotificationTemplate::TextFieldsCount() const
{
    return m_TextFields.size();
}

std::size_t ToastNotificationTemplate::ActionsCount() const
{
    return m_Actions.size();
}

bool ToastNotificationTemplate::HasImage() const
{
    return m_Type < ToastNotificationTemplateType::Text01;
}

const std::vector<std::wstring>& ToastNotificationTemplate::TextFields() const
{
    return m_TextFields;
}

const std::wstring ToastNotificationTemplate::GetStringTextField(TextField pos) const
{
    const auto position = static_cast<std::size_t>(pos);
    if (position >= m_TextFields.size())
    {
        LOG_WARN("ToastNotificationTemplate::GetStringTextField() failed.");
        return std::wstring();
    }
    return m_TextFields[position];
}

const std::wstring ToastNotificationTemplate::ActionLabel(std::size_t position) const
{
    if (position >= m_Actions.size())
    {
        LOG_WARN("ToastNotificationTemplate::ActionLabel() failed.");
        return std::wstring();
    }
    return m_Actions[position];
}

const std::wstring& ToastNotificationTemplate::ImagePath() const
{
    return m_ImagePath;
}

const std::wstring& ToastNotificationTemplate::AudioPath() const
{
    return m_AudioPath;
}

const std::wstring& ToastNotificationTemplate::AttributionText() const
{
    return m_AttributionText;
}

INT64 ToastNotificationTemplate::Expiration() const
{
    return m_Expiration;
}

ToastNotificationTemplate::ToastNotificationTemplateType ToastNotificationTemplate::GetType() const
{
    return m_Type;
}

ToastNotificationTemplate::AudioOption ToastNotificationTemplate::GetAudioOption() const
{
    return m_AudioOption;
}

ToastNotificationTemplate::Duration ToastNotificationTemplate::GetDuration() const
{
    return m_Duration;
}
