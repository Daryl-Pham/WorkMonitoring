#pragma once
#include "OkieResponsive.h"

enum class TimeCardOkieStatus
{
    ERROR_DUTY,
    ON_DUTY,
    OFF_DUTY,
};

enum class WorkDayTargetStatus
{
    NULL_STATUS,
    BLANK_STATUS,
    VARIABLE_STATUS,
};

struct WorkDayTarget
{
    WorkDayTargetStatus targetStatus;
    std::wstring content;
};


class TimeCardOkieResponsive : public OkieResponsive
{
public:
    explicit TimeCardOkieResponsive(const std::wstring& value);
    TimeCardOkieStatus   GetStatus() const;
    int64_t GetUnixTimeStartedAt() const;
    [[nodiscard]] WorkDayTarget GetWorkDayTarget() const;

    bool ParseValue();

private:
    bool GetStartedAtFromBody(std::wstring* startedAt, const std::wstring& body);
    bool GetWorkTargetFromBody(std::wstring* workDayTarget, const std::wstring_view& body);
    bool ClassifyWorkTarget(const std::wstring_view& workTargetString);

    TimeCardOkieStatus m_Status = TimeCardOkieStatus::ERROR_DUTY;
    int64_t            m_UnixTimeStartedAt = 0;
    WorkDayTarget      m_WorkTarget;

};
