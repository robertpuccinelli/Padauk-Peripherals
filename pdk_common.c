/* pdk_utilities.c
   
Common utilities

ROM Consumed : 110B / 0x6E
RAM Consumed : 14B  / 0x0E


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

STATIC EWORD eword1 = 0;
STATIC EWORD eword2 = 0;

BYTE &remainder = eword1$0;
BYTE &quotient  = eword1$1;
BYTE &dividend  = eword1$2;
BYTE &divisor   = eword2$0;
STATIC BYTE &counter   = eword2$1;


void byte_divide(void)
{
	remainder = 0;
	quotient = 0;

	if (dividend < divisor) remainder = dividend;
	else
	{
		counter = 8;
		do 
		{
			sl  divisor$0;
			slc remainder;

			if (remainder >= divisor)
			{
				remainder -= divisor;
				quotient  |= 0b10000000;
			}

			quotient >>= 1;



		} while(--counter);
	}
}