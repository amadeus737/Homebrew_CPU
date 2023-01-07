#pragma once

#include "Util.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

template <typename T>
class Rom
{
public:
	Rom()
		:
		_size(0)
	{
		_data.clear();
	}

	void SetSize(const int s) { _size = s; _data.resize(s); }
	void AddData(const int& address, T d) { if (address < _data.size()) _data[address] = d; };
	std::optional<T> GetData(int i) { return _data[i]; }
	int GetSize() const { return _size; }

	void Initialize(T val)
	{
		//memset(&_data[0], 0, _size);
		for (int i = 0; i < _size; i++)
			_data[i] = val;
	}

	void WriteData_old(std::string filename, size_t num_bytes, size_t num_elems)
	{
		// Unfortunately, fwrite only writes in LITTLE ENDIAN format...why?!?
		FILE* file = fopen(filename.c_str(), "wb");
		fwrite(&_data[0], num_bytes, num_elems, file);
		fclose(file);
	}

	void WriteData(std::string filename, int num_bytes, int start_address, int end_address, bool echo, int blank_val = -1)
	{
		std::ofstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(filename);

		if (echo)
		{
			if (num_bytes == 1)
			{
				printf("\n=====================================================================\n");
				std::cout << std::setw(24 + filename.size() / 2) << std::setfill(' ') << "File :  " << filename << std::setw(24 - filename.size() / 2) << std::setfill(' ') << "\n";
				printf("=====================================================================\n");
				printf("ADDR:  00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\n");
				printf("---------------------------------------------------------------------");
			}

			if (num_bytes == 2)
			{
				printf("\n=====================================================================================================\n");
				std::cout << std::setw(56 - filename.size() / 2) << std::setfill(' ') << "File :  " << filename << std::setw(56 - filename.size() / 2) << std::setfill(' ') << "\n";
				printf("=====================================================================================================\n");
				printf("ADDR:  0000  0001  0002  0003  0004  0005  0006  0007  0010  0012  0014  0016  0018  001A  001C  001E\n");
				printf("-----------------------------------------------------------------------------------------------------");
			}
		}

		int byteCount = 0;
		int usedBytes = 0;
		for (auto b : _data)
		{
			if (num_bytes == 2)
				file << (uint8_t)((b.value_or(0) & 0xff00) >> 8);

			file << (uint8_t)(b.value_or(0) & 0x00ff);

			if (b.has_value())
			{
				usedBytes++;
			}

			if (++byteCount >= 8)
			{
				byteCount = 0;
			}
		}

		if (echo)
		{
			int printedBytes = 0;
			int columnCount = 0;
			int lastVal = -1;
			int currRow = 0;
			int lastRow = -1;
			int ellipsisPrinted = false;
			int column_width = 16;
			std::string header_format = "\n%0" + std::to_string(num_bytes * 4) + "x:   ";
			std::string number_format = "%0" + std::to_string(num_bytes * 2) + "x  ";

			while (start_address % column_width != 0)
			{
				start_address--;
			}

			while (end_address % (column_width - 1) != 0)
			{
				end_address++;
			}

			std::vector<int> row;
			row.reserve(9);
			int row_total = 0;
			int print_bytes_row_total = 0;
			int last_row_printed = -1;
			bool last_row_was_ellipsis = false;
			for (int i = start_address; i <= end_address; i++)
			{
				int addressCalc = start_address + i * 8;

				if (columnCount == 0 || columnCount == 16)
				{
					//printf(header_format.c_str(), i);
					row.push_back(currRow * column_width);

					columnCount = 0;
					currRow++;
				}

				int v = -1;
				if (i < _size && i < _data.size())
				{
					v = _data[i].value();
					//printf(number_format.c_str(), v);

					if (num_bytes == 2)
					{
						uint16_t v16 = ((v & 0xff00) >> 8) | ((v & 0x00ff) << 8);

						row.push_back(v16);
						row_total += v16;
						lastVal = v16;
					}
					else
					{
						row.push_back(v);
						row_total += v;
						lastVal = v;
					}

					print_bytes_row_total++;


					//printedBytes++;
				}
				else
				{
					row.push_back(0);

					lastVal = 0;
				}

				lastRow = currRow;
				columnCount++;

				if (columnCount == 0 || columnCount == 16)
				{
					if (row_total != 0)
					{
						std::cout << "\n" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << row[0] << "   ";
						for (int j = 1; j < row.size(); j++)
						{
							if (row[j] != blank_val)
							{
								std::cout << std::hex << std::uppercase << std::setw(2 * num_bytes) << std::setfill('0') << row[j] << "  ";
							}
							else
							{
								if (num_bytes == 1)
									std::cout << std::hex << std::uppercase << std::setw(2 * num_bytes) << std::setfill('0') << "..  ";
								else
									std::cout << std::hex << std::uppercase << std::setw(2 * num_bytes) << std::setfill('0') << "....  ";
							}
						}

						printedBytes += print_bytes_row_total;
						last_row_printed = currRow;
						last_row_was_ellipsis = false;
					}
					else
					{
						if (currRow - last_row_printed > 1 && !last_row_was_ellipsis)
						{
							if (num_bytes == 1)
							{
								std::cout << "\n....   ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..";
								//	std::cout << "\n....   ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..  ..";
							}

							if (num_bytes == 2)
							{
								std::cout << "\n....   ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....";
								//	std::cout << "\n....   ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....  ....";
							}

							last_row_was_ellipsis = true;
						}
					}

					row_total = 0;
					print_bytes_row_total = 0;
					row.clear();
				}

			}

			if (num_bytes == 1)
				printf("\n=====================================================================\n");

			if (num_bytes == 2)
				printf("\n=====================================================================================================\n");

			std::cout << "\n";

			std::cout << "Binary image successfully generated." << std::endl;
			std::cout << "Bytes used: [ " << usedBytes << " ]  Bytes free: [ "
				<< byteCount - usedBytes << " ]" << "\n\n";
		}
	}

	std::vector<std::optional<T>>& GetData() { return _data; }
	std::optional<T> GetDataAt(int a) { return _data[a]; }
	const void* GetDataPtr() { return &_data[0]; }

private:
	int _size;
	std::vector<std::optional<T>> _data;
};