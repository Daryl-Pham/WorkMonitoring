#include "stdafx.h"
#include "PlainTextFileIO.h"

template <typename Type>
void PlainTextFileIO::write(const Type& contentToWrite, bool appendContent)
{

	if (appendContent) { open(std::ios::out | std::ios::in | std::ios::app); }
	else { open(std::ios::out | std::ios::in | std::ios::trunc); }

	fileStream << contentToWrite;
}

/// Reads text content word by word
std::string PlainTextFileIO::readWithOffset(std::size_t offset)
{

	std::string readContent;
	open(std::ios::in);
	if (offset > 0) {
		seekInputPosition(offset);
	}
	fileStream >> readContent;

	return readContent;
}

/// Reads text content word by word
std::string PlainTextFileIO::read()
{
	return readWithOffset(0);
}

std::string PlainTextFileIO::getline()
{

	std::string s;

	open(std::ios::in);
	std::getline(fileStream, s);

	return s;
}


std::string PlainTextFileIO::safeRead()
{

	std::string readContent;
	open(std::ios::in);
	fileStream >> readContent;

	if ((fileStream.eof() && readContent.empty()) || fileStream.fail()) {
		return nullptr;
	}

	return readContent;
}

std::string PlainTextFileIO::safeGetline()
{

	std::string s;

	open(std::ios::in);
	std::getline(fileStream, s);

	if ((fileStream.eof() && s.empty()) || fileStream.fail()) {
		return nullptr;
	}

	return s;
}

std::string PlainTextFileIO::toString()
{

	this->open(std::ios::in);

	std::string line;
	std::string outputContent;

	while (std::getline(fileStream, line)) {
		outputContent += line + '\n';
	}

	return outputContent;
}

std::string PlainTextFileIO::toString(const std::string& fileName)
{
	return PlainTextFileIO(fileName).toString();
}

void PlainTextFileIO::saveTextTo(const std::string& fileName, const std::string& text)
{
	PlainTextFileIO fileToWrite(fileName);
	fileToWrite.open(std::ios::out);
	fileToWrite.write(text);
}