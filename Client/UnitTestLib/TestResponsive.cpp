#include "stdafx.h"
#include "gtest/gtest.h"
#include "Responsive.h"
#include "ResponsiveCreator.h"

TEST(testResponsive_NotRunOnCI, OKieOnDuty)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_OK, L"{\"status\":\"on_duty\",\"started_at\":\"2020-07-17T08:48:16Z\"}");
    TimeCardOkieResponsive* timeCardResOkie = dynamic_cast<TimeCardOkieResponsive*>(res.get());
    EXPECT_NE(nullptr, timeCardResOkie);
    EXPECT_TRUE(timeCardResOkie->ParseValue());
    EXPECT_EQ(TimeCardOkieStatus::ON_DUTY, timeCardResOkie->GetStatus());
    EXPECT_GT(0, timeCardResOkie->GetUnixTimeStartedAt());
}

TEST(testResponsive, OKieOffDuty)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_OK, L"{\"status\":\"off_duty\"}");
    TimeCardOkieResponsive* timeCardResOkie = dynamic_cast<TimeCardOkieResponsive*>(res.get());
    EXPECT_NE(nullptr, timeCardResOkie);
    EXPECT_TRUE(timeCardResOkie->ParseValue());
    EXPECT_EQ(TimeCardOkieStatus::OFF_DUTY, timeCardResOkie->GetStatus());
}

TEST(testResponsive, OKieInvalidBody)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_OK, L"invalid");
    TimeCardOkieResponsive* timeCardResOkie = dynamic_cast<TimeCardOkieResponsive*>(res.get());
    EXPECT_EQ(nullptr, timeCardResOkie);
}

TEST(testResponsive, ResponsiveCreated)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_CREATED, L"body");
    TimeCardOkieResponsive* timeCardResOkie = dynamic_cast<TimeCardOkieResponsive*>(res.get());
    EXPECT_EQ(nullptr, timeCardResOkie);
    CreatedResponsive* resCreated = dynamic_cast<CreatedResponsive*>(res.get());
    EXPECT_NE(nullptr, resCreated);
}

TEST(testResponsive, ResponsiveBadRequest)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_BAD_REQUEST, L"body");
    BadRequestResponsive* resBadRequest = dynamic_cast<BadRequestResponsive*>(res.get());
    EXPECT_NE(nullptr, resBadRequest);
}

TEST(testResponsive, ResponsiveUnAuthorized)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_UNAUTHORIZED, L"body");
    UnauthorizedResponsive* resUnAuthor = dynamic_cast<UnauthorizedResponsive*>(res.get());
    EXPECT_NE(nullptr, resUnAuthor);
}

TEST(testResponsive, ResponsiveInternalServerError)
{
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_SERVER_ERROR, L"body");
    ServerErrorResponsive* resServerError = dynamic_cast<ServerErrorResponsive*>(res.get());
    EXPECT_NE(nullptr, resServerError);
}
