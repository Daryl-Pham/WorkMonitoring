#pragma once

#ifdef WRITINGDATA_EXPORTS
#define WRITINGDATA_API __declspec(dllexport)
#else
#define WRITINGDATA_API __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include <chrono>
#include "PlainTextFileIO.h"

class PlainTextFileIO;

enum  OperateType
{
	MOUSE_OPERATE = 0,
	KEYBOARD_OPERATE = 1
};

class WritingData
{
private:
	std::unique_ptr<PlainTextFileIO> m_pMouseOperateFileIO;
	std::unique_ptr<PlainTextFileIO> m_pKeyboarOperateFileIO;
public:
	WritingData();
	std::wstring GetFullPathDataFile(const OperateType type);
	bool ExistFile(const OperateType type = MOUSE_OPERATE);
	bool WriteData(const OperateType type, const std::chrono::system_clock::time_point &current);
};
