#pragma once
#include "Responsive.h"

class ServerErrorResponsive : public Responsive
{
public:
    explicit ServerErrorResponsive(const std::wstring& value);
};
