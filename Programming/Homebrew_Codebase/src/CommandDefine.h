#pragma once

#include "Command.h"
#include "Opcode.h"
#include "Util.h"

enum Operation { None, OR, AND };

class InstructionWidthCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto sizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		std::cout << "          *** Instruction Width set to " << std::string(sizeToken.value()) << "\n";
		assembler.SetInstructionWidth(stoi(std::string(sizeToken.value())));
	}
};

class AddressWidthCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto sizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		std::cout << "          *** Address Width set to " << std::string(sizeToken.value()) << "\n";
		assembler.SetAddressWidth(stoi(std::string(sizeToken.value())));
	}
};

class ProgramROMSizeCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto sizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		std::cout << "          *** Program ROM size set to " << std::string(sizeToken.value()) << "bytes\n";
		assembler.SetProgramROMsize(stoi(std::string(sizeToken.value())));
	}
};

class RegisterAddCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto sizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());
				std::cout << "          *** Adding " << std::string(sizeToken.value()) << "-bit Register [" << nameTokenString << "]\n";

				assembler.AddRegister(nameTokenString, stoi(std::string(sizeToken.value())), line);
			}
			else
			{
				tokensRemain = false;
			}
		}
	}
};

class FlagAddCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());
				std::cout << "          *** Adding flag [" << nameTokenString << "]\n";

				assembler.AddFlag(nameTokenString, 0, line);
			}
			else
			{
				tokensRemain = false;
			}
		}
	}
};

class DeviceAddCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());
				std::cout << "          *** Adding device [" << nameTokenString << "]\n";

				//assembler.AddDevice(nameTokenString, 0, line);
			}
			else
			{
				tokensRemain = false;
			}
		}
	}
};

class ControlLineCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto nameToken = Parser::Instance().extract_token_ws(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! No label provided for control line!";
			throw std::exception(msg.str().c_str());
		}

		bool tokensRemain = true;
		int firstNum = -1;
		int op = 0;
		int secondNum = -1;
		while (tokensRemain)
		{
			// Get the rest of 
			auto nextToken = Parser::Instance().extract_token_ws_comma(remainder);
			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());
				LiteralNumType type = Parser::Instance().get_num_type(tokenString);

				if (type != LiteralNumType::None)
				{
					int num = Parser::Instance().parse_literal_num(tokenString, type);
					if (num != -1)
					{
						if (op == 0) firstNum = num;
						else         secondNum = num;
					}
					else
					{
						std::stringstream msg;
						msg << "Assembling command " << label << " at line <" << line << ">! Bad int literal parse type!";
						throw std::exception(msg.str().c_str());
					}
				}
				else
				{
					if (tokenString[0] == '<' && tokenString[1] == '<') op = -1;
					else if (tokenString[0] == '>' && tokenString[1] == '>') op = 1;
					else
					{
						if (!isdigit(tokenString.c_str()[0]))
						{
							if (tokenString[0] == '|')
							{
								op = Operation::OR;
							}
							else if (tokenString[0] == '=')
							{
								continue;
							}
							else
							{
								if (firstNum == -1)
									firstNum = 0;

								if (op == Operation::OR)
								{
									if (tokenString[0] == '_')
										firstNum = firstNum ^ assembler.GetSymbolAddress(tokenString);
									else
										firstNum = firstNum | assembler.GetSymbolAddress(tokenString);

									op = Operation::None;
								}
								else
								{
									firstNum = assembler.GetSymbolAddress(tokenString);
								}
							}
						}
						else
						{
							std::stringstream msg;
							msg << "Assembling command " << label << " at line <" << line << ">! Expected a symbol reference -- found !" << tokenString;
							throw std::exception(msg.str().c_str());
						}
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		int finalNum = -1;
		if (op != 0)
		{
			if (op == -1) finalNum = firstNum << secondNum;
			if (op == 1)  finalNum = firstNum >> secondNum;
		}
		else
		{
			finalNum = firstNum;
		}

		std::cout << "          *** Saving control line " << std::string(nameToken.value()) << ":\n";
		std::cout << "               value = " << " = $" << hex8 << finalNum;
		std::cout << " = %" << std::bitset<sizeof(int) * 8>(finalNum) << "\n";

		assembler.AddControlLine(std::string(nameToken.value()), finalNum, line);
	}
};

class PCincCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto nameToken = Parser::Instance().extract_token_ws(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! No label provided for control line!";
			throw std::exception(msg.str().c_str());
		}

		bool tokensRemain = true;
		int firstNum = -1;
		int op = 0;
		int secondNum = -1;
		while (tokensRemain)
		{
			// Get the rest of 
			auto nextToken = Parser::Instance().extract_token_ws_comma(remainder);
			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());
				LiteralNumType type = Parser::Instance().get_num_type(tokenString);

				if (type != LiteralNumType::None)
				{
					int num = Parser::Instance().parse_literal_num(tokenString, type);
					if (num != -1)
					{
						if (op == 0) firstNum = num;
						else         secondNum = num;
					}
					else
					{
						std::stringstream msg;
						msg << "Assembling command " << label << " at line <" << line << ">! Bad int literal parse type!";
						throw std::exception(msg.str().c_str());
					}
				}
				else
				{
					if (tokenString[0] == '<' && tokenString[1] == '<') op = -1;
					else if (tokenString[0] == '>' && tokenString[1] == '>') op = 1;
					else
					{
						if (!isdigit(tokenString.c_str()[0]))
						{
							if (tokenString[0] == '|')
							{
								op = Operation::OR;
							}
							else if (tokenString[0] == '=')
							{
								continue;
							}
							else
							{
								if (firstNum == -1)
									firstNum = 0;

								if (op == Operation::OR)
								{
									if (tokenString[0] == '_')
										firstNum = firstNum ^ assembler.GetSymbolAddress(tokenString);
									else
										firstNum = firstNum | assembler.GetSymbolAddress(tokenString);

									op = Operation::None;
								}
								else
								{
									firstNum = assembler.GetSymbolAddress(tokenString);
								}
							}
						}
						else
						{
							std::stringstream msg;
							msg << "Assembling command " << label << " at line <" << line << ">! Expected a symbol reference -- found !" << tokenString;
							throw std::exception(msg.str().c_str());
						}
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		int finalNum = -1;
		if (op != 0)
		{
			if (op == -1) finalNum = firstNum << secondNum;
			if (op == 1)  finalNum = firstNum >> secondNum;
		}
		else
		{
			finalNum = firstNum;
		}

		std::cout << "          *** Saving PC_INC control line " << std::string(nameToken.value()) << ":\n";
		std::cout << "               value = " << " = $" << hex8 << finalNum;
		std::cout << " = %" << std::bitset<sizeof(int) * 8>(finalNum) << "\n";

		assembler.AddControlLine(std::string(nameToken.value()), finalNum, line);
		assembler.SetPCincControlValue(finalNum);
	}
};

class OpcodeCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		Opcode opcode;

		auto valueToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!valueToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid value!";
			throw std::exception(msg.str().c_str());
		}

		std::string valueString = std::string(valueToken.value());
		int parsedValue = Parser::Instance().parse_literal_num(valueString);

		opcode.SetValue(parsedValue);

		auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid label!";
			throw std::exception(msg.str().c_str());
		}

		opcode.SetMnemonic(std::string(nameToken.value()));

		bool tokensRemain = true;
		Operation op = Operation::None;
		int seq = 0;
		int num = -1;
		while (tokensRemain)
		{
			auto nextToken = Parser::Instance().extract_token_ws_comma(remainder);

			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());

				if (tokenString[0] == '|')
				{
					op = Operation::OR;
				}
				else if (tokenString[0] == '=')
				{
					continue;
				}
				else if (tokenString[0] == '#')
				{
					Opcode::Arg newArg;
					newArg._type = ArgType::Numeral;
					newArg._string = '#';

					opcode.AddArgument(newArg);
					std::cout << "          *** Adding an immediate value argument [" << newArg._string << "]\n";
				}
				else if (assembler.GetSymbolType(tokenString) == SymbolType::Register)
				{
					Opcode::Arg newArg;
					newArg._type = ArgType::Register;
					newArg._string = tokenString;

					opcode.AddArgument(newArg);

					std::cout << "          *** Adding a register value argument [" << newArg._string << "]\n";
				}
				else
				{
					if (num == -1)
						num = 0;

					if (op == Operation::OR)
					{
						if (tokenString[0] == '_')
							num = num ^ assembler.GetSymbolAddress(tokenString);
						else
							num = num | assembler.GetSymbolAddress(tokenString);

						op = Operation::None;
					}
					else
					{
						num = assembler.GetSymbolAddress(tokenString);
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		std::cout << "          *** Saving opcode " << std::string(nameToken.value()) << " ";
		if (num != -1)
			opcode.AddControlPattern(num);

		assembler.AddOpcode(parsedValue, opcode);

		for (int i = 0; i < opcode.numArgs(); i++)
		{
			std::cout << opcode.arg(i)._string;

			if (i != opcode.numArgs() - 1)
				std::cout << ", ";
		}

		std::cout << " -- val = $";
		std::cout << hex2 << opcode.value();
		std::cout << ", control sequence : ";

		for (int i = 0; i < opcode.numCycles(); i++)
		{
			std::cout << "              " << dec << i << ": $" << hex8 << opcode.controlPattern(i) << "\n";
		}

		std::cout << ", unique_str = " << opcode.GetUniqueString() << "\n";
	}
};


class OpcodeAliasCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		Opcode opcode_alias;

		auto valueToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!valueToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid value!";
			throw std::exception(msg.str().c_str());
		}

		std::string valueString = std::string(valueToken.value());
		int parsedValue = Parser::Instance().parse_literal_num(valueString);

		opcode_alias.SetValue(parsedValue);

		auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Opcode is not assigned a valid label!";
			throw std::exception(msg.str().c_str());
		}

		opcode_alias.SetMnemonic(std::string(nameToken.value()));

		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nextToken = Parser::Instance().extract_token_ws_comma(remainder);

			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());

				if (tokenString[0] == '#')
				{
					Opcode::Arg newArg;
					newArg._type = ArgType::Numeral;
					newArg._string = '#';

					opcode_alias.AddArgument(newArg);
					std::cout << "          *** Adding an immediate value argument [" << newArg._string << "]\n";
				}
				else if (assembler.GetSymbolType(tokenString) == SymbolType::Register)
				{
					Opcode::Arg newArg;
					newArg._type = ArgType::Register;
					newArg._string = tokenString;

					opcode_alias.AddArgument(newArg);

					std::cout << "          *** Adding a register value argument [" << newArg._string << "]\n";
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		std::cout << "          *** Saving opcode " << std::string(nameToken.value()) << " ";

		assembler.AddOpcodeAlias(parsedValue, opcode_alias);
		
		for (int i = 0; i < opcode_alias.numArgs(); i++)
		{
			std::cout << opcode_alias.arg(i)._string;

			if (i != opcode_alias.numArgs() - 1)
				std::cout << ", ";
		}

		std::cout << " -- to existing opcode with val = $";
		std::cout << hex2 << opcode_alias.value();
		std::cout << ", unique_str = " << opcode_alias.GetUniqueString() << "\n";
	}
};

class OpcodeAddSequenceCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		bool tokensRemain = true;
		Operation op = Operation::None;

		int num = 0;
		while (tokensRemain)
		{
			auto nextToken = Parser::Instance().extract_token_ws_comma(remainder);

			if (nextToken.has_value())
			{
				std::string tokenString = std::string(nextToken.value());

				if (tokenString[0] == '|')
				{
					op = Operation::OR;
				}
				else if (tokenString[0] == '=')
				{
					continue;
				}
				else
				{
					if (op == Operation::OR)
					{
						if (tokenString[0] == '_')
							num = num ^ assembler.GetSymbolAddress(tokenString);
						else
							num = num | assembler.GetSymbolAddress(tokenString);

						op = Operation::None;
					}
					else
					{
						num = assembler.GetSymbolAddress(tokenString);
					}
				}
			}
			else
			{
				tokensRemain = false;
			}
		}

		assembler.AddControlPatternToCurrentOpcode(num);

		Opcode opcode = assembler.GetOpcode(assembler.LastOpcodeIndex());
		
		for (int i = 0; i < opcode.numCycles(); i++)
		{
			std::cout << "              " << dec << i << ": $" << hex8 << opcode.controlPattern(i) << "\n";
		}
	}
};

class RomAddCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		auto nameToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!nameToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid name!";
			throw std::exception(msg.str().c_str());
		}

		auto inSizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!inSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid input size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(inSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << inSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		auto outSizeToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!outSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid output size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(outSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << outSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		assembler.WriteDecoderROMs("D:\\Projects\\Homebrew_CPU\\Programming\\Homebrew_Codebase\\roms\\", std::string(nameToken.value()), stoi(inSizeToken.value()), stoi(outSizeToken.value()));
	}
};

class EndArchCommand : public Command
{
public:
	virtual void Process(Assembler& assembler, const std::string& label, std::string& remainder, int line) const override
	{
		std::cout << "\n\nanal beads\n\n";
		assembler.AcknowledgeArchEndTag();
	}
};