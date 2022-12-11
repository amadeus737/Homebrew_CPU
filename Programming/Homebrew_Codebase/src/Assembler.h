#pragma once

#include "Command.h"
#include "Directive.h"
#include "Instruction.h"
#include "Symbol.h"
#include "Opcode.h"
#include "ROM.h"

#include <iostream>
#include <fstream>
#include <map>
#include <variant>
#include <optional>
#include <exception>
#include <locale>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <assert.h>
#include <stack>
#include <climits>

#define hex8 std::setfill('0') << std::setw(8) << std::hex << std::uppercase
#define hex4 std::setfill('0') << std::setw(4) << std::hex << std::uppercase
#define dec std::dec

class Assembler
{
public:
	// Constructors
	Assembler(std::string filename);
	Assembler(std::istream& file);

	// Used for linking include files
	void PushFileToSourceStack(std::string filename, int line);
	void PopFileFromSourceStack();

	void Assemble(std::string name);

	// Used to link tokens with specific functions defined in:
	//  - DirectiveDefine.h
	//  - CommandDefine.h
	//  - InstructionDefine.h
	void RegisterOperations();

	// Used to separate architecture definition parsing and assembly parsing
	void AcknowledgeArchEndTag();

	int NumOpcodeCycles();
	int LastOpcodeIndex();
	void SetPCincControlValue(int v);
	Opcode& GetOpcode(int v);

	void SetInstructionWidth(int w);
	void SetAddressWidth(int w);
	void SetProgramROMsize(int s);

	int GetInstructionWidth();
	int GetAddressWidth();

	void AddByteToProgramROM(uint8_t byte);

	void SetAddress(int a);
	int GetAddress() const;

	int GetSymbolAddress(const std::string& n) const;
	SymbolType GetSymbolType(const std::string& n);

	void AddLabel(const std::string& n, int a, int l);
	void AddVariable(const std::string& n, int a, int l);
	void AddFlag(const std::string& n, int a, int l);
	void AddRegister(const std::string& n, int a, int l);
	void AddControlLine(const std::string& n, int a, int l);
	void AddLabelReference(const std::string& n, int a, int l);
	void AddVariableReference(const std::string& n, int a, int l);
	void AddRegisterReference(const std::string& n, int a, int l);
	void AddControlLineReference(const std::string& n, int a, int l);
	void AddOpcode(int v, const Opcode& oc);
	void AddOpcodeAlias(int v, const Opcode& oca);
	void AddControlPatternToCurrentOpcode(int cp);

	bool IsAMnemonic(const std::string& s) { return std::find(_mnemonics.begin(), _mnemonics.end(), s) != _mnemonics.end(); }
	int GetValueByUniqueOpcodeString(const std::string& s);
	int GetValueByUniqueOpcodeAliasString(const std::string& s);

	void WriteDecoderROMs(const std::string& path, const std::string& name, int inputs, int outputs);

private:
	void ProcessFile(std::istream& file, int startLine);
	void ProcessLine(std::string& line, int linenum, bool preprocessing = false);
	void ValidateAddress() const;
	void ResolveDependencies();

	template <class D>
	void RegisterDirective(std::string name)
	{
		assert(_directives.count(name) == 0);
		_directives.emplace(name, std::make_unique<D>());
	}

	template <class C>
	void RegisterCommand(std::string name)
	{
		assert(_commands.count(name) == 0);
		_commands.emplace(name, std::make_unique<C>());
	}

	template <class I>
	void RegisterInstruction(std::string n)
	{
		assert(_instructions.count(n) == 0);
		_instructions.emplace(n, std::make_unique<I>());
	}

	void RegisterDirectiveAlias(std::string directive, std::string alias)
	{
		assert(_directives.count(alias) == 0);
		_directives.emplace(alias, std::make_unique<DirectiveAlias>(_directives.find(directive)->second.get()));
	}

	template <typename T>
	void WriteDecoderROMinternal(const std::string& path, const std::string& name, int in_bits_needed, int n_roms, int rom_size, int seq_bits, int oc_bits, int outputs)
	{
		for (int i = 0; i < n_roms; i++)
		{
			std::cout << "\nWriting to rom " << i << "...\n\n";

			ROM<T> rom;

			rom.SetSize(rom_size);
			rom.Initialize(0);

			for (int f = 0; f < _nFlags; f++)
			{
				for (int oc = 0; oc <= _maxOpcodeValue; oc++)
				{
					Opcode curr_opcode = _opcodes[oc];

					for (int s = 0; s < curr_opcode.numCycles(); s++)
					{
						int address = (f << (seq_bits + oc_bits)) | (oc << (seq_bits)) | s;

						int controlPattern = 0;
						controlPattern = curr_opcode.controlPattern(s);
						T cp = (controlPattern >> (i * 16)) & 0xffff;

						rom.AddData(address, cp);

						std::cout << "rom[" << dec << std::setw(4) << std::setfill('0') << address << "] = $" << hex4 << cp << "\n";
					}
				}
			}

			//rom.WriteData((path + name + "_" + std::to_string(i) + ".bin").c_str(), outputs / 8, rom_size * 8 / outputs);
			rom.WriteData((path + name + "_" + std::to_string(i) + ".bin").c_str());
		}
	}

private:
	std::ifstream _file;
	std::ostream& _mout = std::cout;
	std::stack<std::pair<std::string, int>> _sourceStack;
	std::map<std::string, Symbol> _symbols;
	std::map<int, Opcode> _opcodes;
	std::map<int, Opcode> _opcode_aliases;
	std::vector<std::string> _mnemonics;
	std::multimap<std::string, Symbol> _symbolReferences;
	std::vector<std::pair<std::string, int>> _sourceLines;
	std::map<std::string, std::unique_ptr<Command>> _commands;
	std::map<std::string, std::unique_ptr<Instruction>> _instructions;
	std::map<std::string, std::unique_ptr<Directive>> _directives;
	int _lastOpcodeIndex = -1;
	int _address = 0;
	int _maxControlLineValue = -1;
	int _maxOpcodeValue = -1;
	int _maxNumCycles = -1;
	int _nFlags = 0;
	bool _arch_defined = false;
	int _pc_inc_val = -1;
	int _instruction_width = -1;
	int _address_width = -1;
	ROM<unsigned char> _program_rom;
}; 