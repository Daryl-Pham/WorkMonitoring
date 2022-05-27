#pragma once

#include "BaseFileIO.h"

class PlainTextFileIO : public BaseFileIO
{
public:
	PlainTextFileIO(const std::string& fileName) : BaseFileIO(fileName) {}

	template <typename Type>
	void Write(const Type& contentToWrite, bool appendContent = true);

	// Reads text content word by word
	std::string ReadWithOffset(std::size_t offset = 0);

	// Reads text content word by word
	std::string Read();

	std::string GetLine();

	std::string SafeRead();

	std::string SafeGetLine();

	std::string ToString();

	static std::string ToString(const std::string& fileName);

	static void SaveTextTo(const std::string& fileName, const std::string& text);
};
