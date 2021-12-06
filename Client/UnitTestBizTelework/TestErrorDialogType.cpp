#include "stdafx.h"
#include "gtest/gtest.h"
#include <BizTeleworkFailedCallApiDialog.h>
#include <BizTeleworkDialogUtility.h>

TEST(testErrorDialogType, WorkStatusStartedError)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE;
    const std::wstring body = L"{\"errors\":[{\"type\":\"Error::WorkStatus::USER_WORK_STARTED\",\"message\":\"Didn't End Yet\"}]}";
    type = BizTeleworkDialogUtility::GetErrorDialogType(400, body);
    EXPECT_EQ(ErrorDialogMessageType::BAD_REQUEST_STARTED_ERROR_TYPE, type);
}

TEST(testErrorDialogType, DayOffRequestStartedError)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE;
    const std::wstring body = L"{\"errors\":[{\"type\":\"Error::DayOffRequest::USER_WORK_STARTED\",\"message\":\"User already start working\"}]}";
    type = BizTeleworkDialogUtility::GetErrorDialogType(400, body);
    EXPECT_EQ(ErrorDialogMessageType::BAD_REQUEST_STARTED_ERROR_TYPE, type);
}

TEST(testErrorDialogType, WorkStatusEndedError)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE;
    const std::wstring body = L"{\"errors\":[{\"type\":\"Error::WorkStatus::USER_WORK_ENDED\",\"message\":\"Didn't Start Yet\"}]}";
    type = BizTeleworkDialogUtility::GetErrorDialogType(400, body);
    EXPECT_EQ(ErrorDialogMessageType::BAD_REQUEST_ENDED_ERROR_TYPE, type);
}

TEST(testErrorDialogType, UnauthorizedError)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE;
    const std::wstring body = L"";
    type = BizTeleworkDialogUtility::GetErrorDialogType(401, body);
    EXPECT_EQ(ErrorDialogMessageType::UNAUTHORIZED_ERROR_TYPE, type);
}

TEST(testErrorDialogType, OtherError)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::BAD_REQUEST_ENDED_ERROR_TYPE;
    const std::wstring body = L"";
    type = BizTeleworkDialogUtility::GetErrorDialogType(500, body);
    EXPECT_EQ(ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE, type);
}
