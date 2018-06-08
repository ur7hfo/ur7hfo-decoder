#ifndef	_EEPROM_24XX_H
#define	_EEPROM_24XX_H

#include <stdbool.h>
#include "EEPROM24xxConfig.h"

bool		EEPROM24XX_IsConnected(void);
bool		EEPROM24XX_Save(uint16_t Address,void *data,size_t size_of_data);	
bool		EEPROM24XX_Load(uint16_t Address,void *data,size_t size_of_data);


#endif
