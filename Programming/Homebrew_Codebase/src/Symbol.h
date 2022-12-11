#pragma once

#include <string>

enum class SymbolType { None, Label, Variable, Register, Flag, ControlLine };

// In my homebrew assembler, a symbol is any non-whitespace token that is not an instruction.
// This class essentially links the symbol string tokens with other useful information, like:
//  - the symbol type
//  - an integer value
//  - the line the symbol was found on
class Symbol
{
public:
	Symbol(Symbol&& donor) noexcept
		:
		_name(std::move(donor._name)),
		_type(donor._type),
		_address(donor._address),
		_line(donor._line)
	{}

	// These just access and return the private members of the class
	const std::string& GetName() const { return _name; }
	int GetAddress() const { return _address; }
	SymbolType GetType() const { return _type; }
	int GetLine() const { return _line; }

	static Symbol MakeVariable(const std::string& n, int a, int l)
	{
		return Symbol(n, SymbolType::Variable, a, l);
	}

	static Symbol MakeLabel(const std::string& n, int a, int l)
	{
		return Symbol(n, SymbolType::Label, a, l);
	}

	static Symbol MakeRegister(const std::string& n, int a, int l)
	{
		return Symbol(n, SymbolType::Register, a, l);
	}

	static Symbol MakeFlag(const std::string& n, int a, int l)
	{
		return Symbol(n, SymbolType::Flag, a, l);
	}

	static Symbol MakeControlLine(const std::string& n, int a, int l)
	{
		return Symbol(n, SymbolType::ControlLine, a, l);
	}

	bool operator<(const Symbol& other) const { return _name < other._name; }
	bool operator==(const Symbol& other) const { return _type == other._type && _name == other._name; }

private:
	Symbol(const std::string& n, SymbolType t, int a, int l)
		:
		_name(n),
		_type(t),
		_address(a),
		_line(l)
	{}

private:
	std::string _name;
	SymbolType _type;
	int _address;
	int _line;
};