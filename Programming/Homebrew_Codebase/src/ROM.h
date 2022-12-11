#pragma once

#include "Util.h"

#include <string>
#include <vector>
#include <fstream>

template <typename T>
class ROM
{
public:
	ROM()
		:
		_size(0)
	{
		_data.clear();
	}

	void SetSize(const int s) { _size = s; _data.resize(s); }
	void AddData(const int& address, T d) { if (address < _data.size()) _data[address] = d; };
	int GetSize() const { return _size; }

	void Initialize(T val)
	{
		memset(&_data[0], 0, _size);
	}

	void WriteData_old(std::string filename, size_t num_bytes, size_t num_elems)
	{
		// Unfortunately, fwrite only writes in LITTLE ENDIAN format...why?!?
		FILE* file = fopen(filename.c_str(), "wb");
		fwrite(&_data[0], num_bytes, num_elems, file);
		fclose(file);
	}

	void WriteData(std::string filename)
	{
		std::ofstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(filename);

		int byteCount = 0;
		int usedBytes = 0;
		for (auto b : _data)
		{
			file << (unsigned char)(b.value_or(0));

			if (b.has_value())
			{
				usedBytes++;
			}

			if (++byteCount >= 8)
			{
				byteCount = 0;
			}
		}

		std::cout << "Binary image successfully generated." << std::endl;
		std::cout << "Bytes used: [ " << usedBytes << " ]  Bytes free: [ "
			<< 256 - usedBytes << " ]" << std::endl;
	}

	std::vector<std::optional<T>>& GetData() { return _data; }
	const void* GetDataPtr() { return &_data[0]; }

private:
	int _size;
	std::vector<std::optional<T>> _data;
};