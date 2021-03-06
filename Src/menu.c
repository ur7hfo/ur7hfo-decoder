/*
 * menu.c
 *
 *  Created on: 31 ��� 2018 �.
 *      Author: dima (UR7HFO)
 */

#include "banddecoder.h"
#include "menu.h"
#include "lcd.h"
#include "ssd1306.h"
#include "MicroMenu.h"
#include "eeprom24xx.h"

/* Menu button ports */
static GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {
BUT_UP_GPIO_Port,
BUT_DOWN_GPIO_Port,
BUT_LEFT_GPIO_Port,
BUT_RIGHT_GPIO_Port,
BUT_SELECT_GPIO_Port };

/* Menu button pins */
static const uint16_t BUTTON_PIN[BUTTONn] = {
BUT_UP_Pin,
BUT_DOWN_Pin,
BUT_LEFT_Pin,
BUT_RIGHT_Pin,
BUT_SELECT_Pin };

/* ---------------------------------------------------------------------- */
static bool flagButton[BUTTONn] = { };
static Button_TypeDef key_pressed = BUTTON_NOTHING;
static bool flagPressed = false;
static const char selected[] = "SELECTED";

/* ---------------------------------------------------------------------- */
extern uint32_t cat_speed;
extern int moode;
extern uint8_t civ_out;
extern int request;

/* Menus  Name   | Next  | Prev  | Parent | Child | SelectFun | EnterFun | Text */
MENU_ITEM(Menu_1, Menu_2, Menu_1, NULL_MENU, Menu_1_1, Menu_1_SelectCallback,
		NULL, "CAT SPEED");
MENU_ITEM(Menu_2, NULL_MENU, Menu_1, NULL_MENU, Menu_2_1, Menu_2_SelectCallback,
		NULL, "SEND REQUEST");

/* Speed submenu */
MENU_ITEM(Menu_1_1, Menu_1_2, Menu_1_5, Menu_1, NULL_MENU, NULL,
		Menu_1_1_EnterCallback, "9600");
MENU_ITEM(Menu_1_2, Menu_1_3, Menu_1_1, Menu_1, NULL_MENU, NULL,
		Menu_1_2_EnterCallback, "19200");
MENU_ITEM(Menu_1_3, Menu_1_4, Menu_1_2, Menu_1, NULL_MENU, NULL,
		Menu_1_3_EnterCallback, "38400");
MENU_ITEM(Menu_1_4, Menu_1_5, Menu_1_4, Menu_1, NULL_MENU, NULL,
		Menu_1_4_EnterCallback, "57600");
MENU_ITEM(Menu_1_5, Menu_1_1, Menu_1_4, Menu_1, NULL_MENU, NULL,
		Menu_1_5_EnterCallback, "115200");

/* Request submenu */
MENU_ITEM(Menu_2_1, Menu_2_2, NULL_MENU, Menu_1, NULL_MENU, NULL,
		Menu_2_1_EnterCallback, "ON");
MENU_ITEM(Menu_2_2, NULL_MENU, Menu_2_1, Menu_1, NULL_MENU, NULL,
		Menu_2_2_EnterCallback, "OFF");

/* ---------------------------------------------------------------------- */
uint8_t getPressKey(void) {
	for (uint8_t i = 0; i < BUTTONn; i++) {
		if (HAL_GPIO_ReadPin(BUTTON_PORT[i], BUTTON_PIN[i]) && !flagButton[i]) {
			flagButton[i] = true;
			key_pressed = (Button_TypeDef) i;
		}

		if (!HAL_GPIO_ReadPin(BUTTON_PORT[i], BUTTON_PIN[i]) && flagButton[i]) {
			flagButton[i] = false;
			key_pressed = BUTTON_NOTHING;
		}
	}
	return (key_pressed);
}

/* ----------------------------------------------------------------------------- */
void Generic_Write(const char* Text) {
	if (Text) {
#ifdef LCD16X2
		LCD_Clear();
		LCD_String((char *)Text);
#endif
#ifdef SSD1306
		ssd1306_Fill(Black);
		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString((char *) Text, Font_7x10, White);
		ssd1306_UpdateScreen();
#endif
	}
}

/* ----------------------------------------------------------------------------- */
void MenuInit(void) {
	Menu_SetGenericWriteCallback(Generic_Write);
	Menu_Navigate(&Menu_1);
}

/* ----------------------------------------------------------------------------- */
void MenuProcess(void) {
	if (getPressKey() != BUTTON_NOTHING && !flagPressed) {
		flagPressed = true;
		switch (getPressKey()) {
		case BUTTON_LEFT:
			Menu_Navigate(MENU_PARENT);
			break;
		case BUTTON_UP:
			Menu_Navigate(MENU_PREVIOUS);
			break;
		case BUTTON_DOWN:
			Menu_Navigate(MENU_NEXT);
			break;
		case BUTTON_RIGHT:
			Menu_Navigate(MENU_CHILD);
			break;
		case BUTTON_SELECT:
			Menu_EnterCurrentItem();
			break;
		default:
			break;
		}
	} else if (getPressKey() == BUTTON_NOTHING && flagPressed) {
		flagPressed = false;
	}
}

/* ------------------------------------------------------------------------------- */
void Menu_1_SelectCallback(void) {
	char buf[15] = { 0 };

	sprintf(buf, "%ld", cat_speed);

#ifdef LCD16X2
	LCD_SetPos(0,1);
	LCD_String(buf);
#endif
#ifdef SSD1306
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString(buf, Font_7x10, White);
	ssd1306_UpdateScreen();
#endif
}

/* -------------------------------------------------------------------------------- */
void Menu_2_SelectCallback(void) {
	if (request == true) {
#ifdef LCD16X2
		LCD_SetPos(0,1);
		LCD_String("ON");
#endif
#ifdef SSD1306
		ssd1306_SetCursor(0, 16);
		ssd1306_WriteString("ON", Font_7x10, White);
		ssd1306_UpdateScreen();
#endif
	} else {
#ifdef LCD16X2
		LCD_SetPos(0,1);
		LCD_String("OFF");
#endif
#ifdef SSD1306
		ssd1306_SetCursor(0, 16);
		ssd1306_WriteString("OFF", Font_7x10, White);
		ssd1306_UpdateScreen();
#endif
	}
}

/* --------------------------------------------------------------------------------- */
void Menu_1_1_EnterCallback(void) {
	cat_speed = 9600;
	EEPROM24XX_Save((uint16_t) ADDR_SPEED, &cat_speed, sizeof(cat_speed));

	char str[] = "9600";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

void Menu_1_2_EnterCallback(void) {
	cat_speed = 19200;
	EEPROM24XX_Save((uint16_t) ADDR_SPEED, &cat_speed, sizeof(cat_speed));

	char str[] = "19200";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

void Menu_1_3_EnterCallback(void) {
	cat_speed = 38400;
	EEPROM24XX_Save((uint16_t) ADDR_SPEED, &cat_speed, sizeof(cat_speed));

	char str[] = "38400";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

void Menu_1_4_EnterCallback(void) {
	cat_speed = 57600;
	EEPROM24XX_Save((uint16_t) ADDR_SPEED, &cat_speed, sizeof(cat_speed));

	char str[] = "57600";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

void Menu_1_5_EnterCallback(void) {
	cat_speed = 115200;
	EEPROM24XX_Save((uint16_t) ADDR_SPEED, &cat_speed, sizeof(cat_speed));

	char str[] = "115200";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

/* --------------------------------------------------------------------------------------------------------------- */
void Menu_2_1_EnterCallback(void) {
	request = TRUE;
	EEPROM24XX_Save((uint16_t) ADDR_REQUEST, &request, sizeof(request));

	char str[] = "ON";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

void Menu_2_2_EnterCallback(void) {
	request = FALSE;
	EEPROM24XX_Save((uint16_t) ADDR_REQUEST, &request, sizeof(request));

	char str[] = "OFF";

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(str);
	LCD_SetPos(0,1);
	LCD_String((char *)selected);
	HAL_Delay(2000);
	LCD_Clear();
#endif
#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(str, Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString((char *) selected, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

	Menu_Navigate(&Menu_1);
}

