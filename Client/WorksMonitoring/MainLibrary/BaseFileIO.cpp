#include "stdafx.h"
#include "BaseFileIO.h"

void BaseFileIO::open(const std::ios_base::openmode inputOutputMode) noexcept(false)
{
	if (this->inputOutputMode == inputOutputMode)
	{
		return;
	}

	close();
	this->inputOutputMode = inputOutputMode;
	fileStream.open(fileName_, inputOutputMode);

	if (fileStream.fail())
	{
		throw OpenCloseException("File couldn't be open");
	}
}

BaseFileIO::BaseFileIO(const std::string& fileName)
{
	this->fileName_ = fileName;
}

bool BaseFileIO::exists(const std::string& filePath)
{
	std::ifstream file(filePath);
	return !file.fail();
}

void BaseFileIO::seekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position)
{
	fileStream.seekg(offsetPosition, position);
}

void BaseFileIO::open(const std::string& fileName)
{
	this->fileName_ = fileName;
	this->close();
}

bool BaseFileIO::endOfFile() const
{
	return fileStream.eof();
}

std::string BaseFileIO::fileName() const noexcept
{
	return fileName_;
}

/// Optional to use, the class automatically closes the file
void BaseFileIO::close()
{
	if (fileStream.is_open())
	{
		fileStream.close();
	}
}

BaseFileIO::~BaseFileIO()
{
	close();
}
