#include "stdafx.h"
#include "Common.h"
#include "BizTeleworkInstance.h"
#include "LanguageResource.h"
#include <Language.h>
#include <boost/exception/diagnostic_information.hpp>
#include "Log.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* MutexName = L"{BA49C45E-B29A-4359-A07C-51B65B5571AD}";

}  // namespace

void HandlerDeleter::operator()(HANDLE hObject) const
{
    if ((hObject != INVALID_HANDLE_VALUE) && (!::CloseHandle(hObject)))
    {
        LOG_WARN("CloseHandle(%p) is failed. 0x%08x", hObject, GetLastError());
    }
}

BizTeleworkInstance::BizTeleworkInstance()
{
    if (m_HandleMutexOneInstance)
    {
        ::ReleaseMutex(m_HandleMutexOneInstance.get());
        ::CloseHandle(m_HandleMutexOneInstance.get());
    }
}

BizTeleworkInstance::~BizTeleworkInstance()
{
    if (m_HandleMutexOneInstance)
    {
        ::ReleaseMutex(m_HandleMutexOneInstance.get());
    }
}

bool BizTeleworkInstance::CheckAlready()
{
    m_HandleMutexOneInstance.reset(::CreateMutex(NULL, TRUE, MutexName));
    bool alreadyRunning((::GetLastError() == ERROR_ALREADY_EXISTS));
    if (m_HandleMutexOneInstance == nullptr || alreadyRunning)
    {
        return true;
    }
    LOG_INFO("Not exist instance of Optimal Biz Telework.");
    return false;
}

LanguageResourceInterface* BizTeleworkInstance::GetLanguageResource() const
{
    return LanguageResource::Singleton();
}
