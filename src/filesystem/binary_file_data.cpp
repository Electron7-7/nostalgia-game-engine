#include "binary_file_data.hpp"

BinaryFileData::BinaryFileData() = default;

BinaryFileData::BinaryFileData(const std::vector<unsigned char>& data)
{ m_Data = data; }

BinaryFileData::BinaryFileData(unsigned char* data, unsigned int size)
: m_Data(data, data + size)
{}

const std::vector<unsigned char>& BinaryFileData::GetData() const
{ return m_Data; }
