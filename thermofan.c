/*
 * thermofan.cpp
 *
 * Created: 05.02.2021 15:57:08
 *  Author: Neb Konstantin Viktorovich
 */

#include "Headers/globals.h"
#include "Headers/thermofan.h"

uint8_t EEMEM tf_afanSets = 60;
uint16_t EEMEM tf_arefTemp1 = 30;
uint16_t EEMEM tf_arefTemp2 = 190;
uint16_t EEMEM tf_arefAdc1 = 108;
uint16_t EEMEM tf_arefAdc2 = 200;
uint16_t EEMEM tf_atempSets = 200;
float EEMEM tf_akP = 0.4;
float EEMEM tf_akI = 0.3;
float EEMEM tf_akD = 0.2;

void thermoFan_init() {
  thermoFan.PID.currentTemp = 0;
  thermoFan.heatinStage = POWER_HEAT_OFF;
  thermoFan.PID.isPowered = POWER_HEAT_OFF;
}

void thermoFan_readEeprom() {
  pid_readEeprom(&thermoFan.PID, &tf_akP, &tf_akI, &tf_akD, &tf_atempSets, 
                &tf_arefTemp1, &tf_arefTemp2, &tf_arefAdc1, &tf_arefAdc2);
  pid_atributesConversion(&thermoFan.PID);
  thermoFan.fan = eeprom_read_byte(&tf_afanSets);
  thermoFan_setFanPWM();
}

void thermoFan_setPowerOn() {
  thermoFan.PID.isPowered = POWER_HEAT_ON;
  thermoFan.heatinStage = POWER_HEAT_ON;
  LED_PORT &= ~LED_FEN; //Led on
  TCCR2 |= (1 << COM21); //PWM Fan on
  TCCR1A |= (1 << COM1A1); //PWM Temp on
  pid_clear(&thermoFan.PID);
}

void thermoFan_setPowerOff() {
  thermoFan.PID.isPowered = POWER_HEAT_OFF;
  TCCR1A &= ~(1 << COM1A1); //PWM Temp off
  if (thermoFan.PID.currentTemp < TFAN_COOLING) {
      TCCR2 &= ~(1 << COM21); //PWM Fan off
  } else {  
    if ((PORT_GERKON & GERKON)||(lcdMenu.level==3)) {
      thermoFan.fanBuf = thermoFan.fan;
      thermoFan_setFan((uint8_t)100);
      thermoFan.heatinStage = TF_HEAT_COOLING;
    }
  }
  LED_PORT |= LED_FEN; //Led off
}

void thermoFan_setPowerSleep() {
  thermoFan.PID.isPowered = TF_ON_HOLDER;
  thermoFan.heatinStage = TF_HEAT_COOLING;
  TCCR1A &= ~(1 << COM1A1); //PWM Temp off
  thermoFan.fanBuf = thermoFan.fan;
  thermoFan_setFan((uint8_t)100);
}

void thermoFan_setPowerFixOnOff() {
  if (thermoFan.PID.isPowered != 3) { //If power off, then power set on
    thermoFan.PID.isPowered = TF_HEAT_ON_FIX_POWER;
    thermoFan.heatinStage = POWER_HEAT_ON;
    LED_PORT &= ~LED_FEN; //Led on
    TCCR2 |= (1 << COM21); //PWM Fan on
    TCCR1A |= (1 << COM1A1); //PWM Temp on
  } else {
    thermoFan_setPowerOff();
  }
}

void thermoFan_setFan(uint8_t fan) {
  thermoFan.fan = fan;
    if (lcdMenu.level == 0) {
      lcd_printInt(7, 0, fan, 3, false);
      if (fan < 100){
        hd44780_sendStringFlash(PSTR("%"));
      }
    }
  thermoFan_setFanPWM();
}

void thermoFan_setFanPWM() {
  OCR2 = thermoFan.fan*2 + thermoFan.fan/2 + thermoFan.fan/20;
}

void thermoFan_setFanRotation(bool isClockwise) {
  if (isClockwise) {
    if (thermoFan.fan < 100){
      thermoFan.fan += 1;
      thermoFan_setFan(thermoFan.fan);
    }      
  } else {
    if (thermoFan.fan > 20) {
      thermoFan.fan -= 1;
      thermoFan_setFan(thermoFan.fan);
    }
  }
}

void thermoFan_setTemp(uint16_t temp) {
  thermoFan.PID.temp = temp;
  lcd_printInt(12, 0, temp, 3, true);
}

void thermoFan_setTempRotation(bool isClockwise) {
  if (isClockwise) {
    if (thermoFan.PID.temp < MAX_TFAN_TEMP) {
      thermoFan.PID.temp += 5;
      thermoFan_setTemp(thermoFan.PID.temp);
    }
  } else {
    if (thermoFan.PID.temp > MIN_TFAN_TEMP) {
      thermoFan.PID.temp -= 5;
      thermoFan_setTemp(thermoFan.PID.temp);
    }
  }
}

void thermoFan_getStatus() {
  if ((PORT_GERKON & GERKON) == 0) { //ThermoFan on stand
    if (thermoFan.PID.isPowered == POWER_HEAT_ON) {
      thermoFan_setPowerSleep();
      sound_beep(150, 2, 200);
    } else if (thermoFan.PID.isPowered == TF_ON_HOLDER) {
        if (thermoFan.heatinStage != TF_HEAT_COOLING)
        LED_PORT ^= LED_FEN; // Blink Led fan
    }
  } else {
    if (thermoFan.PID.isPowered == TF_ON_HOLDER) {
      thermoFan_setFan(thermoFan.fanBuf);
      thermoFan_setPowerOn();
    }
  }
}

void thermoFan_getCooling() {
  if (thermoFan.heatinStage == TF_HEAT_COOLING) {
    if (thermoFan.PID.currentTemp < TFAN_COOLING) {
      thermoFan.heatinStage = POWER_HEAT_OFF;
      thermoFan_setFan(thermoFan.fanBuf);
      LED_PORT |= LED_FEN; //Led off
      TCCR2 &= ~(1 << COM21); //PWM Fan off
      sound_beep(150, 3, 200);
    } else {
      LED_PORT ^= LED_FEN; // Blink Led fan
    }
  }
}