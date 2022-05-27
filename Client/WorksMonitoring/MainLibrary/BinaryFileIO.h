#pragma once

#include "BaseFileIO.h"

class BinaryFileIO : public BaseFileIO
{
public:
	BinaryFileIO(const std::string& fileName) : BaseFileIO(fileName) {}

	template <typename Type>
	void Write(Type&& content, bool appendContent = true);

	template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
	Type ReadWithOffset(std::size_t offset);

	template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
	Type Read();

	template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
	Type ReadWithOffset(std::size_t size, std::size_t offset);

	template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
	Type Read(std::size_t size);

	void SeekPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg);

};
