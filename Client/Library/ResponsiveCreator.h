#pragma once
#include "HttpStatusCode.h"
#include "Responsive.h"
#include "OkieResponsive.h"
#include "TimeCardOkieResponsive.h"
#include "AuthenticateOkieResponsive.h"
#include "WorkLogOkieResponsive.h"
#include "CreatedResponsive.h"
#include "BadRequestResponsive.h"
#include "UnauthorizedResponsive.h"
#include "ServerErrorResponsive.h"

namespace
{
    const wchar_t* DistinctionTimeCardOkResponsive = L"status";
    const wchar_t* DistinctionAuthenOkResponsive = L"company_guid";
    const wchar_t* DistinctionWorkLogOkResponsive = L"self_management_url";
}

class OKieResponsiveCreator
{
public:
    static std::unique_ptr<Responsive> GetOkieResponsive(const std::wstring& body)
    {
        std::unique_ptr<Responsive> res;
        if (body.find(DistinctionTimeCardOkResponsive, 0) != std::wstring::npos)
        {
            res.reset(new TimeCardOkieResponsive(body));
        }
        else if (body.find(DistinctionAuthenOkResponsive, 0) != std::wstring::npos)
        {
            res.reset(new AuthenticateOkieResponsive(body));
        }
        else if (body.find(DistinctionWorkLogOkResponsive, 0) != std::wstring::npos)
        {
            res.reset(new WorkLogOkieResponsive(body));
        }
        return std::move(res);
    }
};

class ResponsiveCreator
{
public:
    static std::unique_ptr<Responsive> GetResponsive(const HTTP_STATUS_CODE code, const std::wstring& body)
    {
        std::unique_ptr<Responsive> res;
        switch (code)
        {
        case HTTP_STATUS_CODE::HTTP_STATUS_CODE_OK:
            res = std::move(OKieResponsiveCreator::GetOkieResponsive(body));
            break;
        case HTTP_STATUS_CODE::HTTP_STATUS_CODE_CREATED:
            res.reset(new CreatedResponsive(body));
            break;
        case HTTP_STATUS_CODE::HTTP_STATUS_CODE_BAD_REQUEST:
            res.reset(new BadRequestResponsive(body));
            break;
        case HTTP_STATUS_CODE::HTTP_STATUS_CODE_UNAUTHORIZED:
            res.reset(new UnauthorizedResponsive(body));
            break;
        case HTTP_STATUS_CODE::HTTP_STATUS_CODE_SERVER_ERROR:
            res.reset(new ServerErrorResponsive(body));
            break;
        default:
            res = nullptr;
            break;
        }

        return res;
    }
};
