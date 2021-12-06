#include "stdafx.h"

/* Fix for issue https://github.com/Microsoft/vcpkg/issues/836
 */
#define _TURN_OFF_PLATFORM_STRING

#include "BizTeleworkDialogDependency.h"
#include <AuthenHelper.h>
#include <CpprestRequestImpl.h>
#include <LanguageResource.h>
#include "GettingRequest.h"
#include <Log.h>

LanguageResourceInterface *BizTeleworkDialogDependency::GetLanguageResource() const
{
    return LanguageResource::Singleton();
}
