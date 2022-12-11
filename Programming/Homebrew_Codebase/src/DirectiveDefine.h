#pragma once

#include "Directive.h"

#include <string>

class IncludeDirective : public Directive
{
public:
	virtual void Process(class Assembler& a, const std::string& d, std::string remainder, int line) const override
	{
		auto token = Parser::Instance().extract_token_str(remainder);
		if (!token.has_value())
		{
			// no data
			std::stringstream msg;
			msg << "Processing include directive ." << d << " at line <" << line << ">! No data!";
			throw std::exception(msg.str().c_str());
		}

		// check for garbage after directive
		{
			if (!remainder.empty())
			{
				std::stringstream msg;
				msg << "Processing include directive ." << d << " at line <" << line << ">! What is this garbage??? [";
				msg << remainder << "]!!";
				throw std::exception(msg.str().c_str());
			}
		}

		std::string tokenString = token.has_value() ? std::string(token.value()) : "";
		Parser::Instance().trim_ws(tokenString);

		if (!tokenString.empty())
		{
			std::cout << "  --> Processing include directive for file: " << tokenString << std::endl;
			a.PushFileToSourceStack("D:\\Projects\\Homebrew_CPU\\Programming\\Homebrew_Codebase\\code\\" + tokenString, line);
		}
		else
		{
			// bad value
			std::stringstream msg;
			msg << "Processing include directive " << d << " at line <" << line << ">! What is this garbage??? [";
			msg << tokenString << "]!!";
			throw std::exception(msg.str().c_str());
		}
	}
};

class OriginDirective : public Directive
{
public:
	virtual void Process(Assembler& a, const std::string& d, std::string remainder, int line) const override
	{
		auto valueToken = Parser::Instance().extract_token_ws_comma(remainder);
		if (!valueToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling directive at line <" << line << ">! Org is not assigned a valid value!";
			throw std::exception(msg.str().c_str());
		}

		std::string valueString = std::string(valueToken.value());
		int parsedValue = Parser::Instance().parse_literal_num(valueString);

		if (parsedValue != -1)
		{
			try
			{
				a.SetAddress(parsedValue);

				std::cout << "          *** Setting Address to $" << hex8 << parsedValue << "\n";
			}
			catch (const std::exception& e)
			{
				std::stringstream msg;
				msg << "Processing directive ." << d << " at line <" << line << ">! Encountered exception!" << std::endl;
				msg << "\t" << e.what();
				throw std::exception(msg.str().c_str());
			}
		}
		else
		{ 
			// bad value
			std::stringstream msg;
			msg << "Processing ORG directive " << d << " at line <" << line << ">! What is this garbage??? [";
			msg << parsedValue << "]!!";
			throw std::exception(msg.str().c_str());
		}
	}
};