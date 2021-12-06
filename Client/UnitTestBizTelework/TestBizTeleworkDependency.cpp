#include "stdafx.h"
#include "gtest/gtest.h"
#include <BizTeleworkDependency.h>
#include <AuthenHelper.h>

TEST(testBizTeleworkDependency, IsActive)
{
    BizTeleworkDependency dependency;
    std::wstring companyGUID = L"CompanyCode";
    std::vector <BYTE> agentGUID(0);
    EXPECT_FALSE(dependency.IsActive(companyGUID, agentGUID));
    agentGUID.push_back(0);
    EXPECT_TRUE(dependency.IsActive(companyGUID, agentGUID));

    AuthenHelper authenHelper;
    if (!authenHelper.GetAgentGUIDFromRegistry().empty() && !authenHelper.GetCompanyGUIDFromRegistry().empty())
    {
        EXPECT_TRUE(dependency.IsActive());
    }
    else
    {
        EXPECT_FALSE(dependency.IsActive());
    }
}
