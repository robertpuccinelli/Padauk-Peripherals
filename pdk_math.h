/* pdk_math.h

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


//===========//
// VARIABLES //
//===========//

EXTERN EWORD &math_remainder;
EXTERN EWORD &math_quotien;
EXTERN EWORD &math_divisor;
EXTERN EWORD &math_dividend;

EXTERN WORD &math_mult_a;
EXTERN WORD &math_mult_b;
EXTERN DWORD &math_product;


//===================//
// PROGRAM FUNCTIONS //
//===================//
                       
void byte_divide   (void);
void word_divide   (void);
void eword_divide  (void);
void byte_multiply (void);
void word_multiply (void);