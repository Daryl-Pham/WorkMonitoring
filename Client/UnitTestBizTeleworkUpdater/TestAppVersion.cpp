#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <AppVersion.h>

TEST(testAppVersion, Parse)
{
    auto ap = AppVersion::Parse(L"1.2.3.4");
    ASSERT_NE(nullptr, ap);
    EXPECT_EQ(1, ap->major);
    EXPECT_EQ(2, ap->minor);
    EXPECT_EQ(3, ap->patch);
    EXPECT_EQ(4, ap->build);

    EXPECT_EQ(nullptr, AppVersion::Parse(L"1.2.3.4\n\n"));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"\n1.2.3.4\n"));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"1.2.3.4 "));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"  1.2.3.4"));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"1.2.3"));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"1.2"));
    EXPECT_EQ(nullptr, AppVersion::Parse(L"1"));
}

TEST(testAppVersion, IsNewerThan)
{
    auto current = AppVersion::Parse(L"1.2.3.4");

    // same version
    EXPECT_FALSE(current->IsNewerThan(*current));

    // older verions
    EXPECT_TRUE(current->IsNewerThan(*AppVersion::Parse(L"1.2.3.0")));
    EXPECT_TRUE(current->IsNewerThan(*AppVersion::Parse(L"1.2.2.4")));
    EXPECT_TRUE(current->IsNewerThan(*AppVersion::Parse(L"1.1.3.4")));
    EXPECT_TRUE(current->IsNewerThan(*AppVersion::Parse(L"0.2.3.4")));
    EXPECT_TRUE(AppVersion::Parse(L"1.3.4.2")->IsNewerThan(*current));

    // newer versions
    EXPECT_FALSE(current->IsNewerThan(*AppVersion::Parse(L"1.2.3.5")));
    EXPECT_FALSE(current->IsNewerThan(*AppVersion::Parse(L"1.2.4.4")));
    EXPECT_FALSE(current->IsNewerThan(*AppVersion::Parse(L"1.3.3.4")));
    EXPECT_FALSE(current->IsNewerThan(*AppVersion::Parse(L"2.2.3.4")));
    EXPECT_FALSE(current->IsNewerThan(*AppVersion::Parse(L"1.3.4.2")));
}

TEST(testAppVersion, IsEqual)
{
    auto v = AppVersion::Parse(L"1.2.3.4");
    EXPECT_TRUE(v->IsEqual(*AppVersion::Parse(L"1.2.3.4")));
    EXPECT_FALSE(v->IsEqual(*AppVersion::Parse(L"1.2.3.0")));
    EXPECT_FALSE(v->IsEqual(*AppVersion::Parse(L"1.2.0.4")));
    EXPECT_FALSE(v->IsEqual(*AppVersion::Parse(L"1.0.3.4")));
    EXPECT_FALSE(v->IsEqual(*AppVersion::Parse(L"0.2.3.4")));
}
