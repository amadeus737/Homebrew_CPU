#pragma once

#include <string>

class Command
{
public:
	virtual ~Command() {};
	virtual void Process(class Assembler& a, const std::string& mnemonic, std::string& remainder, int line) const = 0;
};

class CommandAlias : public Command
{
public:
	CommandAlias(const Command* command)
		:
		_command(command)
	{}

	virtual void Process(class Assembler& a, const std::string& m, std::string& r, int l) const override
	{
		_command->Process(a, m, r, l);
	}

private:
	const Command* const _command;
};