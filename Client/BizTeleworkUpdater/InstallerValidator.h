#pragma once

#include <memory>
#include <string>
#include <Windows.h>
#include <Msi.h>
#include "AppVersion.h"

class InstallerValidator
{
public:
    class Config
    {
    public:
        static std::unique_ptr<Config> Default();
        Config(const std::wstring& upgradeCode_, std::unique_ptr<AppVersion> appVersion_);

        const std::wstring upgradeCode;
        const std::unique_ptr<AppVersion> appVersion;
    };

    InstallerValidator();
    explicit InstallerValidator(std::unique_ptr<Config> config);
    virtual ~InstallerValidator() = default;

    virtual bool ValidateMsiFile(const std::wstring& msiFilePath) const;
    virtual bool ValidateSignature(const std::wstring& msiFilePath) const;
    virtual bool ValidateUpgradeCode(const std::wstring& msiFilePath) const;
    virtual bool ValidateVersion(const std::wstring& msiFilePath) const;

protected:
    // Wrapper methods for test of calling Windows API.
    virtual UINT MsiVerifyPackageW(_In_ LPCWSTR szPackagePath) const;
    DWORD GetModuleFileNameW(
        _In_opt_ HMODULE hModule,
        _Out_writes_to_(nSize, ((return < nSize) ? (return +1) : nSize)) LPWSTR lpFilename,
        _In_ DWORD nSize) const;
    HRESULT MsiGetFileSignatureInformationW(
        _In_ LPCWSTR szSignedObjectPath,
        _In_ DWORD dwFlags,
        _Outptr_ PCCERT_CONTEXT* ppcCertContext,
        _Out_writes_bytes_opt_(*pcbHashData) LPBYTE   pbHashData,
        _Inout_opt_ LPDWORD pcbHashData) const;
    DWORD CertGetNameStringW(
        _In_ PCCERT_CONTEXT pCertContext,
        _In_ DWORD dwType,
        _In_ DWORD dwFlags,
        _In_opt_ void* pvTypePara,
        _Out_writes_to_opt_(cchNameString, return) LPWSTR pszNameString,
        _In_ DWORD cchNameString) const;
    BOOL CertFreeCertificateContext(
        _In_opt_ PCCERT_CONTEXT pCertContext) const;
    UINT MsiOpenDatabaseW(
        LPCWSTR      szDatabasePath,
        LPCWSTR      szPersist,
        MSIHANDLE* phDatabase) const;
    UINT MsiCloseHandle(MSIHANDLE hAny) const;

private:
    bool GetModulePath(std::wstring *dst) const;
    bool GetCommonNameFromCert(std::wstring *dst, PCCERT_CONTEXT cert) const;
    bool GetStringFromMsiRecord(std::wstring* dst, MSIHANDLE hRecord) const;
    bool GetStringFromMsiProductProperty(std::wstring* dst, MSIHANDLE hRecord, LPCWSTR szProperty) const;

    const std::unique_ptr<Config> m_Config = nullptr;
};
