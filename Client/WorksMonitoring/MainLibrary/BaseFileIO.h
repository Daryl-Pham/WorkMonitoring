#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <memory>
#include "FileIOInterface.h"

class BaseFileIO : public FileIOInterface
{
	struct OpenCloseException : public std::runtime_error
	{
		OpenCloseException() :runtime_error("error") {}
		OpenCloseException(const std::string& message) :runtime_error(message.c_str()) {}
	};

protected:
	std::fstream m_FileStream;
	std::ios_base::openmode m_InputOutputMode;
	std::string m_FileName;

	BaseFileIO(const std::string& fileName);
	void Open(const std::ios_base::openmode inputOutputMode) noexcept(false) override;

public:
	static bool Exists(const std::string& filePath);
	void SeekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) override;
	void Open(const std::string& fileName) override;
	bool EndOfFile() const override;
	std::string FileName() const noexcept override;
	void Close() override;
	~BaseFileIO();

};
