#include "stdafx.h"
#include "PlainTextFileIO.h"
#include <codecvt>
#include <string>

template <typename Type>
void PlainTextFileIO::Write(const Type& contentToWrite, bool appendContent)
{
	std::lock_guard<std::mutex> lock(m_MutexFileProcessingFile);
	if (appendContent)
	{
		Open(std::ios::out | std::ios::in | std::ios::app);
	}
	else
	{
		Open(std::ios::out | std::ios::in | std::ios::trunc);
	}

	m_FileStream << contentToWrite;
}

/// Reads text content word by word
const TCHAR* PlainTextFileIO::ReadWithOffset(std::size_t offset)
{

	std::wstring readContent;
	Open(std::ios::in);
	if (offset > 0) {
		SeekInputPosition(offset);
	}
	m_FileStream >> readContent;

	return readContent.c_str();
}

/// Reads text content word by word
const TCHAR* PlainTextFileIO::Read()
{
	return ReadWithOffset(0);
}

std::wstring PlainTextFileIO::GetLine()
{

	std::wstring s;

	Open(std::ios::in);
	std::getline(m_FileStream, s);

	return s;
}


std::wstring PlainTextFileIO::SafeRead()
{

	std::wstring readContent;
	Open(std::ios::in);
	m_FileStream >> readContent;

	if ((m_FileStream.eof() && readContent.empty()) || m_FileStream.fail()) {
		return nullptr;
	}

	return readContent;
}

std::wstring PlainTextFileIO::SafeGetLine()
{

	std::wstring s;

	Open(std::ios::in);
	std::getline(m_FileStream, s);

	if ((m_FileStream.eof() && s.empty()) || m_FileStream.fail()) {
		return nullptr;
	}

	return s;
}

std::wstring PlainTextFileIO::ToString()
{

	this->Open(std::ios::in);

	std::wstring line;
	std::wstring outputContent;

	while (std::getline(m_FileStream, line)) {
		outputContent += line + L"\n";
	}

	return outputContent;
}

std::wstring PlainTextFileIO::ToString(std::wstring& fileName)
{
	return PlainTextFileIO(fileName.c_str()).ToString();
}

void PlainTextFileIO::SaveTextTo(std::wstring& fileName, std::wstring& text)
{
	PlainTextFileIO fileToWrite(fileName.c_str());
	fileToWrite.Open(std::ios::out);
	fileToWrite.Write(text);
}