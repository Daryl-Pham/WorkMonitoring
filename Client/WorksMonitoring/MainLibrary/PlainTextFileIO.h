#pragma once

#include "BaseFileIO.h"

class PlainTextFileIO : public BaseFileIO
{
public:
	PlainTextFileIO(const TCHAR* fileName) : BaseFileIO(fileName) {}

	template <typename Type>
	void Write(const Type& contentToWrite, bool appendContent = true);

	// Reads text content word by word
	TCHAR* ReadWithOffset(std::size_t offset = 0);

	// Reads text content word by word
	TCHAR* Read();

	TCHAR* GetLine();

	TCHAR* SafeRead();

	TCHAR* SafeGetLine();

	TCHAR* ToString();

	static TCHAR* ToString(const TCHAR* fileName);

	static void SaveTextTo(const TCHAR* fileName, const TCHAR* text);

private:
	void StringToTCHAR(std::string str);
};
