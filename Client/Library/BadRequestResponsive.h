#pragma once
#include "Responsive.h"

enum class BadStatus
{
    BAD_WORK_STATUS_STARTED,
    BAD_WORK_STATUS_ENDED,
    BAD_DAY_OFF_REQUEST_STARTED,
    BAD_COMMON_ERROR,
};

class BadRequestResponsive : public Responsive
{
public:
    explicit BadRequestResponsive(const std::wstring& value);
    bool ParseValue();
    BadStatus GetStatus();

private:
    BadStatus m_Status = BadStatus::BAD_COMMON_ERROR;
};
