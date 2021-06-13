/* pdk_i2c.h
   
I2C declarations based off of the PADAUK IDE v0.91 Code Generator. 
Define PERIPH_I2C in system_settings.h

ROM Consumed : 109B / 0x6D
RAM Consumed :  14B / 0x0E  -  WITH 4B FOR COMMUNICATION BUFFER


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

//===========//
// VARIABLES //
//===========//

BYTE	i2c_device;	// Device address. Program sets target address w/enum above.
WORD 	i2c_buffer;	// Pointer to program communication buffer.


//===================//
// PROGRAM FUNCTIONS //
//===================//

void	I2C_Initialize	(void);
void	I2C_Release		(void);
void	I2C_Write		(void);
void	I2C_Read 		(void);
