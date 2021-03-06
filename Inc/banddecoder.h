/*
 * Pin maps, and other. 
 *
 * UART1 - TRX KENWOOD      (PA10->RX, PA9->TX)
 * UART2 -                  (PA3->RX (real logger), PA2->TX (kenwood output ))
 * UART3 - ICOM EMULATOR    (PB11->RX, PB10->TX)
 *
 * B7 - I2C (SDA) LCD16x2 DISPLAY (address 0x3f) 
 * B6 - I2C (SCL) LCD16x2 DISPLAY (address 0x3f)
 * 
 * A0 - 160m
 * A1 - 80m
 * A4 - 40m
 * A5 - 30m
 * A6 - 20m
 * A7 - 17m
 * B0 - 15m
 * B1 - 12m
 * B12 - 10m
 * B13 - 6m
 *
 * B14 - 
 * B15 -
 * B2  - PTT INPUT
 *
 * A8  - SPLIT RELAY ON/OFF 
 *
 * B8  - BUTTON SELECT
 * B5  - BUTTON UP
 * B4  - BUTTON DOWN
 * B3  - BUTTON LEFT
 * A15 - BUTTON RIGHT
 *
 * A11 - 
 * A12 - 
 *
 * C15 - bcd a out
 * C14 - bcd b out
 * C13 - bcd c out
 * B9  - bcd d out
 *
 *
*/

#ifndef BANDDECODER_H_
#define BANDDECODER_H_

	#include "stm32f1xx_hal.h"

	#include <stdbool.h>
	#include <string.h>
	#include <stdlib.h>

	#include "lcd.h"
	#include "ssd1306.h"

	/* #define EEPROM_YES */
	#define SSD1306
	/* #define LCD16X2 */

	#define LCD_16X2_ADDR    0x3F
	#define POLLING_INTERVAL 100
	#define CAT_TX_INTERVAL  5000

	#define CIVREQUESTLEN 6
	#define KENWOODREQUESTLEN 3
	#define KENWOODCATLEN 64

	/* Icom address out */
	#define OUT_CIV_ADDRESS 0x88

	#define TRUE  1
	#define FALSE 0

	#define MOODE_KENWOOD_RS232       40
	#define MOODE_YAESU               50

	/* mem addreses to to store. */
	#define ADDR_MOODE        10
	#define ADDR_SPEED        20
	#define ADDR_CAT_TIMEOUT  30
	#define ADDR_REQUEST      40
	#define ADDR_CIV_ADDR     50
	#define ADDR_I2C_ADDR     60
	#define ADDR_CIV_ADDR_OUT 70

	/* split freq. */
	#define ADDR_SPLIT160     100
	#define ADDR_SPLIT80      110
	#define ADDR_SPLIT40      120
	#define ADDR_SPLIT30      130
	#define ADDR_SPLIT20      140
	#define ADDR_SPLIT17      150
	#define ADDR_SPLIT15      160
	#define ADDR_SPLIT12      170
	#define ADDR_SPLIT10      180
	#define ADDR_SPLIT6       190
	#define ADDR_SPLIT2       200
	#define ADDR_SPLIT07      210
  
	/* addr from 400 to 700, for band plan.. */
	#define ADDR_bp_160_low		400
	#define ADDR_bp_160_high	410
	#define ADDR_bp_80_low		420
	#define ADDR_bp_80_high		430
	#define ADDR_bp_40_low		440
	#define ADDR_bp_40_high		450
	#define ADDR_bp_30_low		460
	#define ADDR_bp_30_high		470
	#define ADDR_bp_20_low		480
	#define ADDR_bp_20_high		490
	#define ADDR_bp_17_low		500
	#define ADDR_bp_17_high		510
	#define ADDR_bp_15_low		520
	#define ADDR_bp_15_high		530
	#define ADDR_bp_12_low		540
	#define ADDR_bp_12_high		550
	#define ADDR_bp_10_low		560
	#define ADDR_bp_10_high		570
	#define ADDR_bp_6_low		580
	#define ADDR_bp_6_high		590
	#define ADDR_bp_2_low		600
	#define ADDR_bp_2_high		610
	#define ADDR_bp_07_low		620
	#define ADDR_bp_07_high		630


	void WriteBand(uint8_t b);
	bool WriteSplit(uint8_t b, uint32_t freq);
	void bcdOut(uint8_t band);
	uint8_t GetBand(uint32_t freq);
	void DisplayDraw(uint8_t band, uint32_t freq);

	void IcomCivEmulator(uint32_t freq);
	void IcomOutToPort(uint8_t command, long freq, uint8_t address);
	void IcomOutToPortMod(uint8_t cmd, uint8_t address, uint8_t mod);
	void IcomOutToPortOK(uint8_t address);
	void IcomOutToPortNG(uint8_t address);

	void KenwoodRequest(void);
	void KenwoodOutToPort(uint32_t freq);

	bool CheckLoggerActive(uint8_t *catBuffer);
	void FlushCatBuffers(uint8_t *buf1, uint8_t *buf2);
	void DisplayText(char *text);
	uint32_t readKenwoodCat(uint8_t *catBuffer);

#endif




