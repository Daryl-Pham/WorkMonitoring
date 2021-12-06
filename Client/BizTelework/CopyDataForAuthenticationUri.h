#pragma once

#include <windows.h>
#include <ActiveInformation.h>

static const GUID Signature = { 0x850677b9, 0xff99, 0x451d, { 0xba, 0xa8, 0x38, 0x3e, 0xdb, 0x5c, 0xd0, 0x76 } };
static const UINT COPYDATA_TYPE_1 = 1;

struct CopyData
{
    GUID signature;
    ActiveInformation infor;
};
