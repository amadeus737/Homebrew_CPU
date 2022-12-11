#pragma once

#include <iomanip>
#include <bitset>
#include <climits>

#define hex8 std::setfill('0') << std::setw(8) << std::hex << std::uppercase
#define hex4 std::setfill('0') << std::setw(4) << std::hex << std::uppercase
#define hex2 std::setfill('0') << std::setw(2) << std::hex << std::uppercase
#define dec std::dec

/*
std::string hex_to_bin(const std::string& s)
{
	int length = s.length();
	std::string output = "";
	for (int i = 0; i < length; i ++)
	{
		if (s[i] == '0') output += "0000";
		if (s[i] == '1') output += "0001";
		if (s[i] == '2') output += "0010";
		if (s[i] == '3') output += "0011";
		if (s[i] == '4') output += "0100";
		if (s[i] == '5') output += "0101";
		if (s[i] == '6') output += "0110";
		if (s[i] == '7') output += "0111";
		if (s[i] == '8') output += "1000";
		if (s[i] == '9') output += "1001";
		if (s[i] == 'a' || s[i] == 'A') output += "1010";
		if (s[i] == 'b' || s[i] == 'B') output += "1011";
		if (s[i] == 'c' || s[i] == 'C') output += "1100";
		if (s[i] == 'd' || s[i] == 'D') output += "1101";
		if (s[i] == 'e' || s[i] == 'E') output += "1110";
		if (s[i] == 'f' || s[i] == 'F') output += "1111";

		std::cout << "     HEX[" << i << "] = " << s[i] << "  BIN = " << output << "\n";
	}

	return output;
}
*/

template<typename T>
void printBin(const T& t) 
{
	size_t nBytes = sizeof(T);
	char* rawPtr((char*)(&t));
	for (size_t byte = 0; byte < nBytes; byte++) 
	{
		for (size_t bit = 0; bit < CHAR_BIT; bit++) 
		{
			std::cout << (((rawPtr[byte]) >> bit) & 1);
		}
	}
	std::cout << std::endl;
};