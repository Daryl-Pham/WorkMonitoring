#pragma once
#include "RegistryHelper.h"
#include <atlstr.h>
#include <wincrypt.h>
#include <cryptuiapi.h>
#pragma comment(lib, "Crypt32.lib")

struct AGENT_INFOR
{
    std::wstring companyGUID;
    std::wstring agentGUID;
    std::wstring apiUrl;
};

class AgentCredential;

class AuthenHelper
{
public:
    virtual ~AuthenHelper() = default;
    BOOL CheckAuthenTelework(const std::wstring& companyGUID, std::vector<BYTE> agentGUID, AgentCredential* agentCredential);

    std::vector<BYTE> GetAgentGUIDFromRegistry();
    std::wstring      GetCompanyGUIDFromRegistry();
    bool              SaveAuthentication(const AGENT_INFOR& agentInfor);
    bool              DeleteAuthentication();

    virtual bool      IsCopiedAuthenticationToCurrentUser() const;
    virtual bool      SetCompanyGuid(const std::wstring& companyGuid) const;
    virtual bool      SetAgentGuid(const std::vector<BYTE>& agentGuid) const;
    virtual bool      GetAuthenInforFromLocalMachine(std::wstring* companyGuid, std::vector<BYTE>* agentGuid, std::wstring* apiUrl) const;
    virtual bool      SetApiUrl(const std::wstring& apiUrl) const;
    virtual bool      DeleteAuthenticationFromLocalMachine() const;

    virtual std::unique_ptr<RegistryHelper> CreateRegistryHelper(const HKEY& hKey);
};
