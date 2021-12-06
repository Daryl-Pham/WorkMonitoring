// UnitTestLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AuthenHelper.h"
#include "RegistryHelper.h"
#include "Common.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <vector>
#include <utility>
#include "Logging.h"
#include <iostream>

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Eq;

//Test function AuthenHelper::CheckAuthenTelework
//Got CompanyGUID, got right AgentGUID, right entropy, expect return TRUE
TEST(testCheckAuthen_NotRunOnCI, gotCompanyGUID_0)
{
    AuthenHelper auHelper;
    std::wstring companyGUID = L"CompanyGUID";
    AgentCredential dataOut = {};
    DATA_BLOB entropy = OptionalEntropy;
    BOOL checkAuthen = auHelper.CheckAuthenTelework(companyGUID, auHelper.GetAgentGUIDFromRegistry(), &dataOut);
    EXPECT_TRUE(checkAuthen);
}

//Test function AuthenHelper::CheckAuthenTelework
//Got CompanyGUID, got wrong AgentGUID, right entropy, expect return FALSE
TEST(testCheckAuthen_NotRunOnCI, gotCompanyGUID_1)
{
    AuthenHelper auHelper;
    std::wstring companyGUID = L"CompanyGUID";
    AgentCredential dataOut = {};
    DATA_BLOB entropy = OptionalEntropy;
    std::vector<BYTE> agentGUID = auHelper.GetAgentGUIDFromRegistry();
    if (agentGUID.empty())
    {
        return;
    }
    agentGUID[2] = 4;//Make agentGUID wrong
    BOOL checkAuthen = auHelper.CheckAuthenTelework(companyGUID, agentGUID, &dataOut);
    EXPECT_FALSE(checkAuthen);
}

class MockAuthenHelper : public AuthenHelper
{
public:
    virtual ~MockAuthenHelper() {}
    MOCK_METHOD(std::unique_ptr<RegistryHelper>, CreateRegistryHelper, (const HKEY&));
};

class MockRegistryHelper : public RegistryHelper
{
public:
    virtual ~MockRegistryHelper() {}
    MOCK_METHOD(bool, DeleteRegistryValue, (const std::wstring&, const std::wstring&));
};

TEST(testAuthenHelper, DeleteAuthenticationSuccessfully)
{
    std::unique_ptr<MockRegistryHelper> registryHelperCompany(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperCompany, DeleteRegistryValue(_, _)).WillOnce(Return(true));

    std::unique_ptr<MockRegistryHelper> registryHelperAgent(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperAgent, DeleteRegistryValue(_, _)).WillOnce(Return(true));

    std::unique_ptr<MockRegistryHelper> registryHelperApiUrl(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperApiUrl, DeleteRegistryValue(_, _)).WillOnce(Return(true));

    std::unique_ptr<MockAuthenHelper> authenHelper(new MockAuthenHelper());

    EXPECT_CALL(*authenHelper, CreateRegistryHelper(_))
        .WillOnce(Return(ByMove(std::move(registryHelperCompany))))
        .WillOnce(Return(ByMove(std::move(registryHelperAgent))))
        .WillOnce(Return(ByMove(std::move(registryHelperApiUrl))));

    EXPECT_TRUE(authenHelper->DeleteAuthentication());
}

TEST(testAuthenHelper, DeleteAuthenticationFailed)
{
    std::unique_ptr<MockRegistryHelper> registryHelperCompany(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperCompany, DeleteRegistryValue(_, _)).WillOnce(Return(false));

    std::unique_ptr<MockRegistryHelper> registryHelperAgent(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperAgent, DeleteRegistryValue(_, _)).WillOnce(Return(false));

    std::unique_ptr<MockRegistryHelper> registryHelperApiUrl(new MockRegistryHelper());
    EXPECT_CALL(*registryHelperApiUrl, DeleteRegistryValue(_, _)).WillOnce(Return(false));

    std::unique_ptr<MockAuthenHelper> authenHelper(new MockAuthenHelper());

    EXPECT_CALL(*authenHelper, CreateRegistryHelper(_))
        .WillOnce(Return(ByMove(std::move(registryHelperCompany))))
        .WillOnce(Return(ByMove(std::move(registryHelperAgent))))
        .WillOnce(Return(ByMove(std::move(registryHelperApiUrl))));

    EXPECT_FALSE(authenHelper->DeleteAuthentication());
}
