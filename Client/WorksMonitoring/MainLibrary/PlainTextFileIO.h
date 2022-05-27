#pragma once

#include "BaseFileIO.h"

class PlainTextFileIO : public BaseFileIO
{
public:
	PlainTextFileIO(const std::string& fileName) : BaseFileIO(fileName) {}

	template <typename Type>
	void write(const Type& contentToWrite, bool appendContent = true);

	// Reads text content word by word
	std::string readWithOffset(std::size_t offset = 0);

	// Reads text content word by word
	std::string read();

	std::string getline();

	std::string safeRead();

	std::string safeGetline();

	std::string toString();

	static std::string toString(const std::string& fileName);

	static void saveTextTo(const std::string& fileName, const std::string& text);
};
