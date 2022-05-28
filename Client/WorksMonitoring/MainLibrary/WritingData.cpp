#include "stdafx.h"
#include "WritingData.h"
#include <Shlobj.h>

std::wstring WritingData::GetFullPathDataFile(const OperateType type)
{
	std::wstring fullPathDataFile;
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path)))
	{
		if (type == MOUSE_OPERATE)
		{
			fullPathDataFile = std::wstring(path) + L"\\MouseOperate.data";
			return fullPathDataFile;
		}
		else if (type == KEYBOARD_OPERATE)
		{
			
			fullPathDataFile = std::wstring(path) + L"\KeyboardOperate.data";
			return fullPathDataFile;
		}
	}
	else return L"";
}

bool WritingData::WriteData(const OperateType type)
{
	return false;
}

WritingData::WritingData()
{

}
