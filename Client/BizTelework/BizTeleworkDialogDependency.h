#pragma once

#include <memory>
#include <string>
#include <LanguageResourceInterface.h>
#include <Request.h>

class BizTeleworkDialogDependency
{
public:
    BizTeleworkDialogDependency() = default;
    virtual ~BizTeleworkDialogDependency() = default;

    virtual LanguageResourceInterface *GetLanguageResource() const;
};
