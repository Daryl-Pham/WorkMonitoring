#include "stdafx.h"
#include "PlainTextFileIO.h"

template <typename Type>
void PlainTextFileIO::Write(const Type& contentToWrite, bool appendContent)
{

	if (appendContent) { Open(std::ios::out | std::ios::in | std::ios::app); }
	else { Open(std::ios::out | std::ios::in | std::ios::trunc); }

	m_FileStream << contentToWrite;
}

/// Reads text content word by word
std::string PlainTextFileIO::ReadWithOffset(std::size_t offset)
{

	std::string readContent;
	Open(std::ios::in);
	if (offset > 0) {
		SeekInputPosition(offset);
	}
	m_FileStream >> readContent;

	return readContent;
}

/// Reads text content word by word
std::string PlainTextFileIO::Read()
{
	return ReadWithOffset(0);
}

std::string PlainTextFileIO::GetLine()
{

	std::string s;

	Open(std::ios::in);
	std::getline(m_FileStream, s);

	return s;
}


std::string PlainTextFileIO::SafeRead()
{

	std::string readContent;
	Open(std::ios::in);
	m_FileStream >> readContent;

	if ((m_FileStream.eof() && readContent.empty()) || m_FileStream.fail()) {
		return nullptr;
	}

	return readContent;
}

std::string PlainTextFileIO::SafeGetLine()
{

	std::string s;

	Open(std::ios::in);
	std::getline(m_FileStream, s);

	if ((m_FileStream.eof() && s.empty()) || m_FileStream.fail()) {
		return nullptr;
	}

	return s;
}

std::string PlainTextFileIO::ToString()
{

	this->Open(std::ios::in);

	std::string line;
	std::string outputContent;

	while (std::getline(m_FileStream, line)) {
		outputContent += line + '\n';
	}

	return outputContent;
}

std::string PlainTextFileIO::ToString(const std::string& fileName)
{
	return PlainTextFileIO(fileName).ToString();
}

void PlainTextFileIO::SaveTextTo(const std::string& fileName, const std::string& text)
{
	PlainTextFileIO fileToWrite(fileName);
	fileToWrite.Open(std::ios::out);
	fileToWrite.Write(text);
}