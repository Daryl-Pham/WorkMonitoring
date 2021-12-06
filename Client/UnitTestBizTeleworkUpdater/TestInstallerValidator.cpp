#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include "UtilityForTest.h"
#include "InstallerValidator.h"


/*
 * Tests for ValidateMsiFile
 */
TEST(testInstallerValidator, ValidateMsiFile)
{
    // This Test Depends on MsiForTesting.msi of "MsiForTesting" project.
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    ASSERT_TRUE(boost::filesystem::exists(msiFilePath));  // You need build MsiForTesting project if this line failed.

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(L"", nullptr));
    EXPECT_TRUE(v.ValidateMsiFile(msiFilePath.wstring()));
}

TEST(testInstallerValidator, ValidateMsiFile_NotExitMsi)
{
    auto notExistPath = UtilityForTest::GetMsiForTestingPath().parent_path() / L"NotExist.msi";
    ASSERT_FALSE(boost::filesystem::exists(notExistPath));

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(L"", nullptr));
    EXPECT_FALSE(v.ValidateMsiFile(notExistPath.wstring()));
}

TEST(testInstallerValidator, ValidateMsiFile_NotMsi)
{
    auto notMsi = UtilityForTest::GetSelfPath();
    ASSERT_TRUE(boost::filesystem::exists(notMsi));

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(L"", nullptr));
    EXPECT_FALSE(v.ValidateMsiFile(notMsi.wstring()));
}

/*
 * Tests for ValidateUpgradeCode
 */
TEST(testInstallerValidator, ValidateUpgradeCode)
{
    // This Test Depends on MsiForTesting.msi of "MsiForTesting" project.
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    ASSERT_TRUE(boost::filesystem::exists(msiFilePath));  // You need build MsiForTesting project if this line failed.

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, nullptr));
    EXPECT_TRUE(v.ValidateUpgradeCode(msiFilePath.wstring()));
}

TEST(testInstallerValidator, ValidateUpgradeCode_WithDifferentUpgradeCode)
{
    // This Test Depends on MsiForTesting.msi of "MsiForTesting" project.
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    ASSERT_TRUE(boost::filesystem::exists(msiFilePath));  // You need build MsiForTesting project if this line failed.

    InstallerValidator differentUpgradeCode(std::make_unique<InstallerValidator::Config>(L"{BadBadBa-dBad-BadB-adBa-dBeefBadBeef}", nullptr));
    EXPECT_FALSE(differentUpgradeCode.ValidateUpgradeCode(msiFilePath.wstring()));

    InstallerValidator smallCase(std::make_unique<InstallerValidator::Config>(L"{ed6e254e-1c15-4480-8afe-24c958c2bb2e}", nullptr));
    EXPECT_FALSE(smallCase.ValidateUpgradeCode(msiFilePath.wstring()));
}

TEST(testInstallerValidator, ValidateUpgradeCode_NotExitMsi)
{
    auto notExistPath = UtilityForTest::GetMsiForTestingPath().parent_path() / L"NotExist.msi";
    ASSERT_FALSE(boost::filesystem::exists(notExistPath));

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, nullptr));
    EXPECT_FALSE(v.ValidateUpgradeCode(notExistPath.wstring()));
}

/*
 * Tests for ValidateVersion
 */
TEST(testInstallerValidator, ValidateVersion)
{
    // This Test Depends on MsiForTesting.msi of "MsiForTesting" project.
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    ASSERT_TRUE(boost::filesystem::exists(msiFilePath));  // You need build MsiForTesting project if this line failed.

    auto newerVersion = AppVersion::Parse(L"1.1.0.0");
    InstallerValidator newer(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, std::move(newerVersion)));
    EXPECT_TRUE(newer.ValidateVersion(msiFilePath.wstring()));

    auto olderVersion = AppVersion::Parse(L"0.9.0.0");
    InstallerValidator older(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, std::move(olderVersion)));
    EXPECT_TRUE(older.ValidateVersion(msiFilePath.wstring()));

    auto sameVersion = AppVersion::Parse(UtilityForTest::VERSION);
    InstallerValidator same(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, std::move(sameVersion)));
    EXPECT_FALSE(same.ValidateVersion(msiFilePath.wstring()));
}

TEST(testInstallerValidator, ValidateVersion_NotExitMsi)
{
    auto notExistPath = UtilityForTest::GetMsiForTestingPath().parent_path() / L"NotExist.msi";
    ASSERT_FALSE(boost::filesystem::exists(notExistPath));

    InstallerValidator v(std::make_unique<InstallerValidator::Config>(UtilityForTest::EXPECTED_UPGRAD_CODE, nullptr));
    EXPECT_FALSE(v.ValidateVersion(notExistPath.wstring()));
}
