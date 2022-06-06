#pragma once

#include "BaseFileIO.h"
#include <mutex>

class PlainTextFileIO : public BaseFileIO
{
private:
	std::mutex m_MutexFileProcessingFile;
public:
	PlainTextFileIO(const TCHAR* fileName) : BaseFileIO(fileName) {}

	template <typename Type>
	void Write(const Type& contentToWrite, bool appendContent = true);

	// Reads text content word by word
	const TCHAR* ReadWithOffset(std::size_t offset = 0);

	// Reads text content word by word
	const TCHAR* Read();

	std::wstring GetLine();

	std::wstring SafeRead();

	std::wstring SafeGetLine();

	std::wstring ToString();

	static std::wstring ToString(std::wstring& fileName);

	static void SaveTextTo(std::wstring& fileName, std::wstring& text);
};
