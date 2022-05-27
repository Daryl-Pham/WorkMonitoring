#pragma once

class FileIOInterface
{
protected:
	virtual void Open(const std::ios_base::openmode inputOutputMode) noexcept(false) = 0;


public:
	virtual void SeekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) = 0;
	virtual void Open(const std::string& fileName) = 0;
	virtual bool EndOfFile() const = 0;
	virtual std::string FileName() const noexcept = 0;
	virtual void Close() = 0;
	virtual ~FileIOInterface() = default;
};

