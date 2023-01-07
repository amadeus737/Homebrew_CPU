#pragma once

#include "Rom.h"

#include <fstream>

class ALU
{
public:
	enum ALU_OPS
	{
		zero            = 0x00,
		pass_lhs		= 0x01,
		pass_rhs		= 0x02,
		pass_nlhs       = 0x03,
		pass_nrhs       = 0x04,
		inc_lhs			= 0x05,
		dec_lhs			= 0x06,
		add_lhs_rhs		= 0x07,
		add_inc_lhs_rhs	= 0x08,
		sub_lhs_rhs		= 0x09,
		sub_dec_lhs_rhs	= 0x0a,
		mul_lo_lhs_rhs	= 0x0b,
		mul_hi_lhs_rhs	= 0x0c,
		div_lhs_rhs		= 0x0d,
		mod_lhs_rhs		= 0x0e,
		shl_lhs			= 0x0f,
		shr_lhs			= 0x10,
		mshl_lhs_rhs	= 0x11,
		mshr_lhs_rhs	= 0x12,
		and_lhs_rhs		= 0x13,
		or_lhs_rhs		= 0x14,
		xor_lhs_rhs		= 0x15,
		not_lhs			= 0x16,
		cmp_lhs_rhs		= 0x17,
		bit_lhs_rhs		= 0x18,
		clc				= 0x19,
		sei				= 0x1a,
		cli				= 0x1b
	};						 

	enum class Flags
	{
		D = 0x1000, // 0b0001 0000 0000 0000
		S = 0x0800, // 0b0000 1000 0000 0000
		Z = 0x0400, // 0b0000 0100 0000 0000
		V = 0x0200, // 0b0000 0010 0000 0000
		C = 0x0100  // 0b0000 0001 0000 0000
	};

	void Generate()
	{
		std::string filename = "roms\\alu.bin";
		FILE* file = fopen(filename.c_str(), "wb");

		int cmask = 0x01ff;
		int address = 0x0000;

		int d, s, z, v, c;
		int data = 0;
		int last_data = -1;

		// loop over all operation types
		for (int op = 0x00; op < 0x20; op++)
		{
			std::cout << "Processing operation " << (int)op << "\n";
			
			bool isDivMod = op == div_lhs_rhs || op == mod_lhs_rhs;
			
			for (int lhs = 0x00; lhs <= 0xff; lhs++)
			{
				// cache lhs sign
				int lhs_sign = lhs & 0x80;
				
				for (int rhs = 0x00; rhs <= 0xff; rhs++)
				{
					d = 0;
					s = 0;
					z = 0;
					v = 0;
					c = 0;

					// cache rhs sign
					int rhs_sign = rhs & 0x80;

					// ROM Input Address
					//  - bits 16-20 : 5-bit ALU operation code
					//  - bits 8-15  : LHS value
					//  - bits 0-7   : RHS value
					int address_lhs = lhs << 0x08;
					int address_op = op << 0x10;
					address = address_op | address_lhs | rhs;

					// ROM Outputs
					//  - bits 13-15 : Unused
					//  - bits 8-12  : Flags
					//  - bits 0-7   : ALU Result
					int output = 0;
					int output_save;
					
					switch (op)
					{
					default:
						output = 0;
						break;

					case zero:
						output = 0;
						break;

					case pass_lhs:
						output = lhs;
						break;

					case pass_rhs:
						output = rhs;
						break;

					case pass_nlhs:
						output = (-lhs) & cmask;
						break;

					case pass_nrhs:
						output = (-rhs) & cmask;
						break;

					case inc_lhs:
						output = (lhs + 1) & cmask;
						break;

					case dec_lhs:
						output = (lhs - 1) & cmask;
						break;

					case add_lhs_rhs:
						output = (lhs + rhs) & cmask;
						break;

					case add_inc_lhs_rhs:
						output = (lhs + rhs + 1) & cmask;
						break;

					case sub_lhs_rhs:
						output = (lhs - rhs) & cmask;
						break;

					case sub_dec_lhs_rhs:
						output = (lhs - rhs - 1) & cmask;
						break;

					case mul_lo_lhs_rhs:
						output = (lhs * rhs) & 0xff;
						break;

					case mul_hi_lhs_rhs:
						output = (lhs * rhs) >> 8;
						break;

					case div_lhs_rhs:
						output = (rhs == 0) ? 0 : (int)(lhs / rhs);
						break;

					case mod_lhs_rhs:
						output = (rhs == 0) ? 0 : (int)(lhs % rhs);
						break;

					case shl_lhs:
						output = (lhs << 1) & cmask;
						break;

					case shr_lhs:
						output = (lhs >> 1);
						break;

					case mshl_lhs_rhs:
						output = (lhs << rhs) & cmask;
						break;

					case mshr_lhs_rhs:
						output = (lhs >> rhs);
						break;

					case and_lhs_rhs:
						output = lhs & rhs;
						break;

					case or_lhs_rhs:
						output = lhs | rhs;
						break;

					case xor_lhs_rhs:
						output = lhs ^ rhs;
						break;

					case not_lhs:
						output = (~lhs) & 0xff;
						break;

					case cmp_lhs_rhs:
						break;

					case bit_lhs_rhs:
						break;

					case clc:
						output &= 0xf0ff;
						break;

					case sei:
						break;

					case cli:
						break;
					}
					
					int output_sign = output & 0x80;
					output_save = output;

					// modify output with flag values
					if ((output & 0xff) == 0)
					{
						output |= (int)Flags::Z;
						z = 1;
					}

					if (output_sign)
					{
						output |= (int)Flags::S;
						s = 1;
					}

					if ((lhs_sign == rhs_sign) && (lhs_sign != output_sign))
					{
						output |= (int)Flags::V;
						v = 1;
					}

					if (isDivMod && rhs == 0)
					{
						output |= (int)Flags::D;
						d = 1;
					}
					
					int8_t byte0 = ((int16_t)output >> 8);
					int8_t byte1 = (int8_t)output;
					data = (byte0 << 8) | byte1;

					fwrite(&byte1, 1, 1, file);
					fwrite(&byte0, 1, 1, file);
					
					//if (data != last_data)
					if(0)
					{
						std::cout << std::hex << "  rom[" << std::setw(7) << address << "] = ";
						std::cout << std::setw(4) << output_save;
						std::cout << std::setw(2) << "\top = " << op << std::setw(2) << "\tlhs = ";
						std::cout << std::setw(2) << lhs << "\trhs = " << std::setw(2) << rhs << "\tfinal = " << byte1 << byte0;
						std::cout << "\td = " << d << "\ts = " << s << "\tz = " << z << "\tv = " << v << "\tc = " << c;
						std::cout << "\tdata = " << data << "\n";
					}
					
					last_data = data;
				}				
			}			
		}

		fclose(file);		
	}
};