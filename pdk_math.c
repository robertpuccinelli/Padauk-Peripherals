/* pdk_math.c

Basic multiplication and division functions.

ROM Consumed : 210B / 0xD2
RAM Consumed : 13B  / 0x0D


NOTE:

	Function maximum run time @ 4 MHz

	byte_divide   31  us
	word_divide   101 us
	eword_divide  167 us

	byte_multiply 30  us  3 us WITH MULTIPLIER
	word_multiply 62  us  9 us WITH MULTIPLIER 

	Division framework sourced from Wikipedia Division Algorithm, Long Div
	Multiplication framework (non-mulop) duplicated from Padauk Code Gen
	Multiplication framework (mulop) developed from experience


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"

//===========//
// VARIABLES //
//===========//

EWORD math_eword1;
EWORD math_eword2;
EWORD math_eword3;
DWORD math_dword1;


//==================//
// VARIABLE ALIASES //
//==================//

EWORD &math_remainder = math_eword1$0;
EWORD &math_quotient  = math_eword2$0;
EWORD &math_divisor   = math_eword3$0;
EWORD &math_dividend  = math_dword1$0;

WORD &math_mult_a     = math_eword1$0;
WORD &math_mult_b     = math_eword2$0;
DWORD &math_product   = math_dword1$0;

STATIC BYTE &counter_div  = math_dword1$3;
STATIC BYTE &counter_mult = math_eword3$2;

//===================//
// PROGRAM FUNCTIONS //
//===================//

void byte_divide(void)
{
	counter_div = 8;
	math_remainder = 0;
	math_quotient = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_remainder$0;
			sl math_quotient$0;

			if (math_remainder$0 >= math_divisor$0)
			{
				math_remainder$0 -= math_divisor$0;
				math_quotient  |= 0b1;
			}

		} while(--counter_div);
	}
}


void word_divide(void)
{
	counter_div = 16;
	math_remainder = 0;
	math_quotient = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_dividend$1;

			slc math_remainder$0;
			slc math_remainder$1;

			sl  math_quotient$0;
			slc math_quotient$1;

			if (math_remainder >= math_divisor)
			{
				math_remainder -= math_divisor;
				math_quotient  |= 0b1;
			}

		} while(--counter_div);
	}
}


void eword_divide(void)
{
	counter_div = 24;
	math_remainder = 0;
	math_quotient = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_dividend$1;
			slc math_dividend$2;

			slc math_remainder$0;
			slc math_remainder$1;
			slc math_remainder$2;

			sl  math_quotient$0;
			slc math_quotient$1;
			slc math_quotient$2;

			if (math_remainder >= math_divisor)
			{
				math_remainder -= math_divisor;
				math_quotient  |= 0b1;
			}

		} while(--counter_div);
	}
}


void byte_multiply(void)
{
	math_product = 0;
#IF HAS_MULTIPLIER
	mulop = math_mult_a$0;
	A = math_mult_b$0;
	mul;
	math_product$0 = A;
	math_product$1 = mulrh;

#ELSE
	counter_mult = 8;

	do {
		math_mult_a$0 >>= 1;
		if (CF)	math_product += (math_mult_b$0 << 8);
		math_product >>= 1;
	} while(--counter_mult);
#ENDIF
}


void word_multiply(void)
{
	math_product = 0;
#IF HAS_MULTIPLIER
	mulop = math_mult_a$0;
	A = math_mult_b$0;
	mul;
	math_product$0 = A;
	math_product$1 = mulrh;

	A = math_mult_b$1;
	mul;
	math_product += (A << 8);
	math_product += (mulrh << 16);

	mulop = math_mult_a$1;
	A = math_mult_b$0;
	mul;
	math_product += (A << 8);
	math_product += (mulrh << 16);

	A = math_mult_b$1;
	mul;
	math_product += (A << 16);
	math_product += (mulrh << 24);

#ELSE
	counter_mult = 16;

	do {
		math_mult_a >>= 1;
		if (CF) math_product += (math_mult_b << 16);
		math_product >>>= 1;
	} while(--counter_mult);
#ENDIF
}