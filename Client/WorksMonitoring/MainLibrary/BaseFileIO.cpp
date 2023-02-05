#include "stdafx.h"
#include "BaseFileIO.h"

void BaseFileIO::Open(const std::ios_base::openmode inputOutputMode) noexcept(false)
{
	if (this->m_InputOutputMode == inputOutputMode)
	{
		return;
	}

	Close();
	this->m_InputOutputMode = inputOutputMode;
	m_FileStream.open(m_FileName, inputOutputMode);

	if (m_FileStream.fail())
	{
		throw OpenCloseException("File couldn't be open");
	}
}

BaseFileIO::BaseFileIO(const TCHAR* fileName)
{
	this->m_FileName = fileName;
}

bool BaseFileIO::Exists()
{
	std::ifstream file(m_FileName);
	return !file.fail();
}

bool BaseFileIO::Exists(const TCHAR* filePath)
{
	std::ifstream file(filePath);
	return !file.fail();
}

void BaseFileIO::SeekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position)
{
	m_FileStream.seekg(offsetPosition, position);
}

void BaseFileIO::Open(TCHAR* fileName)
{
	this->m_FileName = fileName;
	this->Close();
}

bool BaseFileIO::EndOfFile() const
{
	return m_FileStream.eof();
}

const TCHAR* BaseFileIO::FileName() const noexcept
{
	return m_FileName;
}

/// Optional to use, the class automatically closes the file
void BaseFileIO::Close()
{
	if (m_FileStream.is_open())
	{
		m_FileStream.close();
	}
}

BaseFileIO::~BaseFileIO()
{
	Close();
}
