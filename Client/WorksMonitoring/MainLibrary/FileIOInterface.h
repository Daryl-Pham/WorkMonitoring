#pragma once

class FileIOInterface
{
protected:
	virtual void open(const std::ios_base::openmode inputOutputMode) noexcept(false) = 0;


public:
	virtual void seekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) = 0;
	virtual void open(const std::string& fileName) = 0;
	virtual bool endOfFile() const = 0;
	virtual std::string fileName() const noexcept = 0;
	virtual void close() = 0;
	virtual ~FileIOInterface() = default;
};

