#include "stdafx.h"
#include <Shlobj.h>
#include "WritingData.h"
#include "PlainTextFileIO.h"
#include <experimental/filesystem>

std::string WritingData::GetFullPathDataFile(const OperateType type)
{
	std::wstring fullPathDataFile;
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path)))
	{
		if (type == MOUSE_OPERATE)
		{
			fullPathDataFile = std::wstring(path) + L"\\MouseOperate.data";
			std::experimental::filesystem::path(fullPathDataFile).string();
		}
		else if (type == KEYBOARD_OPERATE)
		{
			
			fullPathDataFile = std::wstring(path) + L"\\KeyboardOperate.data";
			return std::experimental::filesystem::path(fullPathDataFile).string();
		}
	}
	else return "";
}

bool WritingData::WriteData(const OperateType type)
{

	return false;
}

WritingData::WritingData()
{
	m_pMouseOperateFileIO = std::make_unique<PlainTextFileIO>(GetFullPathDataFile(MOUSE_OPERATE));
	m_pKeyboarOperateFileIO = std::make_unique<PlainTextFileIO>(GetFullPathDataFile(KEYBOARD_OPERATE));
}
