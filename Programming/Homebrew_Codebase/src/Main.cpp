#include "Assembler.h"
#include "ALU.h"

#include <iostream>
#include <conio.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "No input file specified!" << std::endl;
	}
	else if (std::string{ argv[1] } == "--run-tests")
	{
	//	run_tests();
	}
	else
	{
		try
		{
			Assembler a(argv[1]);
			std::string file = std::string(argv[1]);
			file = std::string(file.begin(), file.end() - 2);
			a.SetEchoLevel(0x00); // unused, major_tasks, minor_tasks, warnings;  major_parsed, minor_parsed, source, rom

			a.Assemble("roms\\test.s");

			ALU alu;
			alu.Generate();
		}
		catch (const std::exception& e)
		{
			std::cout << "Fatal error: " << e.what() << std::endl;
		}
	}

	while (!_kbhit());

	return 0;
}