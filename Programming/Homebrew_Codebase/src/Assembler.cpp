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

	_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	PushFileToSourceStack(filename, 0);
}

Assembler::Assembler(std::istream& file)
{
	RegisterOperations();
	ProcessFile(file, 0);
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

	std::cout << "Processing file :" << filename << "\n";

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

	std::cout << "Popped " << filename << " from file stack...processing file now\n";

	_file.open(filename);

	ProcessFile(_file, linenum);
}

void Assembler::Assemble(std::string name)
{
	ResolveDependencies();

	_mout << "Symbol dependencies successfuly resolved." << std::endl;
		
	_program_rom.WriteData(name.c_str());
}

void Assembler::RegisterOperations()
{
	RegisterDirective<IncludeDirective>(INCLUDE_STR);
	RegisterDirectiveAlias(INCLUDE_STR, ARCH_STR);
	RegisterDirectiveAlias(INCLUDE_STR, INSERT_STR);
	RegisterDirective<OriginDirective>(ORIGIN_STR);

	RegisterCommand<InstructionWidthCommand>(INSTRUCTION_WIDTH_STR);
	RegisterCommand<AddressWidthCommand>(ADDRESS_WIDTH_STR);
	RegisterCommand<ProgramROMSizeCommand>(PROGRAM_ROM_SIZE_STR);
	RegisterCommand<RegisterAddCommand>(REGISTER_STR);
	RegisterCommand<FlagAddCommand>(FLAG_STR);
	RegisterCommand<DeviceAddCommand>(DEVICE_STR);
	RegisterCommand<ControlLineCommand>(CONTROL_STR);
	RegisterCommand<PCincCommand>(PC_INC_CTRL_STR);
	RegisterCommand<OpcodeCommand>(OPCODE_STR);
	RegisterCommand<OpcodeAliasCommand>(OPCODE_ALIAS_STR);
	RegisterCommand<OpcodeAddSequenceCommand>(OPCODE_SEQ_STR);
	RegisterCommand<RomAddCommand>(ROM_WRITE_STR);
	RegisterCommand<EndArchCommand>(END_ARCH_STR);
}

void Assembler::AcknowledgeArchEndTag() 
{
	_arch_defined = true; 
}

int Assembler::NumOpcodeCycles() 
{ 
	return _opcodes[_lastOpcodeIndex].numArgs(); 
}

int Assembler::LastOpcodeIndex() 
{ 
	return _lastOpcodeIndex; 
}

void Assembler::SetPCincControlValue(int v) 
{ 
	_pc_inc_val = v; 
}

Opcode& Assembler::GetOpcode(int v) 
{ 
	return _opcodes[v]; 
}

void Assembler::SetInstructionWidth(int w)
{
	_instruction_width = w;
}

void Assembler::SetAddressWidth(int w)
{
	_address_width = w;
}

int Assembler::GetInstructionWidth()
{
	return _instruction_width;
}

int Assembler::GetAddressWidth()
{
	return _address_width;
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

			std::cout << "\n  --> source line #" << linenum << " = " << line << std::endl;
			
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
		_arch_defined = true;
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
		return;

	if (Parser::Instance().is_name(token.value()))
	{
		if (_symbols.count(token.value()) > 0)
		{
			std::stringstream msg;
			msg << "Found a symbol at line <" << linenum << ">! Symbol [" << token.value() << "] already exists and I hate you!";
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
				<< token.value() << "]. tsk tsk.";
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

		std::cout << "          *** Adding label [" << tokenString << "] at address = $" << _address << "\n";
	}
	else
	{
		std::stringstream msg;
		msg << "What is this? I can't even. at line <" << linenum << ">! It sez [" << token.value() << "].";
		throw std::exception(msg.str().c_str());
	}

	if (preprocessing && !((token.value() == ARCH_STR || token.value() == INCLUDE_STR || token.value() == INSERT_STR)))
	{
		line = line_copy;
	}

	return;
}

void Assembler::SetProgramROMsize(int size)
{
	_program_rom.SetSize(size);
	_program_rom.Initialize(0);
}

void Assembler::AddByteToProgramROM(unsigned char byte)
{
	ValidateAddress();
	int print_byte = byte;
	std::cout << "     --> program rom[" << _address << "] = $" << hex2 << print_byte << "\n";
	_program_rom.AddData(_address, byte);

	_address++;
}

void Assembler::SetAddress(int a)
{
	_address = a;
	ValidateAddress();
}

int Assembler::GetAddress() const
{
	ValidateAddress();
	return _address;
}

int Assembler::GetSymbolAddress(const std::string& n) const
{
	auto i = _symbols.find(n);
	return (i->second).GetAddress();
}

SymbolType Assembler::GetSymbolType(const std::string& n) 
{
	auto i = _symbols.find(n);
	return (i->second).GetType();
}

void Assembler::AddLabel(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeLabel(n, a, l));
}

void Assembler::AddVariable(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeVariable(n, a, l));
}

void Assembler::AddRegister(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeRegister(n, a, l));
}

void Assembler::AddFlag(const std::string& n, int a, int l)
{
	_symbols.emplace(n, Symbol::MakeFlag(n, a, l));
	_nFlags++;

	std::cout << "# flags = " << _nFlags << "\n";
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
				_mout << "Reference <" << irp.first->first << "> from line ["
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
	for (auto& sp : _symbols)
	{
		_mout << "(warning) Unreferenced symbol <" << sp.first << "> declared "
			<< "at line [" << sp.second.GetLine() << "]" << std::endl;
	}

	// show errors for unresolved references
	for (auto& rp : _symbolReferences)
	{
		_mout << "Unresolved reference <" << rp.first << "> referenced "
			<< "at line [" << rp.second.GetLine() << "] !!" << std::endl;
	}

	if (_symbolReferences.size() > 0)
	{
		throw std::exception("Halting assembly due to unresolved references!!");
	}
}

void Assembler::WriteDecoderROMs(const std::string& path, const std::string& name, int inputs, int outputs)
{
	int seq_bits = (int)(ceil(log2f(_maxNumCycles)));
	int oc_bits = (int)(ceil(log2f(_maxOpcodeValue)));
	int in_bits_needed = (_nFlags + seq_bits + oc_bits) / 8;
	in_bits_needed *= 8;

	std::cout << dec << "   MAX CYCLES = " << _maxNumCycles << "\n";
	std::cout << "   MAX OC VAL = " << _maxOpcodeValue << "\n";

	std::cout << "   IN seq = " << dec << seq_bits << "   oc = " << oc_bits << "   f = " << _nFlags << "\n";

	std::cout << "  --> Writing ROM : [" << name << "] -- " << in_bits_needed << " inputs are required\n";

	if (inputs < in_bits_needed)
	{
		std::stringstream msg;
		msg << "Attempting to write rom : [" << name << "]" << " -- " << in_bits_needed << " bits are required but only " << inputs << " were specified.\n";
		throw std::exception(msg.str().c_str());
	}

	int out_bits_needed = (int)(log2f(_maxControlLineValue));
	int rom_size = outputs * pow(2, in_bits_needed);

	std::cout << "  --> Writing ROM : [" << name << "] -- " << out_bits_needed << " outputs are required\n";
	std::cout << "  size = " << rom_size << "\n";
	int n_roms = 1;

	if (outputs < out_bits_needed)
		n_roms = ceil((float)out_bits_needed / (float)outputs);
	std::cout << "  max cl = " << hex8 << _maxControlLineValue << "\n";
	std::cout << " --> For this ROM, splitting into " << dec << n_roms << " files since that many ROMs will be needed\n";

	if (outputs == 8)  WriteDecoderROMinternal<uint8_t>(path, name, in_bits_needed, n_roms, rom_size, seq_bits, oc_bits, outputs);
	if (outputs == 16) WriteDecoderROMinternal<uint16_t>(path, name, in_bits_needed, n_roms, rom_size, seq_bits, oc_bits, outputs);
	if (outputs == 32) WriteDecoderROMinternal<uint32_t>(path, name, in_bits_needed, n_roms, rom_size, seq_bits, oc_bits, outputs);
	if (outputs == 64) WriteDecoderROMinternal<uint64_t>(path, name, in_bits_needed, n_roms, rom_size, seq_bits, oc_bits, outputs);
}
