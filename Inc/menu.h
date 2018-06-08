/* menu def. */

#ifndef MENU_H_
#define MENU_H_

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define BUTTONn 5

typedef enum
{
  BUTTON_UP   = 0,
  BUTTON_DOWN = 1,
  BUTTON_LEFT = 2,
  BUTTON_RIGHT = 3,
  BUTTON_SELECT  = 4,
  BUTTON_NOTHING  = 255
} Button_TypeDef;

uint8_t getPressKey(void);
void Generic_Write(const char* );
void MenuInit(void);
void MenuProcess(void);

void Menu_1_SelectCallback(void);
void Menu_2_SelectCallback(void);

void Menu_1_1_EnterCallback(void);
void Menu_1_2_EnterCallback(void);
void Menu_1_3_EnterCallback(void);
void Menu_1_4_EnterCallback(void);
void Menu_1_5_EnterCallback(void);

void Menu_2_1_EnterCallback(void);
void Menu_2_2_EnterCallback(void);

#endif /* MENU_H_ */
