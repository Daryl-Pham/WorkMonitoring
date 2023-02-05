#include "stdafx.h"
#include "BinaryFileIO.h"

template <typename Type>
void BinaryFileIO::Write(Type&& content, bool appendContent)
{

	if (appendContent) {
		Open(std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
	}
	else { Open(std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc); }

	if constexpr (!std::is_same<Type, std::string>()) {
		m_FileStream.write(reinterpret_cast<char*>(&content), sizeof(content));
	}
	else {
		m_FileStream.write(content.c_str(), content.length());
	}
}

template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::ReadWithOffset(std::size_t offset)
{

	Type readInput{};
	Open(std::ios::in | std::ios::binary);
	if (offset > 0) {
		SeekPosition(offset);
	}
	m_FileStream.read(reinterpret_cast<char*>(&readInput), sizeof(readInput));
	return readInput;
}

template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::Read()
{
	return ReadWithOffset<Type>(0);
}

template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::ReadWithOffset(std::size_t size, std::size_t offset)
{

	std::string readContent;
	Open(std::ios::in);
	if (offset > 0) {
		SeekInputPosition(offset);
	}
	//m_FileStream >> readContent;//khanhpqtest: ignore to fix for c++20

	return readContent;
}

template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
Type BinaryFileIO::Read(std::size_t size)
{
	return ReadWithOffset<Type>(size, 0);
}

void BinaryFileIO::SeekPosition(std::size_t offsetPosition, std::ios_base::seekdir position)
{
	m_FileStream.seekp(offsetPosition, position);
}