#include "Assembler.h"
#include "SymbolConfig.h"
#include "Parser.h"
#include "DirectiveDefine.h"
#include "CommandDefine.h"
#include "InstructionDefine.h"

#include <iostream>
#include <exception>
#include <sstream>
#include <iomanip>
#include <assert.h>

Assembler::Assembler(std::string filename)
{
	RegisterOperations();
	_start_file = filename;
}

/*
Assembler::Assembler(std::istream& file)
{
	RegisterOperations();
	ProcessFile(file, 0);
}
*/

void Assembler::RegisterOperations()
{
	RegisterDirective<IncludeDirective>(INCLUDE_STR);
	RegisterDirectiveAlias(INCLUDE_STR, ARCH_STR);
	RegisterDirectiveAlias(INCLUDE_STR, INSERT_STR);
	RegisterDirective<OriginDirective>(ORIGIN_STR);

	RegisterCommand<InstructionWidthCommand>(INSTRUCTION_WIDTH_STR);
	RegisterCommand<AddressWidthCommand>(ADDRESS_WIDTH_STR);
	RegisterCommand<RegisterAddCommand>(REGISTER_STR);
	RegisterCommand<FlagAddCommand>(FLAG_STR);
	RegisterCommand<DeviceAddCommand>(DEVICE_STR);
	RegisterCommand<ControlLineCommand>(CONTROL_STR);
	RegisterCommand<OpcodeCommand>(OPCODE_STR);
	RegisterCommand<OpcodeAliasCommand>(OPCODE_ALIAS_STR);
	RegisterCommand<OpcodeAddSequenceCommand>(OPCODE_SEQ_STR);
	RegisterCommand<DecoderRomCommand>(DECODER_ROM_STR);
	RegisterCommand<ProgramRomCommand>(PROGRAM_ROM_STR);
	RegisterCommand<EndArchCommand>(END_ARCH_STR);
}

void Assembler::Assemble(std::string name)
{
	_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	PushFileToSourceStack(_start_file, 0);

	ResolveDependencies();

	if (_echo_major_tasks)
		std::cout << "\nSymbol dependencies successfuly resolved." << std::endl;
	
	WriteProgramRom();
}

void Assembler::PushFileToSourceStack(std::string filename, int line)
{
	if (_file.is_open())
		_file.close();

	if (!_sourceStack.empty())
		_sourceStack.top().second = line;

	_file.open(filename);
	//_sourceStack.emplace(filename, 0);
	_sourceStack.push(std::pair<std::string, int>(filename, 0));

	if (_echo_major_tasks)
		std::cout << "\nProcessing file :" << filename << "\n\n";

	ProcessFile(_file, 0);
}

void Assembler::PopFileFromSourceStack()
{
	if (_file.is_open())
		_file.close();
	_sourceStack.pop();
	auto stackTop = _sourceStack.top();

	std::string filename = stackTop.first;
	int linenum = stackTop.second;

	if (_echo_major_tasks)
		std::cout << "\nProcessing file :" << filename << " \n\n";

	_file.open(filename);

	ProcessFile(_file, linenum);
}

void Assembler::AcknowledgeArchEndTag() 
{
	_arch_defined = true; 
}

int Assembler::GetInstructionWidth()
{
	return _instruction_width;
}

int Assembler::GetAddressWidth()
{
	return _address_width;
}

void Assembler::SetInstructionWidth(int w)
{
	_instruction_width = w;
}

void Assembler::SetAddressWidth(int w)
{
	_address_width = w;
}

void Assembler::SetEchoLevel(unsigned char e)
{// $1000 0000 currently unused
	_echo_major_tasks = (e & 0x40) == 0x40;  // $0100 0000
	_echo_minor_tasks = (e & 0x20) == 0x20;  // $0010 0000
	_echo_warnings = (e & 0x10) == 0x10;     // $0001 0000
	_echo_parsed_major = (e & 0x08) == 0x08; // $0000 1000
	_echo_parsed_minor = (e & 0x04) == 0x04; // $0000 0100
	_echo_source = (e & 0x02) == 0x02;       // $0000 0010
	_echo_rom_data = (e & 0x01) == 0x01;     // $0000 0001
}

bool Assembler::EchoMajorTasks()
{
	return _echo_major_tasks;
}

bool Assembler::EchoMinorTasks()
{
	return _echo_minor_tasks;
}

bool Assembler::EchoWarnings()
{
	return _echo_warnings;
}

bool Assembler::EchoSource()
{
	return _echo_source;
}

bool Assembler::EchoParsedMajor()
{
	return _echo_parsed_major;
}

bool Assembler::EchoParsedMinor()
{
	return _echo_parsed_minor;
}

bool Assembler::EchoRomData()
{
	return _echo_rom_data;
}

SymbolType Assembler::GetSymbolType(const std::string& n)
{
	auto i = _symbols.find(n);
	return (i->second).GetType();
}

bool Assembler::IsAMnemonic(const std::string& s) 
{ 
	return std::find(_mnemonics.begin(), _mnemonics.end(), s) != _mnemonics.end(); 
}

int Assembler::GetValueByUniqueOpcodeString(const std::string& m)
{
	for (auto it = _opcodes.begin(); it != _opcodes.end(); ++it)
		if (it->second.GetUniqueString() == m)
			return it->first;
}

int Assembler::GetValueByUniqueOpcodeAliasString(const std::string& m)
{
	for (auto it = _opcode_aliases.begin(); it != _opcode_aliases.end(); ++it)
		if (it->second.GetUniqueString() == m)
			return it->first;
}

int Assembler::NumOpcodeCycles() 
{ 
	return _opcodes[_lastOpcodeIndex].numArgs(); 
}

int Assembler::LastOpcodeIndex() 
{ 
	return _lastOpcodeIndex; 
}

Opcode& Assembler::GetOpcode(int v) 
{ 
	return _opcodes[v]; 
}

void Assembler::SetAddress(int a)
{
	_address = a;

	if (_address > _max_address) _max_address = _address;

	ValidateAddress();
}

int Assembler::GetAddress() const
{
	ValidateAddress();
	return _address;
}

void Assembler::ValidateAddress() const
{
	if (_address < 0 || _address >= int(_program_rom.GetSize()))
	{
		std::stringstream msg;
		msg << "Assembling address out of bounds: " << std::hex << "0x" << _address;
		msg << " (" << dec << _address << ")";
		throw std::exception(msg.str().c_str());
	}
}

int Assembler::GetSymbolAddress(const std::string& n) const
{
	auto i = _symbols.find(n);
	return (i->second).GetAddress();
}

void Assembler::AddLabel(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeLabel(n, a, l));
}

void Assembler::AddVariable(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeVariable(n, a, l));
}

void Assembler::AddFlag(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeFlag(n, a, l));
	_nFlags++;
}

void Assembler::AddRegister(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeRegister(n, a, l));
}

void Assembler::AddControlLine(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeControlLine(n, a, l));

	if (a > _maxControlLineValue) _maxControlLineValue = a;
}

void Assembler::AddOpcode(int v, const Opcode& oc)
{
	_lastOpcodeIndex = v;
	_opcodes.emplace(v, oc);

	if (v > _maxOpcodeValue) _maxOpcodeValue = v;

	_mnemonics.push_back(_opcodes[v].mnemonic());
	RegisterInstruction<OpcodeInstruction>(_opcodes[v].GetUniqueString());
}

void Assembler::AddOpcodeAlias(int v, const Opcode& oca)
{
	_opcode_aliases.emplace(v, oca);

	_mnemonics.push_back(_opcode_aliases[v].mnemonic());
	RegisterInstruction<OpcodeAliasInstruction>(_opcode_aliases[v].GetUniqueString());
}

void Assembler::AddControlPatternToCurrentOpcode(int cp)
{
	_opcodes[_lastOpcodeIndex].AddControlPattern(cp);
	if (_opcodes[_lastOpcodeIndex].numCycles() > _maxNumCycles) _maxNumCycles = _opcodes[_lastOpcodeIndex].numCycles();
}

void Assembler::AddLabelReference(const std::string& n, int a, int l)
{
	_symbolReferences.emplace(n, Symbol::MakeLabel(n, a, l));
}

void Assembler::AddVariableReference(const std::string& n, int a, int l)
{
	_symbolReferences.emplace(n, Symbol::MakeVariable(n, a, l));
}

void Assembler::AddRegisterReference(const std::string& n, int a, int l)
{
	_symbolReferences.emplace(n, Symbol::MakeRegister(n, a, l));
}

void Assembler::AddControlLineReference(const std::string& n, int a, int l)
{
	_symbolReferences.emplace(n, Symbol::MakeControlLine(n, a, l));
}

void Assembler::ProcessFile(std::istream& file, int startLine)
{
	int linenum = 1;
	std::string line;

	try
	{
		while (std::getline(file, line))
		{
			if (startLine != 0 && linenum <= startLine)
			{
				linenum++;
				continue;
			}

			if (_echo_source)
				std::cout << "  --> source line #" << linenum << " = " << line << "\n";

			ProcessLine(line, linenum, !_arch_defined);
			_sourceLines.emplace_back(line, linenum++);

		}
	}
	catch (const std::ifstream::failure& e)
	{
		if (!file.eof())
		{
			throw e;
		}
		else
		{
			if (_sourceStack.size() > 1)
				PopFileFromSourceStack();
		}
	}
}

void Assembler::ProcessLine(std::string& line, int linenum, bool preprocessing)
{
	// Remove comments
	Parser::Instance().strip_comment(line);

	if (line == END_ARCH_STR)
	{
		_commands[END_ARCH_STR]->Process(*this, END_ARCH_STR, line, linenum);
		return;
	}

	std::string line_copy = line;
	bool modifyLine = false;
	std::string modified_line = line;
	int saved_value_0 = -1;
	int saved_value_1 = -1;
	if (_arch_defined)
	{
		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto token = Parser::Instance().extract_token_ws_comma(line);
			if (token.has_value())
			{
				std::string tokenString = std::string(token.value());

				if (IsAMnemonic(tokenString))
				{
					modifyLine = true;
					modified_line = "";
				}

				if (modifyLine)
				{
					if (tokenString[0] == BIN_KEY || tokenString[0] == HEX_KEY || tokenString[0] == DEC_KEY || isdigit(tokenString[0]))
					{
						int val = Parser::Instance().parse_literal_num(tokenString);

						float max_val = pow(2, _instruction_width * 8);
						if (val > 2 * max_val - 1)
						{
							std::stringstream msg;
							msg << "Assembling instruction " << line_copy << " at line <" << line << ">! Too many bytes follow the instruction!";
							throw std::exception(msg.str().c_str());
						}
						else if (val <= 2 * max_val - 1 && val > max_val - 1)
						{
							saved_value_0 = val & (_instruction_width * 8 - 1);
							saved_value_1 = (val >> (_instruction_width * 8)) & (_instruction_width * 8 - 1);
						}
						else
						{
							saved_value_0 = val;
							saved_value_1 = -1;
						}
					}

					if (saved_value_0 != -1 || saved_value_1 != -1)
					{
						modified_line += "#_";
					}
					else
					{
						modified_line += tokenString + "_";
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		if (modifyLine)
		{
			modified_line = std::string(modified_line.begin(), modified_line.end() - 1);

			if (_echo_parsed_minor)
				std::cout << "\n       --> Line was modified to " << modified_line << "\n\n";
		}
	}

	if (modifyLine)
		line = modified_line;
	else
		line = line_copy;

	auto token = Parser::Instance().extract_token_ws(line);



	if (!token.has_value() && !modifyLine)
	{
		return;

	}

	if (modifyLine && _instructions.count(modified_line) > 0)
	{
		if (!preprocessing)
		{
			_instructions[modified_line]->Process(*this, modified_line, saved_value_0, saved_value_1, _address);
			return;
		}
	}

	std::string tokenString = "";
	if (!modifyLine)
		tokenString = std::string(token.value());
	else
		tokenString = modified_line;

	if (tokenString.front() == '{' || tokenString.front() == '}')
	{
		if (tokenString.front() == '}' && _echo_parsed_major)
			std::cout << "\n";

		return;
	}

	if (Parser::Instance().is_name(token.value()))
	{
		if (_symbols.count(token.value()) > 0)
		{
			std::stringstream msg;
			msg << "Found a symbol at line <" << linenum << ">! Symbol [" << token.value() << "] already exists!";
			throw std::exception(msg.str().c_str());
		}
		else
		{
			if (_commands.count(token.value()) > 0)
			{
				if (preprocessing)
					_commands[token.value()]->Process(*this, token.value(), line, linenum);
			}
		}
	}
	else if (Parser::Instance().is_directive(token.value()))
	{
		std::string tokenString = std::string(token.value());

		// strip off the dot
		token.value().erase(token.value().begin(), token.value().begin() + 1);

		if (_directives.count(token.value()) == 0)
		{
			std::stringstream msg;
			msg << "Unknown directive at line <" << linenum << ">! Found [."
				<< token.value() << "]";
			throw std::exception(msg.str().c_str());
		}

		// process directive
		_directives[token.value()]->Process(*this, token.value(), std::move(line), linenum);
	}
	else if (Parser::Instance().is_label(token.value()))
	{
		std::string tokenString = std::string(token.value());
		Parser::Instance().try_strip_label(tokenString);

		AddLabel(tokenString, _address, linenum);

		if (_echo_parsed_major)
			std::cout << "          *** Adding label [" << tokenString << "] at address = $" << _address << "\n\n";
	}
	else
	{
		std::stringstream msg;
		msg << "Unknown token <" << linenum << ">! It sez [" << token.value() << "].";
		throw std::exception(msg.str().c_str());
	}

	if (preprocessing && !((token.value() == ARCH_STR || token.value() == INCLUDE_STR || token.value() == INSERT_STR)))
	{
		line = line_copy;
	}

	return;
}

void Assembler::AddDecoderRom(bool write, int inputs, int outputs)
{
	_write_decode_rom = write;
	_in_bits_decode = inputs;
	_out_bits_decode = outputs;
}

void Assembler::WriteDecoderRom()
{
	_seq_bits_decode = (int)(ceil(log2f(_maxNumCycles)));
	_oc_bits_decode = (int)(ceil(log2f(_maxOpcodeValue)));














	// ***************************************************
	// SUPER TEMPORARY!!
	// ***************************************************
	_seq_bits_decode = 3;
	_oc_bits_decode = 8;














	int _in_bits_needed_decode = (_nFlags + _seq_bits_decode + _oc_bits_decode) / 8;
	_in_bits_needed_decode *= 8;

	if (_echo_parsed_minor)
	{
		std::cout << dec << "   MAX CYCLES = " << _maxNumCycles << "\n";
		std::cout << "   MAX OC VAL = " << _maxOpcodeValue << "\n";
		std::cout << "   IN seq = " << dec << _seq_bits_decode << "   oc = " << _oc_bits_decode << "   f = " << _nFlags << "\n";
		std::cout << "  --> Writing decode ROM : " << _in_bits_needed_decode << " inputs are required\n";
	}

	if (_in_bits_decode < _in_bits_needed_decode)
	{
		std::stringstream msg;
		msg << "Attempting to write rom :  -- " << _in_bits_needed_decode << " bits are required but only " << _in_bits_decode << " were specified.\n";
		throw std::exception(msg.str().c_str());
	}

	int out_bits_needed = (int)(log2f(_maxControlLineValue));
	_rom_size_decode = _out_bits_decode * pow(2, _in_bits_needed_decode);

	if (_echo_parsed_minor)
	{
		std::cout << "  --> Writing ROM : " << out_bits_needed << " outputs are required\n";
		std::cout << "  size = " << _rom_size_decode << "\n";
	}

	int _n_roms_decode = 1;

	if (_out_bits_decode < out_bits_needed)
		_n_roms_decode = ceil((float)out_bits_needed / (float)_out_bits_decode);

	if (_echo_parsed_minor)
	{
		std::cout << "  max cl = " << hex8 << _maxControlLineValue << "\n";
	}

	if (_echo_parsed_major)
	{
		std::cout << " *** For decode Rom, splitting into " << dec << _n_roms_decode << " files since that many Roms will be needed\n";
	}

	if (_write_decode_rom)
	{
		std::string path = "roms\\";
		std::string name = _start_file;
		name = _start_file.substr(_start_file.find_last_of("/\\") + 1);
		std::string::size_type const p(name.find_last_of('.'));
		name = name.substr(0, p);
		name += "_decoder";

		for (int i = 0; i < _n_roms_decode; i++)
		{
			if (_echo_parsed_major)
				std::cout << "\nWriting to rom " << i << "...\n\n";

			std::string path_i = path + name + "_" + std::to_string(i) + ".bin";

			if (_out_bits_decode ==  8) WriteDecoderRom_Internal<uint8_t> (path_i, i);
			if (_out_bits_decode == 16) WriteDecoderRom_Internal<uint16_t>(path_i, i);
			if (_out_bits_decode == 32) WriteDecoderRom_Internal<uint32_t>(path_i, i);
			if (_out_bits_decode == 64) WriteDecoderRom_Internal<uint64_t>(path_i, i);
		}
	}
	else if (_echo_warnings)
	{
		std::cout << "(warning) You have requested to write to the decode rom but the architecture file specified that it should not written to...ignoring your request\n";
		std::cout << "Please modify architecture file if you would like to write to this rom\n";
	}
}

void Assembler::AddProgramRom(bool write, int inputs, int outputs)
{
	int size = outputs * pow(2, inputs);

	_program_rom.SetSize(size);
	_program_rom.Initialize(0);

	_write_program_rom = write;
	_in_bits_program = inputs;
	_out_bits_program = outputs;
}

void Assembler::AddByteToProgramRom(unsigned char byte)
{
	ValidateAddress();

	int print_byte = byte;

	if (_echo_parsed_minor)
		std::cout << "     --> program rom[" << _address << "] = $" << hex2 << print_byte << "\n";

	_program_rom.AddData(_address, byte);

	if (_address > _max_address) _max_address = _address;

	_address++;
}

void Assembler::WriteProgramRom()
{
	if (_write_program_rom)
	{
		std::string path = "roms\\";		
		std::string name = _start_file;
		name = _start_file.substr(_start_file.find_last_of("/\\") + 1);
		std::string::size_type const p(name.find_last_of('.'));
		name = name.substr(0, p);

		_program_rom.WriteData((path + name + ".bin").c_str(), _in_bits_program / 8, 0, _max_address, _echo_rom_data, 0);
	}
}

void Assembler::ResolveDependencies()
{
	for (auto it = _symbols.cbegin(), next_it = _symbols.cbegin(); it != _symbols.cend(); it = next_it)
	{
		next_it = std::next(it);

		bool unreferenced = true;
		auto irp = _symbolReferences.equal_range(it->first);
		while (irp.first != irp.second)
		{
			if (irp.first->second.GetType() == it->second.GetType())
			{
				//_rom.GetData()[irp.first->second.GetAddress()] = it->second.GetAddress();
				unreferenced = false;
				_symbolReferences.erase(irp.first++);
			}
			else
			{
				std::cout << "Reference <" << irp.first->first << "> from line ["
					<< irp.first->second.GetLine() << "] does not match symbol <"
					<< it->first << "> defined at line [" << it->second.GetLine()
					<< "]" << std::endl;
				irp.first++;
			}
		}

		if (!unreferenced)
		{
			_symbols.erase(it);
		}
	}

	// show warnings for unrefd symbols
	if (_echo_warnings)
	{
		for (auto& sp : _symbols)
		{
			std::cout << "(warning) Unreferenced symbol <" << sp.first << "> declared "
				<< "at line [" << sp.second.GetLine() << "]" << std::endl;
		}
	}

	// show errors for unresolved references
	for (auto& rp : _symbolReferences)
	{
		std::cout << "Unresolved reference <" << rp.first << "> referenced "
			<< "at line [" << rp.second.GetLine() << "] !!" << std::endl;
	}

	if (_symbolReferences.size() > 0)
	{
		throw std::exception("Halting assembly due to unresolved references!!");
	}
}
