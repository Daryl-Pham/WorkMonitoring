#pragma once

#ifdef WRITINGDATA_EXPORTS
#define WRITINGDATA_API __declspec(dllexport)
#else
#define WRITINGDATA_API __declspec(dllimport)
#endif

#include <string>

class PlainTextFileIO;

enum  OperateType
{
	MOUSE_OPERATE = 0,
	KEYBOARD_OPERATE = 1
};

class WritingData
{
private:
	PlainTextFileIO* m_MouseOperateFileIO;
	PlainTextFileIO* m_KeyboarOperateFileIO;

	std::wstring GetFullPathDataFile(const OperateType type);
	bool WriteData(const OperateType type);
public:
	WritingData();
};
