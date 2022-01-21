/*
 * solder.cpp
 *
 * Created: 05.02.2021 15:57:08
 *  Author: Neb Konstantin Viktorovich
 */ 

#include "Headers/globals.h"
#include "Headers/solder.h"

uint16_t EEMEM sol_arefTemp1 = 20;
uint16_t EEMEM sol_arefTemp2 = 150;
uint16_t EEMEM sol_arefAdc1 = 100;
uint16_t EEMEM sol_arefAdc2 = 500;
uint16_t EEMEM sol_atempSets = 250;
float EEMEM sol_akP = 0.4;
float EEMEM sol_akI = 0.3;
float EEMEM sol_akD = 0.2;

void solder_init() {
  solder.PID.currentTemp = 0;
  solder.timerSleep = 0;
  solder.PID.isPowered = POWER_HEAT_OFF;
}

void solder_setPowerOn() {
  solder.PID.isPowered = POWER_HEAT_ON;
  LED_PORT &= ~LED_SOL; //Led solder on
  TCCR1A |= (1 << COM1B1); //PWM Temp on
  pid_clear(&solder.PID);
}

void solder_setPowerOff() {
  solder.PID.isPowered = POWER_HEAT_OFF;
  LED_PORT |= LED_SOL; //Led solder off
  TCCR1A &= ~(1 << COM1B1); //PWM Temp off
}

void solder_setPowerFixOnOff() {
  if (solder.PID.isPowered != SOL_HEAT_ON_FIX_POWER) {
    solder.PID.isPowered = SOL_HEAT_ON_FIX_POWER;
    LED_PORT &= ~LED_SOL; //Led solder on
    TCCR1A |= (1 << COM1B1); //PWM Temp on
  } else {
    solder_setPowerOff();
  }
}

void solder_setPowerSleep() {
  solder.PID.isPowered = SOL_HEAT_SLEEP;
  TCCR1A &= ~(1 << COM1B1); //PWM Temp off
  sound_beep(300, 3, 400);
}

void solder_getStatus() {
  if (solder.PID.isPowered == SOL_HEAT_SLEEP) {
    LED_PORT ^= LED_SOL; //Led solder blink
  }
}

void solder_setTemp(uint16_t temp) {
  solder.PID.temp = temp;
  lcd_printInt(12, 1, temp, 3, true);
}

void solder_setTempRotation(bool isClockwise) {
  if (isClockwise){
    if (solder.PID.temp < MAX_SOLDER_TEMP) {
      solder.PID.temp += 5;
      solder_setTemp(solder.PID.temp);
    }
  } else {
    if (solder.PID.temp > MIN_SOLDER_TEMP) {
      solder.PID.temp -= 5;
      solder_setTemp(solder.PID.temp);
    }
  }
}

void solder_setPower(uint8_t power) {
  solder.PID.power = power;
  OCR1B = 31 * power;
}

void solder_setPowerRotation(bool isClockwise) {
  if (isClockwise){
    if (solder.PID.power < 100) {
      solder.PID.power += 1;
      solder_setPower(solder.PID.power);
    }
  } else {
    if (solder.PID.power > 1) {
      solder.PID.power -= 1;
      solder_setPower(solder.PID.power);
    }
  }
  lcd_printInt(10, 1, solder.PID.power, 3, true);
}

void solder_readEeprom(){
  pid_readEeprom(&solder.PID, &sol_akP, &sol_akI, &sol_akD, &sol_atempSets, 
                &sol_arefTemp1, &sol_arefTemp2, &sol_arefAdc1, &sol_arefAdc2);
  pid_atributesConversion(&solder.PID);
}