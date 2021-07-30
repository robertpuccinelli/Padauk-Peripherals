/* system_settings.h

This file is used to configure peripherals on the target Padauk IC.

Include this file as well as the desired peripheral source and header files
into your Padauk IDE project. To configure:
    1. Change SYSTEM_CLOCK to match SYSCLK defined in your project
    2. Change IC_TARGET to match the Padauk microcontroller being programmed
        NOTE:     If your IC is not listed, create an entry in the
                SUPPORTED MICROCONTROLLERs section. It is important
                to correctly define the IC features. Refer to datasheet.
    3. Enable the desired pheripherals such as PERIPH_I2C
    4. Change peripheral configuration in the PERIPHERAL SETTINGS section

ROM Used : 0B
RAM Used : 0B


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/
#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#define SYSTEM_CLOCK   4000000   // Change to match your choice of SYSCLK in Hz
#define IC_TARGET      PMS132    // PMS150C, PMS132, PMS134

#define PERIPH_I2C_M   1         // I2C Master.    Disable: 0, Enable: 1
#define PERIPH_PWM_11B 1         // 11B PWM.       Disable: 0, Enable: 1
#define PERIPH_BUTTON  1         // Buttons.       Disable: 0, Enable: 1
#define PERIPH_LCD     1         // LCD.           Disable: 0, Enable: 1
#define PERIPH_EEPROM  1         // EEPROM.        Disable: 0, Enable: 1
#define PERIPH_STEPPER 1         // Stepper motor. Disable: 0, Enable: 1
#define PERIPH_TIMER8  1 

//======================//
// RESOURCE UTILIZATION //
//======================//
//
// Use this area to track which resources are used and where
//
//    PA0    -             PB0    -           PC0    X
//    PA1    X             PB1    BTN         PC1    X    
//    PA2    X             PB2    TM2         PC2    X
//    PA3    STEP_D        PB3    PWMG2       PC3    X
//    PA4    STEP_S        PB4    PWMG0       PC4    X
//    PA5    -             PB5    -           PC5    X
//    PA6    I2C_SDA       PB6    TM3         PC6    X
//    PA7    I2C_SCL       PB7    BTN         PC7    X
//
//    TM16   -
//    TM2    BTN
//    TM3    -
//
//    PWMG0  -
//    PWMG1  STEP_S
//    PWMG2  -
//
//    ADC    -
//    COMP   -
//    LCD    X
//    OPA    X
//    TOUCH  X
//


//============================//
// SUPPORTED MICROCONTROLLERS //
//============================//


#define ICE_ILRC_HZ 34700	// ILRC clock of ICE for code validation


#ifidni IC_TARGET, PMS150C
    #define HAS_MULTIPLIER 0
    #define HAS_PWM        1
    #define HAS_11B_PWM    0
    #define HAS_CMP        1
    #define HAS_ADC        0
    #define HAS_12B_ADC    0
    #define HAS_OPA        0
    #define ILRC_HZ        59000    // Varies with voltage + temperature
	#define INSTR_CYCLES   2        // 2T architecture - DO NOT CHANGE
#endif


#ifidni IC_TARGET, PMS132
    #define HAS_MULTIPLIER 1
    #define HAS_PWM        1
    #define HAS_11B_PWM    1
    #define HAS_CMP        1
    #define HAS_ADC        1
    #define HAS_12B_ADC    1
    #define HAS_OPA        0
    #define ILRC_HZ        54000    // Varies with voltage + temperature
	#define INSTR_CYCLES   2        // 2T architecture - DO NOT CHANGE

	#define INTR_TM3  7
	#define INTR_TM2  6
	#define INTR_PWM  5
	#define INTR_GPC  4
	#define INTR_ADC  3
	#define INTR_T16  2
	#define INTR_EXT1 1
	#define INTR_EXT0 0
#endif


#ifidni IC_TARGET, PMS134
    #define HAS_MULTIPLIER 1
    #define HAS_PWM        1
    #define HAS_11B_PWM    1
    #define HAS_CMP        1
    #define HAS_ADC        1
    #define HAS_12B_ADC    1
    #define HAS_OPA        0
    #define ILRC_HZ        53000    // Varies with voltage + temperature
	#define INSTR_CYCLES   2        // 2T architecture - DO NOT CHANGE
#endif


//=====================//
// PERIPHERAL SETTINGS //
//=====================//


//============//
// I2C MASTER //
//============//
#ifidni PERIPH_I2C_M, 1
    #define I2C_SDA     PA.6
    #define I2C_SCL     PA.7
    #define I2C_WR_CMD  0b0
    #define I2C_RD_CMD  0b1

    //      T_xxx    nS
    #define T_High   4700
    #define T_Low    4700
    #define T_Start  4700
    #define T_Stop   4700
    #define T_Buf    4700

    // Addresses
    #define    ST7032  0b0111110  // LCD Controller
    #define    M24C01  0b1010000  // EEPROM, STM device 0 (can have 8 on bus) 


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////

    // TIME TO CLOCK CONVERSION
    #define I2C_D_HIGH   T_High    ?  (((SYSTEM_CLOCK) / (1000000000 / T_High))  + 1) : 0
    #define I2C_D_LOW    T_Low     ?  (((SYSTEM_CLOCK) / (1000000000 / T_Low))   + 1) : 0
    #define I2C_D_START  T_Start   ?  (((SYSTEM_CLOCK) / (1000000000 / T_Start)) + 1) : 0
    #define I2C_D_STOP   T_Stop    ?  (((SYSTEM_CLOCK) / (1000000000 / T_Stop))  + 1) : 0
    #define I2C_D_BUF    T_Buf     ?  (((SYSTEM_CLOCK) / (1000000000 / T_Buf))   + 1) : 0




    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif


//==============//
// BUTTON INPUT //
//==============//
#ifidni PERIPH_BUTTON, 1
    #define BTN_WAKE_SYS   1

    // Debouncer timer selection
    #define BTN_TIMER      TM2
    #define BTN_TIMER_CTL  TM2C
    #define BTN_TIMER_CNT  TM2CT
    #define BTN_TIMER_BND  TM2B
    #define BTN_TIMER_SCL  TM2S
    #define BTN_TIMER_CLK  ILRC // Refer to datasheet and XXX.INC for options


    // Debouncer timing settings
    #define BTN_DEBOUNCE_T  5       // ms to wait before validating button state    
    #define BTN_TIMER_FREQ  ILRC_HZ // ILRC_HZ is defined in IC definition above
    #define BTN_TIMER_DIV   31      // [0 : 31] Clock divider
    //  # of timer cycles = (timer Hz / ( 2 x (clock divider + 1)) x (wait in ms / 1000));
    //  # of timer cycles MUST BE [1:255]
    //
    //  Example:
    //        # cycles  =  (60000 / 31 + 1) x (5 / 1000)  =  9 cycles


    // REVIEW IC DATASHEET FOR PIN COMPATABILITY!
    #define BTN_USE_PA    0        // Options: Disable bank: 0 / Enable bank: 1
    #define BTN_PA0       0        // Options: 0 / 1
    #define BTN_PA1       0        // Options: 0 / 1
    #define BTN_PA2       0        // Options: 0 / 1
    #define BTN_PA3       0        // Options: 0 / 1
    #define BTN_PA4       0        // Options: 0 / 1
    #define BTN_PA5       0        // Options: 0 / 1
    #define BTN_PA6       0        // Options: 0 / 1
    #define BTN_PA7       0        // Options: 0 / 1
    
	#define BTN_USE_PB    1        // Options: Disable bank: 0 / Enable bank: 1
    #define BTN_PB0       0        // Options: 0 / 1
    #define BTN_PB1       1        // Options: 0 / 1
    #define BTN_PB2       0        // Options: 0 / 1
    #define BTN_PB3       0        // Options: 0 / 1
    #define BTN_PB4       0        // Options: 0 / 1
    #define BTN_PB5       0        // Options: 0 / 1
    #define BTN_PB6       0        // Options: 0 / 1
    #define BTN_PB7       1        // Options: 0 / 1

    #define BTN_USE_PC    0        // Options: Disable bank: 0 / Enable bank: 1
    #define BTN_PC0       0        // Options: 0 / 1
    #define BTN_PC1       0        // Options: 0 / 1
    #define BTN_PC2       0        // Options: 0 / 1
    #define BTN_PC3       0        // Options: 0 / 1
    #define BTN_PC4       0        // Options: 0 / 1
    #define BTN_PC5       0        // Options: 0 / 1
    #define BTN_PC6       0        // Options: 0 / 1
    #define BTN_PC7       0        // Options: 0 / 1


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////

    #if BTN_USE_PA
        #define BTN_PA   ((BTN_PA7 << 7) | \
                         (BTN_PA6 << 6) | \
                         (BTN_PA5 << 5) | \
                         (BTN_PA4 << 4) | \
                         (BTN_PA3 << 3) | \
                         (BTN_PA2 << 2) | \
                         (BTN_PA1 << 1) | \
                         (BTN_PA0 << 0))

    #endif
    #if BTN_USE_PB
        #define BTN_PB   ((BTN_PB7 << 7) | \
                         (BTN_PB6 << 6) | \
                         (BTN_PB5 << 5) | \
                         (BTN_PB4 << 4) | \
                         (BTN_PB3 << 3) | \
                         (BTN_PB2 << 2) | \
                         (BTN_PB1 << 1) | \
                         (BTN_PB0 << 0))

    #endif
    #if BTN_USE_PC
        #define BTN_PC   ((BTN_PC7 << 7) | \
                         (BTN_PC6 << 6) | \
                         (BTN_PC5 << 5) | \
                         (BTN_PC4 << 4) | \
                         (BTN_PC3 << 3) | \
                         (BTN_PC2 << 2) | \
                         (BTN_PC1 << 1) | \
                         (BTN_PC0 << 0))
    #endif

    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif    


//===============//
// LCD INTERFACE //
//===============//
#ifidni PERIPH_LCD, 1
    #define LCD_COMM_MODE  I2C       // Parallel not yet developed
    #define LCD_DRIVER     ST7032    // Only ST7032 is validated
    #define LCD_VOLTAGE    5         // Only 5V is validated

    // LCD Constants
    #define LCD_WIDTH      16        // Number of chars per line
    #define LCD_HEIGHT     2         // Number of lines
    #define LCD_L1         0x00      // Line 1 address
    #define LCD_L2         0x40      // Line 2 address
    #define LCD_INIT_T     40000     // Initialization time, microseconds
    #define LCD_PWR_T      200000    // Power setting stabilization time, microseconds
    #define LCD_WAIT_T     30        // Instruction gap time, microseconds


    // Character Values, 8-bit
    #define LCD_A        0x41
    #define LCD_B        0x42
    #define LCD_C        0x43
    #define LCD_D        0x44
    #define LCD_E        0x45
    #define LCD_F        0x46
    #define LCD_G        0x47
    #define LCD_H        0x48    
    #define LCD_I        0x49
    #define LCD_J        0x4A
    #define LCD_K        0x4B
    #define LCD_L        0x4C
    #define LCD_M        0x4D
    #define LCD_N        0x4E
    #define LCD_O        0x4F
    #define LCD_P        0x50
    #define LCD_Q        0x51
    #define LCD_R        0x52
    #define LCD_S        0x53
    #define LCD_T        0x54
    #define LCD_U        0x55
    #define LCD_V        0x56
    #define LCD_W        0x57
    #define LCD_X        0x58
    #define LCD_Y        0x59
    #define LCD_Z        0x5A
    #define LCD_0        0x30
    #define LCD_1        0x31
    #define LCD_2        0x32
    #define LCD_3        0x33
    #define LCD_4        0x34
    #define LCD_5        0x35
    #define LCD_6        0x36
    #define LCD_7        0x37
    #define LCD_8        0x38
    #define LCD_9        0x39
    #define LCD_dot      0x2E
    #define LCD_lt       0x3C
    #define LCD_gt       0x3E
    #define LCD_eq       0x3D
    #define LCD_colon    0x3A
    #define LCD_space    0x20

    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////

    // DRIVER INSTRUCTION CODES
    #ifidni %LCD_DRIVER, %ST7032

        #define LCD_RAISE_CONTROL_B     0x80
        #define LCD_LOWER_CONTROL_B     0x00
        #define LCD_DATA_MODE           0x40
        #define LCD_COMMAND_MODE        0x00

        #define LCD_BUSY_MASK           0x80    // COMMAND MODE & READ
        #define LCD_ADDR_MASK           0x7F    // COMMAND MODE & READ

        #define LCD_CLEAR_F             0x01

        #define LCD_HOME_F              0x02

        #define LCD_ENTRY_F             0x04
        #define LCD_ENTRY_INC_DDRAM     0x02
        #define LCD_ENTRY_DEC_DDRAM     0x00
        #define LCD_ENTRY_DISP_SHIFT    0x01
        #define LCD_ENTRY_DDRAM_SHIF    0x00

        #define LCD_DISP_F              0x08
        #define LCD_DISP_ON             0x04
        #define LCD_DISP_OFF            0x00
        #define LCD_DISP_CURSOR_ON      0x02
        #define LCD_DISP_CURSOR_OFF     0x00
        #define LCD_DISP_BLINK_ON       0x01
        #define LCD_DISP_BLINK_OFF      0x00

        #define LCD_SHIFT_F             0x10    // Function set: NORMAL
        #define LCD_SHIFT_DISP_CTL      0x08    // Function set: NORMAL
        #define LCD_SHIFT_CURSOR_CTL    0x00    // Function set: NORMAL
        #define LCD_SHIFT_RIGHT         0x04    // Function set: NORMAL
        #define LCD_SHIFT_LEFT          0x00    // Function set: NORMAL

        #define LCD_FUNC_F              0x20
        #define LCD_FUNC_8BIT           0x10
        #define LCD_FUNC_4BIT           0x00
        #define LCD_FUNC_2L             0x08
        #define LCD_FUNC_1L             0x00
        #define LCD_FUNC_HEIGHT2X       0x04
        #define LCD_FUNC_HEIGHT1X       0x00
        #define LCD_FUNC_EXTENDED       0x01    // EXTENDED
        #define LCD_FUNC_NORMAL         0x00    // NORMAL

        #define LCD_SET_CGRAM_ADDR      0x40    // Function set: NORMAL
        #define LCD_SET_ICON_ADDR       0x40    // Function set: EXTENDED
        #define LCD_SET_DDRAM_ADDR      0x80

        #define LCD_BIAS_OSC_F          0x10    // Function set: EXTENDED
        #define LCD_BIAS_14             0x08    // Function set: EXTENDED
        #define LCD_BIAS_15             0x00    // Function set: EXTENDED
        #define LCD_OSC_F2              0x04    // Function set: EXTENDED
        #define LCD_OSC_F1              0x02    // Function set: EXTENDED
        #define LCD_OSC_f0              0x01    // Function set: EXTENDED

        #define LCD_PWR_ICON_CNTRSTH_F  0x50    // Function set: EXTENDED
        #define LCD_ICON_ON             0x08    // Function set: EXTENDED
        #define LCD_BOOST_ON            0x04    // Function set: EXTENDED
        #define LCD_CONTRASTH_C5        0x02    // Function set: EXTENDED
        #define LCD_CONTRASTH_C4        0x01    // Function set: EXTENDED

        #define LCD_CONTRASTL_F         0x70    // Function set: EXTENDED
        #define LCD_CONTRASTL_C3        0x08    // Function set: EXTENDED
        #define LCD_CONTRASTL_C2        0x04    // Function set: EXTENDED
        #define LCD_CONTRASTL_C1        0x02    // Function set: EXTENDED
        #define LCD_CONTRASTL_C0        0x01    // Function set: EXTENDED

        #define LCD_FOLLOWER_F          0x60    // Function set: EXTENDED
        #define LCD_FOLLOWER_ON         0x08    // Function set: EXTENDED
        #define LCD_FOLLOWER_RAB2       0x04    // Function set: EXTENDED
        #define LCD_FOLLOWER_RAB1       0x02    // Function set: EXTENDED
        #define LCD_FOLLOWER_RAB0       0x01    // Function set: EXTENDED

        #ifidni LCD_VOLTAGE, 5
			#define LCD_INIT_FUNC1             (LCD_FUNC_F | LCD_FUNC_2L | LCD_FUNC_NORMAL)
            #define LCD_INIT_FUNC2             (LCD_FUNC_F | LCD_FUNC_2L | LCD_FUNC_EXTENDED)
            #define LCD_INIT_BIAS_OSC          (LCD_BIAS_OSC_F | LCD_OSC_F2)
            #define LCD_INIT_PWR_ICON_CNTRSTH  (LCD_PWR_ICON_CNTRSTH_F)
            #define LCD_INIT_CONTRASTL         (LCD_CONTRASTL_F | LCD_CONTRASTL_C3 | LCD_CONTRASTL_C0)
            #define LCD_INIT_FOLLOWER          (LCD_FOLLOWER_F | LCD_FOLLOWER_ON | LCD_FOLLOWER_RAB2)
        #endif

    #endif


    // TIME TO CLOCK CONVERSION
    #define LCD_INIT_D   LCD_INIT_T    ?  (SYSTEM_CLOCK / (1000000 / LCD_INIT_T) / 2 + 1) : 0
    #define LCD_PWR_D    LCD_PWR_T     ?  (SYSTEM_CLOCK / (1000000 / LCD_PWR_T)  / 2 + 1) : 0
    #define LCD_WAIT_D   LCD_WAIT_T    ?  (SYSTEM_CLOCK / (1000000 / LCD_WAIT_T) / 2 + 1) : 0


    // INTERFACE COMPATABILITY WARNING
    #ifidni LCD_COMM_MODE, I2C
        #ifz PERIPH_I2C_M
            .error LCD with I2C Comm Mode REQUIRES PERIPH_I2C to be enabled! 
        #endif
        #define LCD_2L_SETTINGS (LCD_FUNC_F | LCD_FUNC_2L | LCD_FUNC_HEIGHT1X)
        #define LCD_1L_SETTINGS (LCD_FUNC_F | LCD_FUNC_1L | LCD_FUNC_HEIGHT2X)
    #endif

    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif


//==================//
// EEPROM INTERFACE //
//==================//

#ifidni PERIPH_EEPROM, 1
    #define EEPROM_COMM_MODE    I2C       // 
    #define EEPROM_DRIVER       M24C01    //  
    #define EEPROM_WRITE_CTL    NONE      // Pin on ~WC (ie PA.7)
    #define EEPROM_PAGE_SIZE    16        // Page size in bytes
    #define EEPROM_MEM_SIZE     128       // Memory size in bytes


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////
    #ifidni EEPROM_COMM_MODE, I2C
        #ifz PERIPH_I2C_M
            .error EEPROM with I2C Comm Mode REQUIRES PERIPH_I2C to be enabled! 
        #endif
    #endif

    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif


//==========//
// 8b TIMER //
//==========//

#ifidni PERIPH_TIMER8, 1

	#define TIMER8_USE_TM2       1
	#define TIMER8_USE_TM3       1
	#define TIMER8_SOLVER_ENABLE 1  // CHECK HEADER FOR RESOURCE USAGE!


	// TIMER 2
	#define TIMER8_2_CLK    ILRC    // ILRC, SYSCLK, other 
	#define TIMER8_2_HZ     ILRC_HZ //ILRC_HZ, SYSTEM_CLOCK, other. ICE_ILRC_HZ for TESTING ONLY
	#define TIMER8_2_MODE   Period  // Period, PWM
	#define TIMER8_2_OUT    PB2     // Ex: Disable, PB2, PA3, PB4
	#define TIMER8_2_6BIT   0       // 0: 8-bit PWM;           1: 6-bit PWM
	#define TIMER8_2_INV    0       // 0: Out polarity normal; 1: Out polarity inverted


	// TIMER 3
	#define TIMER8_3_CLK    ILRC
	#define TIMER8_3_HZ     ICE_ILRC_HZ
	#define TIMER8_3_MODE   PWM
	#define TIMER8_3_OUT    PB6
	#define TIMER8_3_6BIT   1
	#define TIMER8_3_INV    1


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////

	#define TIMER8_2_CTL    TM2C
	#define TIMER8_2_CNT    TM2CT
	#define TIMER8_2_SCL    TM2S
	#define TIMER8_2_BND    TM2B

	#define TIMER8_3_CTL    TM3C
	#define TIMER8_3_CNT    TM3CT
	#define TIMER8_3_SCL    TM3S
	#define TIMER8_3_BND    TM3B


	#if TIMER8_2_6BIT
		#define TIMER8_2_RES 0b10000000
	#else
		#define TIMER8_2_RES 0b00000000
	#endif


	#if TIMER8_2_INV
		#define TIMER8_2_POL Inverse
	#else
		#define TIMER8_2_POL
	#endif


	#if TIMER8_3_6BIT
		#define TIMER8_3_RES 0b10000000
	#else
		#define TIMER8_3_RES 0b00000000
	#endif

	#if TIMER8_3_INV
		#define TIMER8_3_POL Inverse
	#else
		#define TIMER8_3_POL
	#endif


	/////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif


//=========//
// 11b PWM //
//=========//

#ifidni PERIPH_PWM_11B, 1

	#define PWM_USE_G0 1
	#define PWM_USE_G1 1
	#define PWM_USE_G2 1
	#define PWM_SOLVER_ENABLE 1  // CHECK HEADER FOR RESOURCE USAGE!


	// PWM 0
    #define PWM_0_CLOCK     SYSCLK       // SYSCLK, IHRC, IHRC*2
	#define PWM_0_CLK_HZ    SYSTEM_CLOCK // Clock frequency
	#define PWM_0_OUTPUT    PB4          // Disable, PB5, PA0, PB4
	#define PWM_0_INV       0            // Invert PWM output, 0/1
	#define PWM_0_INT_ZERO  0            // Interrupt at zero or duty, 0/1


	// PWM 1
    #define PWM_1_CLOCK     SYSCLK
	#define PWM_1_CLK_HZ    SYSTEM_CLOCK
	#define PWM_1_OUTPUT    PA4
	#define PWM_1_INV       0
	#define PWM_1_INT_ZERO  0            // INTERUPT ONLY SUPPORTED ON G0


	// PWM 2
    #define PWM_2_CLOCK     SYSCLK
	#define PWM_2_CLK_HZ    SYSTEM_CLOCK
	#define PWM_2_OUTPUT    PB3
	#define PWM_2_INV       1
	#define PWM_2_INT_ZERO  0            // INTERUPT ONLY SUPPORTED ON G0


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////

    #define PWM_RESET      Reset
    #define PWM_ENABLE     Enable

	// PWM 0
	#define PWM_0_CTL      PWMG0C
    #define PWM_0_SCALAR   PWMG0S
    #define PWM_0_COUNT_H  PWMG0CUBH
    #define PWM_0_COUNT_L  PWMG0CUBL
    #define PWM_0_DUTY_H   PWMG0DTH
    #define PWM_0_DUTY_L   PWMG0DTL

	// PWM 1
	#define PWM_1_CTL      PWMG1C
    #define PWM_1_SCALAR   PWMG1S
    #define PWM_1_COUNT_H  PWMG1CUBH
    #define PWM_1_COUNT_L  PWMG1CUBL
    #define PWM_1_DUTY_H   PWMG1DTH
    #define PWM_1_DUTY_L   PWMG1DTL

	// PWM 2
	#define PWM_2_CTL      PWMG2C
    #define PWM_2_SCALAR   PWMG2S
    #define PWM_2_COUNT_H  PWMG2CUBH
    #define PWM_2_COUNT_L  PWMG2CUBL
    #define PWM_2_DUTY_H   PWMG2DTH
    #define PWM_2_DUTY_L   PWMG2DTL


	#if PWM_0_INV
		#define PWM_0_POL Inverse
	#else
		#define PWM_0_POL
	#endif

	#if PWM_0_INT_ZERO
		#define PWM_0_INT 0b10000000
	#else
		#define PWM_0_INT 0b00000000
	#endif


	#if PWM_1_INV
		#define PWM_1_POL Inverse
	#else
		#define PWM_1_POL
	#endif

	#if PWM_1_INT_ZERO
		#define PWM_1_INT 0b10000000
	#else
		#define PWM_1_INT 0b00000000
	#endif


	#if PWM_2_INV
		#define PWM_2_POL Inverse
	#else
		#define PWM_2_POL
	#endif

	#if PWM_2_INT_ZERO
		#define PWM_2_INT 0b10000000
	#else
		#define PWM_2_INT 0b00000000
	#endif

    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif



//===================//
// STEPPER INTERFACE //
//===================//

#ifidni PERIPH_STEPPER, 1
    #define EEPROM_COMM_MODE    I2C       // 
    #define EEPROM_DRIVER       M24C01    //  
    #define EEPROM_WRITE_CTL    NONE      // Pin on ~WC (ie PA.7)
    #define EEPROM_PAGE_SIZE    16        // Page size in bytes
    #define EEPROM_MEM_SIZE     128       // Memory size in bytes


    ///////////////////////////
    // DO NOT TOUCH -- START //
    ///////////////////////////
    #ifdifi PERIPH_PWM_11B, 1
        .error PERIPH_STEPPER REQUIRES PERIPH_PWM_11B to be enabled! 
    #endif

    /////////////////////////
    // DO NOT TOUCH -- END //
    /////////////////////////
#endif


#endif // SYSTEM_SETTINGS_H