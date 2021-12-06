#include "pch.h"
#include "ConfigUtility.h"
#include "InstallerValidator.h"
#include <MsiQuery.h>
#include <Log.h>

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}  // namespace

std::unique_ptr<InstallerValidator::Config> InstallerValidator::Config::Default()
{
    auto upgradeCode = ConfigUtility::GetUpgradeCode();
    if (upgradeCode.empty())
    {
        LOG_WARN("ConfigUtility::GetUpgradeCode() failed.");
        return nullptr;
    }

    std::wstring versionStr = ConfigUtility::GetVersion();
    if (versionStr.empty())
    {
        LOG_WARN("ConfigUtility::GetVersion() failed.");
        return nullptr;
    }

    auto appVersion = AppVersion::Parse(versionStr);
    if (!appVersion)
    {
        LOG_WARN("AppVersion::Parse() failed.");
        return nullptr;
    }

    LOG_INFO("The upgradeCode is \"%S\"", upgradeCode.c_str());
    LOG_INFO("The versionStr is \"%S\"", versionStr.c_str());
    return std::make_unique<InstallerValidator::Config>(upgradeCode, std::move(appVersion));
}

InstallerValidator::Config::Config(const std::wstring& upgradeCode_, std::unique_ptr<AppVersion> appVersion_)
    : upgradeCode(upgradeCode_)
    , appVersion(std::move(appVersion_))
{
}

InstallerValidator::InstallerValidator()
    : InstallerValidator(InstallerValidator::Config::Default())
{
}

InstallerValidator::InstallerValidator(std::unique_ptr<InstallerValidator::Config> config)
    : m_Config(std::move(config))
{
}

UINT InstallerValidator::MsiVerifyPackageW(_In_ LPCWSTR szPackagePath) const
{
    return ::MsiVerifyPackageW(szPackagePath);
}

DWORD InstallerValidator::GetModuleFileNameW(
    _In_opt_ HMODULE hModule,
    _Out_writes_to_(nSize, ((return < nSize) ? (return +1) : nSize)) LPWSTR lpFilename,
    _In_ DWORD nSize) const
{
    return ::GetModuleFileNameW(hModule, lpFilename, nSize);
}

HRESULT InstallerValidator::MsiGetFileSignatureInformationW(
    _In_ LPCWSTR szSignedObjectPath,                     // path to the signed object
    _In_ DWORD dwFlags,                                   // special extra error case flags
    _Outptr_ PCCERT_CONTEXT* ppcCertContext,          // returned signer cert context
    _Out_writes_bytes_opt_(*pcbHashData) LPBYTE   pbHashData,   // returned hash buffer, NULL if not desired
    _Inout_opt_ LPDWORD pcbHashData) const
{
    return ::MsiGetFileSignatureInformationW(szSignedObjectPath, dwFlags, ppcCertContext, pbHashData, pcbHashData);
}

DWORD InstallerValidator::CertGetNameStringW(
    _In_ PCCERT_CONTEXT pCertContext,
    _In_ DWORD dwType,
    _In_ DWORD dwFlags,
    _In_opt_ void* pvTypePara,
    _Out_writes_to_opt_(cchNameString, return) LPWSTR pszNameString,
    _In_ DWORD cchNameString) const
{
    return ::CertGetNameStringW(pCertContext, dwType, dwFlags, pvTypePara, pszNameString, cchNameString);
}

BOOL InstallerValidator::CertFreeCertificateContext(
    _In_opt_ PCCERT_CONTEXT pCertContext) const
{
    return ::CertFreeCertificateContext(pCertContext);
}

UINT InstallerValidator::MsiOpenDatabaseW(
    LPCWSTR      szDatabasePath,
    LPCWSTR      szPersist,
    MSIHANDLE* phDatabase) const
{
    return ::MsiOpenDatabaseW(szDatabasePath, szPersist, phDatabase);
}

UINT InstallerValidator::MsiCloseHandle(MSIHANDLE hAny) const
{
    return ::MsiCloseHandle(hAny);
}

bool InstallerValidator::GetModulePath(std::wstring *dst) const
{
    if (!dst)
    {
        LOG_WARN("The dst must be not null.");
        return false;
    }

    std::wstring buf(MAX_PATH, L'\0');
    DWORD len = GetModuleFileName(NULL, &buf[0], static_cast<DWORD>(buf.size()));
    if (len == 0)
    {
        LOG_WARN("GetModuleFileName() failed. %d", GetLastError());
        return false;
    }
    else if (len > buf.size())
    {
        buf.resize(len);
        len = GetModuleFileName(NULL, &buf[0], static_cast<DWORD>(buf.size()));
        if (len == 0)
        {
            LOG_WARN("GetModuleFileName() failed. %d", GetLastError());
            return false;
        }
    }

    buf.resize(len);
    *dst = std::move(buf);

    return true;
}

bool InstallerValidator::GetCommonNameFromCert(std::wstring* dst, PCCERT_CONTEXT cert) const
{
    if (!dst)
    {
        LOG_WARN("The dst must be not null.");
        return false;
    }
    if (!cert)
    {
        LOG_ERROR("The cert must be not null.");
        return false;
    }

    DWORD len = CertGetNameString(cert, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, NULL, 0);
    if (len == 1)  // The len contains length of L'\0'.
    {
        LOG_WARN("CertGetNameString() failed.");
        return false;
    }

    std::wstring buf(len, L'\0');

    len = CertGetNameString(cert, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, &buf[0], len);
    if (len == 1)  // The len contains length of L'\0'.
    {
        LOG_WARN("CertGetNameString() failed.");
        return false;
    }

    *dst = std::move(buf);
    return true;
}

bool InstallerValidator::GetStringFromMsiRecord(std::wstring *dst, MSIHANDLE hRecord) const
{
    const DWORD recordNumber = 1;
    DWORD len = 0;
    DWORD result = MsiRecordGetString(hRecord, recordNumber, NULL, &len);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiRecordGetString() must return ERROR_SUCCESS but %u", result);
        return false;
    }

    len++;
    std::wstring buf(len, L'\0');
    result = MsiRecordGetString(hRecord, recordNumber, &buf[0], &len);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiRecordGetString() failed. %u", result);
        return false;
    }
    buf.resize(len);

    *dst = std::move(buf);

    return true;
}

bool InstallerValidator::GetStringFromMsiProductProperty(std::wstring* dst, MSIHANDLE hMsi, LPCWSTR szProperty) const
{
    DWORD len = 0;
    UINT result = MsiGetProductProperty(hMsi, szProperty, nullptr, &len);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiGetProductProperty() failed. %u", result);
        return false;
    }

    len++;
    std::wstring buf(len, L'\0');
    result = MsiGetProductProperty(hMsi, szProperty, &buf[0], &len);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiGetProductProperty() failed. %u", result);
        return false;
    }
    buf.resize(len);

    *dst = std::move(buf);

    return true;
}

bool InstallerValidator::ValidateMsiFile(const std::wstring& msiFilePath) const
{
    UINT r = MsiVerifyPackage(msiFilePath.c_str());
    if (r != ERROR_SUCCESS)
    {
        LOG_WARN("MsiVerifyPackage() failed. %u", r);
        return false;
    }

    return true;
}

bool InstallerValidator::ValidateSignature(const std::wstring& msiFilePath) const
{
    std::wstring selfPath;
    if (!GetModulePath(&selfPath))
    {
        LOG_WARN("GetModulePath() failed.");
        return false;
    }

    PCCERT_CONTEXT selfCert = nullptr;
    PCCERT_CONTEXT msiCert = nullptr;
    HRESULT hr;
    if (FAILED((hr = MsiGetFileSignatureInformation(selfPath.c_str(), MSI_INVALID_HASH_IS_FATAL, &selfCert, NULL, NULL))))
    {
        LOG_WARN("MsiGetFileSignatureInformation() failed. 0x%08x", hr);
        return false;
    }
    else if (FAILED((hr = MsiGetFileSignatureInformation(msiFilePath.c_str(), MSI_INVALID_HASH_IS_FATAL, &msiCert, NULL, NULL))))
    {
        LOG_WARN("MsiGetFileSignatureInformation() failed. 0x%08x", hr);
        CertFreeCertificateContext(selfCert);
        return false;
    }

    std::wstring selfCN;
    std::wstring msiCN;
    if (!GetCommonNameFromCert(&selfCN, selfCert))
    {
        LOG_WARN("GetCommonNameFromCert() failed.");
        CertFreeCertificateContext(selfCert);
        CertFreeCertificateContext(msiCert);
        return false;
    }
    else if (!GetCommonNameFromCert(&msiCN, msiCert))
    {
        LOG_WARN("GetCommonNameFromCert() failed.");
        CertFreeCertificateContext(selfCert);
        CertFreeCertificateContext(msiCert);
        return false;
    }

    CertFreeCertificateContext(selfCert);
    CertFreeCertificateContext(msiCert);

    if (selfCN != msiCN)
    {
        LOG_WARN("The Common Name is not same. \"%S\" != \"%S\"", selfCN.c_str(), msiCN.c_str());
        return false;
    }

    LOG_INFO("Valid signature.");
    return true;
}

bool InstallerValidator::ValidateUpgradeCode(const std::wstring &msiFilePath) const
{
    if (!m_Config)
    {
        LOG_WARN("The m_Config must not be null.");
        return false;
    }

    MSIHANDLE hMsi = NULL;
    UINT result = MsiOpenDatabase(msiFilePath.c_str(), MSIDBOPEN_READONLY, &hMsi);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiOpenDatabase(\"%S\") failed. %u", msiFilePath.c_str(), result);
        return false;
    }

    const LPCTSTR QUERY_UPGRADECODE = _T("SELECT `UpgradeCode` FROM `Upgrade`");
    MSIHANDLE hView = NULL;
    result = MsiDatabaseOpenView(hMsi, QUERY_UPGRADECODE, &hView);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiDatabaseOpenView() failed. %u", result);
        MsiCloseHandle(hMsi);
        return false;
    }

    result = MsiViewExecute(hView, NULL);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiViewExecute() failed. %u", result);
        MsiCloseHandle(hMsi);
        return false;
    }

    while (true)
    {
        MSIHANDLE hRecord = NULL;

        result = MsiViewFetch(hView, &hRecord);
        if (result == ERROR_SUCCESS)
        {
            std::wstring msiUpgradeCode;
            if (!GetStringFromMsiRecord(&msiUpgradeCode, hRecord))
            {
                LOG_WARN("GetStringFromMsiRecord() failed.");

                MsiCloseHandle(hRecord);
                MsiCloseHandle(hView);
                MsiCloseHandle(hMsi);

                return false;
            }

            LOG_INFO("The msiUpgradeCode is \"%S\".", msiUpgradeCode.c_str());
            MsiCloseHandle(hRecord);
            MsiCloseHandle(hView);
            MsiCloseHandle(hMsi);

            if (msiUpgradeCode != m_Config->upgradeCode)
            {
                LOG_WARN("The msiUpgradeCode must be \"%S\" but \"%S\"", m_Config->upgradeCode.c_str(), msiUpgradeCode.c_str());
                return false;
            }

            LOG_INFO("Valid upgrade code.");
            return true;
        }

        if (result == ERROR_NO_MORE_ITEMS)
        {
            LOG_INFO("There are no more items from.");
        }
        else
        {
            LOG_WARN("MsiViewFetch() returns unexpected value %u", result);
        }

        if (hRecord)
        {
            MsiCloseHandle(hRecord);
        }
    }

    if (hView)
    {
        MsiCloseHandle(hView);
    }
    if (hMsi)
    {
        MsiCloseHandle(hMsi);
    }

    return false;
}

bool InstallerValidator::ValidateVersion(const std::wstring& msiFilePath) const
{
    if (!m_Config)
    {
        LOG_WARN("The m_Config must not be null.");
        return false;
    }

    MSIHANDLE hMsi = NULL;
    UINT result = MsiOpenPackageEx(msiFilePath.c_str(), MSIOPENPACKAGEFLAGS_IGNOREMACHINESTATE, &hMsi);
    if (result != ERROR_SUCCESS)
    {
        LOG_WARN("MsiOpenPackageEx() failed. %u", result);
        return false;
    }

    std::wstring msiVersion;
    const LPCTSTR PRODUCT_VERSION = _T("ProductVersion");
    if (!GetStringFromMsiProductProperty(&msiVersion, hMsi, PRODUCT_VERSION))
    {
        LOG_WARN("GetStringFromMsiProductProperty() failed.");
        MsiCloseHandle(hMsi);
        return false;
    }

    MsiCloseHandle(hMsi);

    auto msiAppVersion = AppVersion::Parse(msiVersion);
    if (!msiAppVersion)
    {
        LOG_WARN("AppVersion::Parse() failed.");
        return false;
    }

    if (m_Config->appVersion->IsEqual(*msiAppVersion))
    {
        LOG_WARN("The app version must not be same. %S", msiVersion.c_str());
        return false;
    }

    LOG_INFO("Valid version.");

    return true;
}
