/*
 * lcd.cpp
 *
 * Created: 07.04.2021 15:33:48
 *  Author: Neb Konstantin Viktorovich
 */ 

#include "Headers/globals.h"
#include "Headers/lcd.h"

stMenu_t lcdMenu;

const uint8_t curType[4] PROGMEM = {CURSOR_LEFT_ARROW, CURSOR_RIGHT_ARROW,
                            CURSOR_LEFT_ANGLE, CURSOR_RIGHT_ANGLE};

const uint16_t curPosDashboard[6] PROGMEM = {
  0x5100, 0x5000, 0xA000, 0xB001, 0xB101, 0xA100
};
const uint16_t curPosRootMenu[5] PROGMEM = {
  0x6100, 0x6000, 0xD000, 0xE001, 0xB100
};
const uint16_t curPosCalibration[6] PROGMEM = {
  0x4100, 0x4000, 0xE001, 0xE101, 0x915E, 0x9101
};
const uint16_t curPosPIDMenu[8] PROGMEM = {
  0x4100, //solder kP
  0x4000, //tf kP
  0x9000, //tf kI
  0xE000, //tf kD
  0xE001, //exit
  0xE101, //save
  0xE100, //solder kD
  0x9100 //solder kI
};

void lcd_init() {
  lcdMenu.isEdit = 0;
  lcdMenu.param = 1;
  lcdMenu.level = 255;
}

void lcd_printMain() {
  cli();
  hd44780_clear();
  hd44780_goTo(0, 0);
  hd44780_sendStringFlash(PSTR("\x04"));
  lcd_printInt(1, 0, thermoFan.PID.currentTemp, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  hd44780_goTo(6, 0);
  hd44780_sendStringFlash(PSTR("\x03"));
  lcd_printInt(7, 0, thermoFan.fan, 3, false);
  hd44780_sendStringFlash(PSTR("%"));
  lcd_printInt(12, 0, thermoFan.PID.temp, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  hd44780_goTo(0, 1);
  hd44780_sendChar(0);
  lcd_printInt(1, 1, solder.PID.currentTemp, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  hd44780_goTo(9, 1);
  hd44780_sendStringFlash(PSTR("\x06"));
  lcd_printInt(12, 1, solder.PID.temp, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  lcd_printMenuCursor(CURSOR_TYPE_ARROW);
  sei();
} 

void lcd_printMenu() {
  cli();
  hd44780_clear();
  hd44780_goTo(0, 0);
  hd44780_sendStringFlash(PSTR("cal.TF   Save  x"));
  hd44780_goTo(0, 1);
  hd44780_sendStringFlash(PSTR("cal.Sl  PID"));
  lcd_printMenuCursor(CURSOR_TYPE_ARROW);
  sei();
}

void lcd_printPIDMenu() {
  cli();
  hd44780_clear();
  hd44780_goTo(0, 0);
  hd44780_sendStringFlash(PSTR("\x04"));
  lcd_printInt(1, 0, 
      pid_getMultiplier(&thermoFan.PID, PID_KP), 3, true);
  hd44780_sendStringFlash(PSTR(" I"));
  lcd_printInt(6, 0, 
      pid_getMultiplier(&thermoFan.PID, PID_KI), 3, true);
  hd44780_sendStringFlash(PSTR(" D"));
  lcd_printInt(11, 0, 
      pid_getMultiplier(&thermoFan.PID, PID_KD), 3, true);
  hd44780_sendStringFlash(PSTR(" x"));

  hd44780_goTo(0, 1);
  hd44780_sendChar(0);
  lcd_printInt(1, 1, 
      pid_getMultiplier(&solder.PID, PID_KP), 3, true);
  hd44780_sendStringFlash(PSTR(" I"));
  lcd_printInt(6, 1, 
      pid_getMultiplier(&solder.PID, PID_KI), 3, true);
  hd44780_sendStringFlash(PSTR(" D"));
  lcd_printInt(11, 1, 
      pid_getMultiplier(&solder.PID, PID_KD), 3, true);
  hd44780_sendStringFlash(PSTR(" s"));

  lcd_printMenuCursor(CURSOR_TYPE_ARROW);
  sei();
}

void lcd_printCalibration(uint8_t calibrationMenu) {
  stPID_t *pid = NULL;
  if (calibrationMenu == CALIBRATION_THERMOFAN) {
    pid = &thermoFan.PID;   
  } else {
    pid = &solder.PID;
  }
  pid_setPower(pid, (uint8_t)20);
  uint16_t temp = pid->currentTemp;
  uint16_t temp1 = pid->refTemp1;
  uint16_t temp2 = pid->refTemp2;
  uint16_t adc1 = pid->refAdc1;
  uint16_t adc2 = pid->refAdc2;

  cli();
  hd44780_clear();
  lcd_printInt(0, 0, temp1, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  lcd_printInt(5, 0, adc1, 4, true);
  hd44780_sendChar(calibrationMenu);
  lcd_printInt(10, 0, temp, 3, true);
  hd44780_sendStringFlash(PSTR("\x02 x"));
  lcd_printInt(0, 1, temp2, 3, true);
  hd44780_sendStringFlash(PSTR("\x02"));
  lcd_printInt(5, 1, adc2, 4, true);
  lcd_printInt(10, 1, 20, 3, true);
  hd44780_sendStringFlash(PSTR("% s"));
  lcd_printMenuCursor(CURSOR_TYPE_ARROW);
  sei();
}

void lcd_printInt(uint8_t x, uint8_t y, uint16_t source, uint8_t len, 
                   bool zero) {
  char buf[5];
  hd44780_goTo(x, y);
  itoa(buf, source, len, zero);
  hd44780_sendString(buf);  
}

void lcd_printLogo() {
  hd44780_goTo(1, 0);
  hd44780_sendStringFlash(PSTR("Soldering"));
  hd44780_goTo(3, 1);
  hd44780_sendStringFlash(PSTR("Station 1.0"));
}

void lcd_printIconsStatus() {
  hd44780_goTo(6, 1);
  if ((PORT_VIBRO & VIBRO) == 0) {
    hd44780_sendStringFlash(PSTR("\x07"));
  } else {
    hd44780_sendStringFlash(PSTR("\x01"));
  }
  if ((PORT_GERKON & GERKON) == 0) { //ThermoFan on stand
    hd44780_sendStringFlash(PSTR("\x05"));
  } else {
    hd44780_sendStringFlash(PSTR("\xc6"));
  }  
}

void lcd_printMenuCursor(uint8_t cursorType) {
  uint16_t buf = 0;
  switch (lcdMenu.level){ 
    case 0: //Dashboard cursor moving 
      buf = pgm_read_word(&curPosDashboard[lcdMenu.param]);    
      break;
    case 1: //Root menu cursor moving
      buf = pgm_read_word(&curPosRootMenu[lcdMenu.param]);    
      break;
    case 2: //TF and
    case 3: //Solder menu cursor moving
      buf = pgm_read_word(&curPosCalibration[lcdMenu.param]);
      break;
    case 4: //PID Multipliers cursor moving
      buf = pgm_read_word(&curPosPIDMenu[lcdMenu.param]);
      break;
  } 
  uint8_t x = buf >> 12;
  uint8_t y = (buf >> 8) & 0x0F;
  uint8_t cursorId = buf & 0x00FF;
  if (cursorId <= 1) {
    cursorId = pgm_read_byte(&curType[(lcdMenu.isEdit << 1)|
                              (cursorId & 0x01)]);
  }
  if (cursorType != CURSOR_TYPE_ARROW){
    cursorId = cursorType;
  }
  hd44780_goTo(x, y);
  hd44780_sendChar(cursorId);
}

void itoa(char* buf, uint16_t source, uint8_t len, bool zero) {
  uint16_t div = 1;
  uint8_t i = 1;
  while (i < len){
    div *= 10;
    i++;
  }
  while (div > 0)
  {
    *buf = source / div;
    if (*buf > 0){
      *buf += 0x30;
      buf++;
      zero = true;
    } else if (zero) {
      *buf = 0x30;
      buf++;
    }
    source %= div;
    div /= 10;
  }
  *buf = '\0';  
}

void lcd_swapIsEdit(){
  lcdMenu.isEdit ^= 1;
  lcd_printMenuCursor(CURSOR_TYPE_ARROW);
}
