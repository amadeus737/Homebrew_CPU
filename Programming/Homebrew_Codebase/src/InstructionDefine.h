#pragma once

#include "Instruction.h"
#include "Opcode.h"
#include "Util.h"

class OpcodeInstruction : public Instruction
{
public:
	virtual void Process(Assembler& assembler, std::string& instruction_line, int value0, int value1, int startAddress) const override
	{			
		int oc_value  = assembler.GetValueByUniqueOpcodeString(instruction_line);

		std::cout << "   PROCESSING INSTRUCTION " << instruction_line << " with val = " << oc_value;

		if (value0 != -1)
			std::cout << "; optional value : [0] = $" << hex2 << value0;
		
		if (value1 != -1)
			std::cout << "; optional value : [1] = $" << hex2 << value1;
		
		std:: cout << " at address = $" << hex4 << startAddress << "\n";
		
		int instruction_width = assembler.GetInstructionWidth();
		
		//for (int i = 0; i < instruction_width; i++)
		{
			assembler.AddByteToProgramROM(oc_value);
			/*
			if (i < instruction_width - 1)
				assembler.AddByteToProgramROM(0);
			else
				assembler.AddByteToProgramROM(oc_value);
				*/
		}

		if (value0 != -1)
		{
			assembler.AddByteToProgramROM(value0);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value0);
			}*/
		}

		if (value1 != -1)
		{
			assembler.AddByteToProgramROM(value1);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value1);
			}*/
		}
	}
};

class OpcodeAliasInstruction : public Instruction
{
	virtual void Process(Assembler& assembler, std::string& instruction_line, int value0, int value1, int startAddress) const override
	{	
		int oca_value = assembler.GetValueByUniqueOpcodeAliasString(instruction_line);

		std::cout << "   PROCESSING INSTRUCTION " << instruction_line << " with val = " << oca_value;

		if (value0 != -1)
			std::cout << "; optional value : [0] = $" << hex2 << value0;

		if (value1 != -1)
			std::cout << "; optional value : [1] = $" << hex2 << value1;

		std::cout << " at address = $" << hex4 << startAddress << "\n";

		int instruction_width = assembler.GetInstructionWidth();

		//for (int i = 0; i < instruction_width; i++)
		{
			assembler.AddByteToProgramROM(oca_value);
			/*
			if (i < instruction_width - 1)
				assembler.AddByteToProgramROM(0);
			else
				assembler.AddByteToProgramROM(oc_value);
				*/
		}

		if (value0 != -1)
		{
			assembler.AddByteToProgramROM(value0);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value0);
			}*/
		}

		if (value1 != -1)
		{
			assembler.AddByteToProgramROM(value1);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value1);
			}*/
		}
	}
};
