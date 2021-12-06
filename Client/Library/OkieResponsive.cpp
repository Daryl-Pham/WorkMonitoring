#include "stdafx.h"
#include "HttpStatusCode.h"
#include "OkieResponsive.h"

OkieResponsive::OkieResponsive(const std::wstring& value)
    : Responsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_OK, value)
{
}
