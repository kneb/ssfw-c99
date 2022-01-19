/*
 * hd44780.h
 *
 * Created: 05.02.2021 15:57:08
 *  Author: Neb Konstantin Viktorovich
 */ 

#ifndef HD44780_H_
#define HD44780_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

#define LCD_CUR_UL 0x0e
#define LCD_CUR_BS 0x0d
#define LCD_CUR_OFF 0x0c

void hd44780_init();
void hd44780_send(bool isCommand, uint8_t data);
void hd44780_switchE();
void hd44780_sendCommand(uint8_t cmd);
void hd44780_sendChar(const char chr);
void hd44780_sendString(const char* str);
void hd44780_sendStringFlash(const char* str);
void hd44780_clear();
void hd44780_goTo(uint8_t x, uint8_t y);

#endif /* HD44780_H_ */