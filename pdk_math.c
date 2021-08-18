/* pdk_math.c

Basic multiplication and division functions.

ROM Consumed : 284B / 0x11C
RAM Consumed : 11B  / 0x0B


NOTE:

	Function maximum run time @ 4 MHz

	byte_divide   28  us
	word_divide   92  us
	eword_divide  148 us
	dword_divide  204 us

	byte_multiply 30  us  3 us WITH MULTIPLIER
	word_multiply 62  us  9 us WITH MULTIPLIER 

	Division framework sourced from Wikipedia Division Algorithm, Long Div.
	Div was made more efficient by having quotient and dividend share memory.
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

STATIC BYTE  math_byte1;
STATIC EWORD math_eword1;
STATIC EWORD math_eword2;
STATIC DWORD math_dword1;


//==================//
// VARIABLE ALIASES //
//==================//


DWORD &math_dividend  = math_dword1$0;
EWORD &math_divisor   = math_eword1$0;
EWORD &math_remainder = math_eword2$0;
DWORD &math_quotient  = math_dividend$0;

WORD &math_mult_a     = math_eword1$0;
WORD &math_mult_b     = math_eword2$0;
DWORD &math_product   = math_dword1$0;

STATIC BYTE &counter = math_byte1$0;

//===================//
// PROGRAM FUNCTIONS //
//===================//

void byte_divide(void)
{
	counter = 8;
	math_remainder = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_remainder$0;

			if (math_remainder$0 >= math_divisor$0)
			{
				math_remainder$0 -= math_divisor$0;
				math_quotient  |= 0b1;
			}

		} while(--counter);
	}
}


void word_divide(void)
{
	counter = 16;
	math_remainder = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_dividend$1;

			slc math_remainder$0;
			slc math_remainder$1;

			if (math_remainder >= math_divisor)
			{
				math_remainder -= math_divisor;
				math_quotient  |= 0b1;
			}

		} while(--counter);
	}
}


void eword_divide(void)
{
	counter = 24;
	math_remainder = 0;

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

			if (math_remainder >= math_divisor)
			{
				math_remainder -= math_divisor;
				math_quotient  |= 0b1;
			}

		} while(--counter);
	}
}

void dword_divide(void)
{
	counter = 32;
	math_remainder = 0;

	if (math_dividend < math_divisor) math_remainder = math_dividend;
	else
	{
		do 
		{
			sl  math_dividend$0;
			slc math_dividend$1;
			slc math_dividend$2;
			slc math_dividend$3;

			slc math_remainder$0;
			slc math_remainder$1;
			slc math_remainder$2;

			if (math_remainder >= math_divisor)
			{
				math_remainder -= math_divisor;
				math_quotient |= 0b1;
			}

		} while(--counter);
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
	counter = 8;

	do {
		math_mult_a$0 >>= 1;
		if (CF)	math_product += (math_mult_b$0 << 8);
		math_product >>= 1;
	} while(--counter);
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
	counter = 16;

	do {
		math_mult_a >>= 1;
		if (CF) math_product += (math_mult_b << 16);
		math_product >>>= 1;
	} while(--counter);
#ENDIF
}