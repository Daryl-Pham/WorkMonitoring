#include "stdafx.h"
#include "Common.h"
#include "Environment.h"
#include "RegistryHelper.h"
#include "AuthenHelper.h"
#include "Utility.h"
#include "Log.h"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/format.hpp>
#include "AppConfig.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* CompanyGuidRegistryKey = L"CompanyGuid";

    const wchar_t* AgentGuidRegistryKey = L"AgentGuid";

    const wchar_t* ApiUrlRegistryKey = L"ApiUrl";

    const CString subKeyAgentInfor = Environment::GetInstance()->IsWindowsOS64() ? TeleworkAgentInfor64Reg : TeleworkAgentInfor32Reg;

    const CString subKeyTelework = Environment::GetInstance()->IsWindowsOS64() ? Telework64Reg : Telework32Reg;

}  // namespace

BOOL AuthenHelper::CheckAuthenTelework(const std::wstring& companyGUID, std::vector<BYTE> agentGUID, AgentCredential* agentCredential)
{
    if (!agentCredential)
    {
        LOG_WARN("The agentCredential should not be null.");
        return FALSE;
    }

    DATA_BLOB blobSrc = { static_cast<DWORD>(agentGUID.size()), reinterpret_cast<BYTE*>(&agentGUID[0]) };
    DATA_BLOB dataOut;

    if (::CryptUnprotectData(
        &blobSrc,
        nullptr,
        const_cast<DATA_BLOB*>(&OptionalEntropy),
        nullptr,
        nullptr,
        CRYPTPROTECT_LOCAL_MACHINE | CRYPTPROTECT_UI_FORBIDDEN,
        &dataOut))
    {
        DataBlob bizBlog = {};

        bizBlog.cbData = dataOut.cbData;
        bizBlog.pbData.reset(dataOut.pbData, ::LocalFree);

        if (bizBlog.cbData != sizeof(BizGuid))
        {
            return FALSE;
        }

        std::wstring strAgentGUID;
        try
        {
            std::wstringstream wss;
            strAgentGUID.resize(bizBlog.cbData * 2);
            for (DWORD i = 0; i < bizBlog.cbData; i++)
            {
                wss << boost::basic_format<wchar_t>(L"%02x") % bizBlog.pbData.get()[i];
            }
            strAgentGUID = wss.str();
        }
        catch (const boost::exception& e)
        {
            LOG_ERROR("Failed by boost::exception %s", boost::diagnostic_information_what(e));
            return FALSE;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("std::exception occured. %s", e.what());
            return FALSE;
        }

        AgentCredential credential = { companyGUID, strAgentGUID };
        *agentCredential = credential;

        return TRUE;
    }
    else
    {
        LOG_WARN("::CryptUnprotectData() failed(). 0x % 08x", GetLastError());
        return FALSE;
    }
}

std::wstring AuthenHelper::GetCompanyGUIDFromRegistry()
{
    RegistryHelper regHelper(HKEY_CURRENT_USER);
    // Get CompanyGuid from register information of Biz Telework
    return regHelper.GetRegistryStringValue(subKeyAgentInfor, CompanyGuidRegistryKey);
}

std::vector<BYTE> AuthenHelper::GetAgentGUIDFromRegistry()
{
    RegistryHelper regHelper(HKEY_CURRENT_USER);
    return regHelper.GetRegistryBinaryValue(subKeyAgentInfor, AgentGuidRegistryKey);
}

bool AuthenHelper::SaveAuthentication(const AGENT_INFOR& agentInfor)
{
    RegistryHelper reg(HKEY_CURRENT_USER);

    try
    {
        BizGuid bizAgentGuid = boost::lexical_cast<BizGuid>(agentInfor.agentGUID);
        DATA_BLOB blobSrc = { sizeof(BizGuid), reinterpret_cast<BYTE*>(&bizAgentGuid) };
        DATA_BLOB dataOut;

        if (::CryptProtectData(
            &blobSrc,
            nullptr,
            const_cast<DATA_BLOB*>(&OptionalEntropy),
            nullptr,
            nullptr,
            CRYPTPROTECT_LOCAL_MACHINE,
            &dataOut))
        {
            DataBlob result = {};
            result.cbData = dataOut.cbData;
            result.pbData.reset(dataOut.pbData, ::LocalFree);

            std::vector<BYTE> agentGUIDEncrypted(result.pbData.get(), result.pbData.get() + result.cbData);

            if (!SetCompanyGuid(agentInfor.companyGUID) || !SetAgentGuid(agentGUIDEncrypted))
            {
                return false;
            }

            if (!agentInfor.apiUrl.empty() && !SetApiUrl(agentInfor.apiUrl))
            {
                LOG_WARN("Set apiUrl failed.");
            }

            AppConfig appConfig;
            if (!appConfig.SetCopiedAuthentication(true))
            {
                LOG_WARN("SetCopiedAuthentication() failed.");
            }
        }
        else
        {
            LOG_WARN("::CryptProtectData() failed(). 0x % 08x", GetLastError());
            return false;
        }
    }
    catch (const boost::bad_lexical_cast& lexicalException)
    {
        LOG_ERROR("Failed by boost::bad_lexical_cast %s.", lexicalException.what());
    }
    catch (const std::exception& exception)
    {
        LOG_ERROR("Failed std::exception failed() %s.", exception.what());
    }

    return true;
}

bool AuthenHelper::DeleteAuthentication()
{
    bool resDeleteCompanyGuid = false;
    if ((resDeleteCompanyGuid = CreateRegistryHelper(HKEY_CURRENT_USER)->DeleteRegistryValue(subKeyAgentInfor.GetString(), CompanyGuidRegistryKey)) == false)
    {
        LOG_WARN("AuthenHelper::DeleteRegistryValue() company guid failed.");
    }

    bool resDeleteAgentGuid = false;
    if ((resDeleteAgentGuid = CreateRegistryHelper(HKEY_CURRENT_USER)->DeleteRegistryValue(subKeyAgentInfor.GetString(), AgentGuidRegistryKey)) == false)
    {
        LOG_WARN("AuthenHelper::DeleteRegistryValue() agent guid failed.");
    }

    bool resDeleteApiUrl = false;
    if ((resDeleteApiUrl = CreateRegistryHelper(HKEY_CURRENT_USER)->DeleteRegistryValue(subKeyTelework.GetString(), ApiUrlRegistryKey)) == false)
    {
        LOG_WARN("AuthenHelper::DeleteRegistryValue() api url failed.");
    }

    if (!resDeleteCompanyGuid && !resDeleteAgentGuid && !resDeleteApiUrl)
    {
        return false;
    }

    return true;
}

bool AuthenHelper::IsCopiedAuthenticationToCurrentUser() const
{
    bool isCopiedAuthentication = false;
    AppConfig appConfig;

    if ((appConfig.GetCopiedAuthentication(&isCopiedAuthentication)) && (isCopiedAuthentication == true))
    {
        return true;
    }

    return false;
}

bool AuthenHelper::SetCompanyGuid(const std::wstring& companyGuid) const
{
    RegistryHelper regHelperSetCompanyGuid(HKEY_CURRENT_USER);
    return regHelperSetCompanyGuid.SetRegistryStringValue(subKeyAgentInfor.GetString(), CompanyGuidRegistryKey, companyGuid);
}

bool AuthenHelper::SetAgentGuid(const std::vector<BYTE>& agentGuid) const
{
    RegistryHelper regHelperSetAgentGuid(HKEY_CURRENT_USER);
    return regHelperSetAgentGuid.SetRegistryBinaryValue(subKeyAgentInfor.GetString(), AgentGuidRegistryKey, agentGuid);
}

bool AuthenHelper::GetAuthenInforFromLocalMachine(std::wstring* companyGuid, std::vector<BYTE>* agentGuid, std::wstring* apiUrl) const
{
    RegistryHelper regHelperQueryCompanyGuid;
    *companyGuid = regHelperQueryCompanyGuid.GetRegistryStringValue(subKeyAgentInfor, CompanyGuidRegistryKey);
    if ((*companyGuid).empty())
    {
        LOG_WARN("Get company guid from local machine failed.");
        return false;
    }

    RegistryHelper regHelperQueryAgentGuid;
    *agentGuid = regHelperQueryAgentGuid.GetRegistryBinaryValue(subKeyAgentInfor, AgentGuidRegistryKey);
    if ((*agentGuid).empty())
    {
        LOG_WARN("Get agent guid from local machine failed.");
        return false;
    }

    RegistryHelper regHelperQueryApiUrl;
    *apiUrl = regHelperQueryApiUrl.GetRegistryStringValue(subKeyTelework, ApiUrlRegistryKey);
    if ((*apiUrl).empty())
    {
        LOG_WARN("Get api url from local machine failed.");
    }

    return true;
}

bool AuthenHelper::SetApiUrl(const std::wstring& apiUrl) const
{
    RegistryHelper regHelperSetApiUrl(HKEY_CURRENT_USER);
    return regHelperSetApiUrl.SetRegistryStringValue(subKeyTelework.GetString(), ApiUrlRegistryKey, apiUrl);
}

bool AuthenHelper::DeleteAuthenticationFromLocalMachine() const
{
    RegistryHelper regCompany;
    if (!regCompany.DeleteRegistryValue(subKeyAgentInfor.GetString(), CompanyGuidRegistryKey))
    {
        LOG_WARN("AuthenHelper::DeleteRegistryValue() company guid failed.");
        return false;
    }

    RegistryHelper regAgent;
    if (!regAgent.DeleteRegistryValue(subKeyAgentInfor.GetString(), AgentGuidRegistryKey))
    {
        LOG_WARN("AuthenHelper::DeleteRegistryValue() agent guid failed.");
        return false;
    }

    return true;
}

std::unique_ptr<RegistryHelper> AuthenHelper::CreateRegistryHelper(const HKEY& hKey)
{
    return std::make_unique<RegistryHelper>(hKey);
}
