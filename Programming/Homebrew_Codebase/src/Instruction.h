#pragma once

#include <string>

class Instruction
{
public:
	virtual ~Instruction() {};
	virtual void Process(class Assembler& a, std::string& instruction_line, int instruction_value0, int instruction_value1, int startAddress) const = 0;
};

class InstructionAlias : public Instruction
{
public:
	InstructionAlias(const Instruction* instruction)
		:
		_instruction(instruction)
	{}

	virtual void Process(class Assembler& a, std::string& il, int iv0, int iv1, int sa) const override
	{
		_instruction->Process(a, il, iv0, iv1, sa);
	}

private:
	const Instruction* const _instruction;
};