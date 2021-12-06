#include "stdafx.h"
#include "HttpStatusCode.h"
#include "CreatedResponsive.h"

CreatedResponsive::CreatedResponsive(const std::wstring& value)
    : Responsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_CREATED, value)
{
}
