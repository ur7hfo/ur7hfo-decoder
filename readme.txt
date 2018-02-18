/*
 * =======================================================   
 * Pins for Arduino MEGA 2660:     
 * 
 * LCD                       - I2C 20(SDA), 21(SCL)   
 * TRX CAT DATA              - SERIAL_CAT_PORT TX(14), RX(15)  
 * FOR K3NG WINKEY2          - Serial (usb on board)
 * KENWOOD CAT OUT           - Serial1 TX(19), RX(18) 
 * ICOM IC-PW1 AMP CAT OUT   - Serial2 TX(16), RX(17)   
 *
 * 160    band   - 22 pin
 * 80     band   - 23 pin
 * 40     band   - 24 pin
 * 30     band   - 25 pin
 * 20     band   - 26 pin
 * 17     band   - 27 pin
 * 15     band   - 28 pin
 * 12     band   - 30 pin
 * 10     band   - 31 pin
 * 6      band   - 32 pin
 * 2      band   - 33 pin 
 * 0.7    band   - 34 pin 
 * SPLIT relay   - 35 pin
 *
 *
 * PTT INPUT     -  2     // PTT Input (Low level PTT => TX)  
 *
 * SEQUENCER PTT_OUT_TRX   4 
 * SEQUENCER PTT_OUT_AMP   5
 * SEQUENCER PTT_OUT_COAX  6   
 *
 * TFT DISPLAY PINS:
 *
 * ST7735 1.8 TFT_CS   - 47 pin 
 * ST7735 1.8 TFT_DC   - 48 pin
 * ST7735 1.8 TFT_RST  - 49 pin
 * SPI to TFT DISPLAY  - 51 (MOSI)(4-DIN), 52 (SCK)(5-CLK)
 * SPI not used        - 50 (MISO), 53 (SS)
 * 
 * FST3253 mux:
 *
 * FST3253_S0    - 8 pin (bus B1 - civ, bus B2 - if232, bus B3 - rs232, B4 - not used)    
 * FST3253_S1    - 9 pin 
 *
 * BCD data out pins:
 *  
 * BCD_A_OUT - 10     
 * BCD_B_OUT - 11     
 * BCD_C_OUT - 12    
 * BCD_D_OUT - 13    
 *
 * BCD data in pins:
 *
 * BCD_A_IN  - A0 
 * BCD_B_IN  - A1
 * BCD_C_IN  - A2
 * BCD_D_IN  - A3
 *
 * Encoder pins:
 *
 * Rotator pin1     - A14
 * Rotator pin2     - A15
 * Rotator switch   - A5  
 *  
 * 
 * KEYER : 
 *
 * KEY COMMAND BUTTON        -  A5 
 * KEY COMMAND BUTTON LED    -  A6 
 * KEY PTT1                  -  A7 
 * KEY PTT2                  -  A8 
 * KEY TX1                   -  A9 
 * KEY TX2                   -  A10 
 * KEY PADDLE LEFT           -  A11 
 * KEY PADDLE RIGHT          -  A12 
 * KEY SIDETONE              -  A13
 * 
 * -----------   -  A4 
 * -----------   -  3  
 * -----------   -  29 
 * -----------   -  36
 * -----------   -  43 
 * -----------   -  44 
 * -----------   -  45 
 * -----------   -  46 
 *          
*/ 