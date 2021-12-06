#include "stdafx.h"
#include "HttpStatusCode.h"
#include "UnauthorizedResponsive.h"

UnauthorizedResponsive::UnauthorizedResponsive(const std::wstring& value)
    : Responsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_UNAUTHORIZED, value)
{
}
