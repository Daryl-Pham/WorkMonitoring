#define __STDC_WANT_LIB_EXT1__ 1
#define _XOPEN_SOURCE

#include "stdafx.h"
#include <Shlobj.h>
#include "WritingData.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include <time.h>

static std::wstring pathMouseOperate = L"";
static std::wstring pathKeyboardOperate = L"";
std::wstring WritingData::GetFullPathDataFile(const OperateType type)
{
	std::wstring fullPathDataFile = L"";
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path)))
	{
		if (type == MOUSE_OPERATE)
		{
			fullPathDataFile = std::wstring(path) + L"\\MouseOperate.data";
		}
		else if (type == KEYBOARD_OPERATE)
		{
			fullPathDataFile = std::wstring(path) + L"\\KeyboardOperate.data";
		}
	}

	return fullPathDataFile;
}

bool WritingData::WriteData(const OperateType type, const std::chrono::system_clock::time_point &current)
{
	std::wstringstream stream;
	struct tm localTime;
	auto in_time_t = std::chrono::system_clock::to_time_t(current);
	wchar_t szBuffer[32];
	int error = localtime_s(&localTime, &in_time_t);
	size_t size = wcsftime(szBuffer, 32, L"%Y-%m-%d %H:%M:%S", &localTime);
	stream << szBuffer;
	
	if (type == KEYBOARD_OPERATE)
	{
		m_pKeyboarOperateFileIO->Write(stream.str());
	}
	else
	{
		m_pMouseOperateFileIO->Write(stream.str());
	}
	return true;
}

WritingData::WritingData()
{
	pathMouseOperate = GetFullPathDataFile(MOUSE_OPERATE);
	pathKeyboardOperate = GetFullPathDataFile(KEYBOARD_OPERATE);

	m_pMouseOperateFileIO = std::make_unique<PlainTextFileIO>(pathMouseOperate.c_str());
	m_pKeyboarOperateFileIO = std::make_unique<PlainTextFileIO>(pathKeyboardOperate.c_str());
}

bool WritingData::ExistFile(const OperateType type)
{
	if (type == MOUSE_OPERATE) return m_pMouseOperateFileIO->Exists();
	else return m_pKeyboarOperateFileIO->Exists();
}
