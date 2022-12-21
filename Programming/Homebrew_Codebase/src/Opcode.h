#pragma once

#include <iostream>
#include <vector>
#include <string>

enum class ArgType { None, Register, Numeral, Ascii, DerefReg, DerefNum, DerefAscii };

class Opcode
{
public:
	// Used to specify opcode arguments
	class Arg
	{
	public:
		ArgType _type;
		std::string _string;
	};

	Opcode()
	{
		_mnemonic = "";
		_value = -1;
		_controlPattern.clear();
		_arguments.clear();
	}

	void SetMnemonic(const std::string& s) { _mnemonic = s; }
	void SetValue(const int& v) { _value = v; }
	void AddControlPattern(const int& p) { _controlPattern.push_back(p); }
	void AddArgument(Arg a) { _arguments.push_back(a); }

	std::string mnemonic() { return _mnemonic; }
	int value() { return _value; }
	int numArgs() { return _arguments.size(); }
	Arg arg(int i) { return _arguments[i]; }
	int numCycles() { return _controlPattern.size(); }
	int& controlPattern(int i) { return _controlPattern[i]; }

	std::string GetUniqueString()
	{
		std::string unique_str = _mnemonic + "_";
		
		for (int i = 0; i < _arguments.size(); i++)
		{
			switch (_arguments[i]._type)
			{
				case ArgType::Register:
				case ArgType::DerefReg:
					unique_str += _arguments[i]._string + "_";
					break;

				case ArgType::Numeral:
					unique_str += "#_";
					break;

				case ArgType::DerefNum:
					unique_str += "[#]_";
					break;

				case ArgType::Ascii:
					unique_str += "ASCII_";
					break;

				case ArgType::DerefAscii:
					unique_str += "[ASCII]_";
					break;
			}
		}

		// Erase the '_' character on the end
		unique_str = std::string(unique_str.begin(), unique_str.end() - 1);

		return unique_str;
	}

private:
	std::string _mnemonic;
	int _value;
	std::vector<int> _controlPattern;
	std::vector<Arg> _arguments;
};