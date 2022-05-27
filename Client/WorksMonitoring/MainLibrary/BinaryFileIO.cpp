#include "stdafx.h"
#include "BinaryFileIO.h"

template <typename Type>
void BinaryFileIO::write(Type&& content, bool appendContent)
{

	if (appendContent) {
		open(std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
	}
	else { open(std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc); }

	if constexpr (!std::is_same<Type, std::string>()) {
		fileStream.write(reinterpret_cast<char*>(&content), sizeof(content));
	}
	else {
		fileStream.write(content.c_str(), content.length());
	}
}

template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::readWithOffset(std::size_t offset)
{

	Type readInput{};
	open(std::ios::in | std::ios::binary);
	if (offset > 0) {
		seekPosition(offset);
	}
	fileStream.read(reinterpret_cast<char*>(&readInput), sizeof(readInput));
	return readInput;
}

template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::read()
{
	return readWithOffset<Type>(0);
}

template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::readWithOffset(std::size_t size, std::size_t offset)
{

	std::string readContent;
	open(std::ios::in);
	if (offset > 0) {
		seekInputPosition(offset);
	}
	fileStream >> readContent;

	return readContent;
}

template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::read(std::size_t size)
{
	return readWithOffset<Type>(size, 0);
}

void BinaryFileIO::seekPosition(std::size_t offsetPosition, std::ios_base::seekdir position)
{
	fileStream.seekp(offsetPosition, position);
}