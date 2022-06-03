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
	const TCHAR* m_FileName;

	BaseFileIO(const TCHAR* fileName);
	void Open(const std::ios_base::openmode inputOutputMode) noexcept(false) override;

public:
	static bool Exists(const TCHAR* filePath);
	void SeekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) override;
	void Open(const TCHAR* fileName) override;
	bool EndOfFile() const override;
	const TCHAR* FileName() const noexcept override;
	void Close() override;
	~BaseFileIO();

};
