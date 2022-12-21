#pragma once

#include "Rom.h"

template <typename T>
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
		dec_lhs			= 0x07,
		add_lhs_rhs		= 0x08,
		add_inc_lhs_rhs	= 0x09,
		sub_lhs_rhs		= 0x0a,
		sub_dec_lhs_rhs	= 0x0b,
		mul_lo_lhs_rhs	= 0x0c,
		mul_hi_lhs_rhs	= 0x0d,
		div_lhs_rhs		= 0x0e,
		mod_lhs_rhs		= 0x0f,
		shl_lhs			= 0x10,
		shr_lhs			= 0x11,
		mshl_lhs_rhs	= 0x12,
		mshr_lhs_rhs	= 0x13,
		and_lhs_rhs		= 0x14,
		or_lhs_rhs		= 0x15,
		xor_lhs_rhs		= 0x16,
		not_lhs			= 0x17,
		cmp_lhs_rhs		= 0x18,
		bit_lhs_rhs		= 0x19,
		clc				= 0x1a,
		sei				= 0x1b,
		cli				= 0x1c
	};

	enum class Flags
	{
		D = 0x1000, // 0b0001 0000 0000 0000
		S = 0x0800, // 0b0000 1000 0000 0000
		Z = 0x0400, // 0b0000 0100 0000 0000
		V = 0x0200, // 0b0000 0010 0000 0000
		C = 0x0100  // 0b0000 0001 0000 0000
	};


	void Generate(int size)
	{
		_rom.SetSize(size);
		_rom.Initialize(0);

		T cmask = 0x01ff;

		// loop over all operation types
		for (T op = 0x00; i < 0x1f; i++)
		{
			bool isDivMod = op == div_lhs_rhs || op == mod_lhs_rhs;

			for (T lhs = 0x00; lhs < 0x0f; lhs++)
			{
				// cache lhs sign
				T lhs_sign = lhs & 0x80;

				for (T rhs = 0x00; rhs < 0x0f; rhs++)
				{
					// cache rhs sign
					T rhs_sign = rhs & 0x80;

					// ROM Input Address
					//  - bits 16-20 : 5-bit ALU operation code
					//  - bits 8-15  : RHS value
					//  - bits 0-7   : LHS value
					int address_lhs = lhs << 0x0f;
					int address_op = op << 0xff;
					address = address_op | address_cb | address_lhs | rhs;

					// ROM Outputs
					//  - bits 13-15 : Unused
					//  - bits 8-12  : Flags
					//  - bits 0-7   : ALU Result
					T output;

					switch (op)
					{
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
							output = (- lhs)& cmask;
							break;

						case pass_nrhs:
							output = (- rhs) & cmask;
							break;

						case inc_lhs:
							output = (lhs++) & cmask;
							break;

						case dec_lhs:
							output = (lhs--) & cmask;
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
							output = (rhs == 0) ? 0 : (T)(lhs / rhs);
							break;

						case mod_lhs_rhs:
							output = (rhs == 0) ? 0 : (T)(lhs % rhs);
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

					// modify output with flag values
					if ((output & 0xff) == 0) output |= (T)Flags::Z;
					if (output_sign) output |= (T)Flags::S;
					if ((lhs_sign == rhs_sign) && (lhs_sign != output_sign)) output |= (T)Flags::V;
					if (isDivMod && rhs == 0) output |= (T)Flags::D;
				}
			}
		}
	}

private:
	Rom<T> _rom;
};