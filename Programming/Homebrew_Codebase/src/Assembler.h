#pragma once

#include "Command.h"
#include "Directive.h"
#include "Instruction.h"
#include "Symbol.h"
#include "Opcode.h"
#include "Rom.h"

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
	//Assembler(std::istream& file);

	// Used to link tokens with specific functions defined in DirectiveDefine.h, CommandDefine.h, and InstructionDefine.h
	void RegisterOperations();

	// Start the assembler
	void Assemble(std::string name);

	// Used for linking include files
	void PushFileToSourceStack(std::string filename, int line);
	void PopFileFromSourceStack();	

	// Used to separate architecture definition parsing and assembly parsing
	void AcknowledgeArchEndTag();

	// General stuff
	int GetInstructionWidth();
	int GetAddressWidth();
	void SetInstructionWidth(int w);
	void SetAddressWidth(int w);

	// Echo stuff
	void SetEchoLevel(unsigned char e);
	bool EchoMajorTasks();
	bool EchoMinorTasks();
	bool EchoWarnings();
	bool EchoSource();
	bool EchoParsedMajor();
	bool EchoParsedMinor();
	bool EchoRomData();

	// Symbol stuff
	SymbolType GetSymbolType(const std::string& n);

	// Opcode stuff
	bool IsAMnemonic(const std::string& s);
	int GetValueByUniqueOpcodeString(const std::string& s);
	int GetValueByUniqueOpcodeAliasString(const std::string& s);
	int NumOpcodeCycles();
	int LastOpcodeIndex();
	Opcode& GetOpcode(int v);

	// Addressing stuff
	void SetAddress(int a);
	int GetAddress() const;
	int GetSymbolAddress(const std::string& n) const;

	// Symbol and instruction registration stuff
	void AddLabel(const std::string& n, int a, int l);
	void AddVariable(const std::string& n, int a, int l);
	void AddFlag(const std::string& n, int a, int l);
	void AddRegister(const std::string& n, int a, int l);
	void AddControlLine(const std::string& n, int a, int l);
	void AddOpcode(int v, const Opcode& oc);
	void AddOpcodeAlias(int v, const Opcode& oca);
	void AddControlPatternToCurrentOpcode(int cp);

	// Symbol and instruction reference registration stuff
	void AddLabelReference(const std::string& n, int a, int l);
	void AddVariableReference(const std::string& n, int a, int l);
	void AddRegisterReference(const std::string& n, int a, int l);
	void AddControlLineReference(const std::string& n, int a, int l);
			
	// Decoder Rom stuff
	void AddDecoderRom(bool write, int inputs, int outputs);
	void WriteDecoderRom();
	
	// ProgramRom stuff
	void AddProgramRom(bool write, int inputs, int outputs);
	void AddByteToProgramRom(uint8_t byte);	
	void WriteProgramRom();	

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
	void WriteDecoderRom_Internal(std::string fullfilepath, int rom_number)
	{
		Rom<T> rom;

		rom.SetSize(_rom_size_decode);
		rom.Initialize(0);

		int last_address = 0;
		for (int f = 0; f < _nFlags; f++)
		{
			for (int oc = 0; oc <= _maxOpcodeValue; oc++)
			{
				Opcode curr_opcode = _opcodes[oc];

				for (int s = 0; s < curr_opcode.numCycles(); s++)
				{
					int address = (f << (_seq_bits_decode + _oc_bits_decode)) | (oc << (_seq_bits_decode)) | s;
					last_address = address;

					int controlPattern = 0;
					controlPattern = curr_opcode.controlPattern(s);
					T cp = (controlPattern >> (rom_number * 16)) & 0x0000ffff;

					//std::cout << "ROM[" << dec << address << "] = $" << std::hex << std::setw(4) << std::setfill('0') << cp << "\n";

					rom.AddData(address, cp);
				}
			}
		}
		
		rom.WriteData(fullfilepath.c_str(), _in_bits_decode / 8, 0, last_address, _echo_rom_data, 0);
	}

private:
	// File stuff
	std::ifstream _file;
	std::string _start_file;
	std::stack<std::pair<std::string, int>> _sourceStack;
	std::vector<std::pair<std::string, int>> _sourceLines;

	// General stuff
	bool _arch_defined = false;
	int _instruction_width = -1;
	int _address_width = -1;

	// Echo stuff
	bool _echo_major_tasks;
	bool _echo_minor_tasks;
	bool _echo_warnings;
	bool _echo_source;
	bool _echo_parsed_major;
	bool _echo_parsed_minor;
	bool _echo_rom_data;

	// Flag stuff
	int _nFlags = 0;

	// Symbol stuff
	std::map<std::string, Symbol> _symbols;
	std::multimap<std::string, Symbol> _symbolReferences;

	// Opcode stuff
	std::map<int, Opcode> _opcodes;
	std::map<int, Opcode> _opcode_aliases;
	std::vector<std::string> _mnemonics;
	int _lastOpcodeIndex = -1;
	
	// Token identifier stuff
	std::map<std::string, std::unique_ptr<Directive>>   _directives;
	std::map<std::string, std::unique_ptr<Command>>	    _commands;
	std::map<std::string, std::unique_ptr<Instruction>> _instructions;
	
	// Addressing stuff
	int _address = 0;
	int _max_address = 0;
	
	// Decode rom stuff
	bool _write_decode_rom = false;	
	int _maxControlLineValue = -1;
	int _maxOpcodeValue = -1;
	int _maxNumCycles = -1;
	int _rom_size_decode;
	int _in_bits_decode;
	int _out_bits_decode;	
	int _oc_bits_decode;
	int _seq_bits_decode;

	// Program rom stuff
	Rom<unsigned char> _program_rom;
	bool _write_program_rom = false;
	int _in_bits_program;
	int _out_bits_program;
};
