#pragma once
#include "Responsive.h"

class UnauthorizedResponsive : public Responsive
{
public:
    explicit UnauthorizedResponsive(const std::wstring& value);
};
