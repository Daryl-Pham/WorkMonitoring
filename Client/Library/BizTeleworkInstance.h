#pragma once
#include <boost/filesystem.hpp>
#include <memory>
#include "LanguageResourceInterface.h"

struct HandlerDeleter
{
    void operator()(HANDLE hObject) const;
};

class BizTeleworkInstance
{
public:
    BizTeleworkInstance();
    virtual ~BizTeleworkInstance();
    bool CheckAlready();
    virtual LanguageResourceInterface* GetLanguageResource() const;

private:
    std::unique_ptr<std::remove_pointer<HANDLE>::type, HandlerDeleter> m_HandleMutexOneInstance = nullptr;
};
