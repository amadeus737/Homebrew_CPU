#pragma once

#include "Assembler.h"
#include "Parser.h"

#include <string>
#include <iostream>
#include <sstream>
#include <string>

class Directive
{
public:
	virtual ~Directive() {};
	virtual void Process(class Assembler& a, const std::string& d, std::string remainder, int line) const = 0;
	virtual void Process(class Assembler& a, const std::string& d, std::string label, std::string remainder, int line) const
	{
		throw std::exception((std::string("Directive [.") + d + "] does not support labels [" + label + "]").c_str());
	}
};

class DirectiveAlias : public Directive
{
public:
	DirectiveAlias(const Directive* Directive)
		:
		_directive(Directive)
	{}

	virtual void Process(class Assembler& a, const std::string& d, std::string r, int l) const override
	{
		_directive->Process(a, d, r, l);
	}

private:
	const Directive* const _directive;
};