/*
 * hd44780.cpp
 *
 * Created: 05.02.2021 15:57:08
 *  Author: Neb Konstantin Viktorovich
 */ 
#include "Headers/globals.h"
#include "Headers/hd44780.h"

const uint8_t otherSymbol[8][8] PROGMEM = {
  {0x04, 0x0A, 0x12, 0x16, 0x1E, 0x1E, 0x1E, 0x00}, // icon solder
  {0x10, 0x18, 0x1c, 0x1a, 0x09, 0x05, 0x02, 0x00}, // icon soldering up
  {0x08, 0x14, 0x0b, 0x04, 0x04, 0x04, 0x03, 0x00}, // icon degree celsius
  {0x06, 0x06, 0x14, 0x1b, 0x05, 0x0c, 0x0c, 0x00}, // icon fan
  {0x00, 0x0c, 0x13, 0x0c, 0x13, 0x0c, 0x13, 0x00}, // icon air
  {0x0e, 0x11, 0x11, 0x0e, 0x00, 0x0e, 0x1f, 0x00}, // icon thermofan on stand
  {0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00}, // icon menu
  {0x02, 0x05, 0x09, 0x1a, 0x1c, 0x18, 0x10, 0x00}, // icon soldering down
};

void HD44780_init() {
  PORT_LCD_D |= 0x30;
  HD44780_switchE();
  _delay_ms(5);
  HD44780_switchE();
  _delay_us(100);
  HD44780_witchE();
  _delay_ms(10);
  PORT_LCD_D &= 0x0f;
  PORT_LCD_D |= 0x20;
  HD44780_switchE(); 
  sendCommand(0x28);
  sendCommand(0x0c);
  sendCommand(0x06);
  sendCommand(0x40);
  _delay_ms(1);
  for (uint8_t j = 0; j < 8; j ++) {
    for (uint8_t i = 0; i < 8; i ++) {
      HD44780_sendChar(pgm_read_byte(&otherSymbol[j][i]));
      _delay_ms(1);
    }
  }
  HD44780_clear();  
}

void HD44780_switchE() {
  _delay_us(20);
  PORT_LCD_E |= PIN_LCD_E;
  _delay_us(10);
  PORT_LCD_E &= ~PIN_LCD_E;
  _delay_us(20);
}

void HD44780_send(bool isCommand, uint8_t data) {
  if (isCommand == false) {
    PORT_LCD_RS |= PIN_LCD_RS;
  } else {
    PORT_LCD_RS &= ~PIN_LCD_RS;
  }
  PORT_LCD_D &= 0x0f; 
  PORT_LCD_D |= (data & 0xf0);
  HD44780_switchE();
  PORT_LCD_D &= 0x0f;
  PORT_LCD_D |= (data << 4);
  HD44780_switchE();  
}

void HD44780_sendCommand(uint8_t cmd) {
  HD44780_send(true, cmd);  
}

void HD44780_sendChar(const char chr) {
  HD44780_send(false, chr);
}

void HD44780_sendString(const char* str) {
  while (*str != 0) {
    HD44780_sendChar(*str);
    str += 1;
  }
}

void HD44780_clear() {
  HD44780_sendCommand(0x01);
  _delay_ms(2);
}

void HD44780_goTo(uint8_t x, uint8_t y) {
  uint8_t addr = 0x80 + 0x40*y + x;
  HD44780_sendCommand(addr);
}

void HD44780_sendStringFlash(const char* str) {
  while (pgm_read_byte(str) != 0) {
    HD44780_sendChar(pgm_read_byte(str));
    str ++;
  }
}