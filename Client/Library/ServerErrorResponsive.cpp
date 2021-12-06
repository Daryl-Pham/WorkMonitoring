#include "stdafx.h"
#include "HttpStatusCode.h"
#include "ServerErrorResponsive.h"

ServerErrorResponsive::ServerErrorResponsive(const std::wstring& value)
    : Responsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_SERVER_ERROR, value)
{
}
