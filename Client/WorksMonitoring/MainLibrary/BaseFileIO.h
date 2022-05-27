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
	std::fstream fileStream;
	std::ios_base::openmode inputOutputMode;
	std::string fileName_;

	BaseFileIO(const std::string& fileName);
	void open(const std::ios_base::openmode inputOutputMode) noexcept(false) override;

public:
	static bool exists(const std::string& filePath);
	void seekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) override;
	void open(const std::string& fileName) override;
	bool endOfFile() const override;
	std::string fileName() const noexcept override;
	void close() override;
	~BaseFileIO();

};
