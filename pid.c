/*
 * globals.cpp
 *
 * Created: 09.02.2021 13:59:12
 *  Author: Neb Konstantin Viktorovich
 */

#include "Headers/pid.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "Headers/globals.h"

void pid_init(stPID_t *pid) {
  pid->integral = 0.;
  pid->prevError = 0;
  pid->dT = 0.2;
}

uint8_t pid_computePower(stPID_t *pid, uint16_t input, 
                                  uint16_t set) {
  int16_t error = set - input;
  pid->integral = pid->integral + error * pid->dT * pid->kI;
  if (pid->integral > 100) { pid->integral = 100.;}
  if (pid->integral < 0) { pid->integral = 0.;}
  float diff = (error - pid->prevError) / pid->dT;
  pid->prevError = error;
  int16_t result = error * pid->kP + pid->integral + diff * pid->kD;
  if (result > 100) { result = 100;}
  if (result < 0) { result = 0;}
  return result;
}

void pid_clear(stPID_t *pid) {
  pid->integral = 0;
  pid->prevError = 0;
}
    
uint16_t pid_getMultiplier(stPID_t *pid, uint8_t typeKPID) {
  if (typeKPID == PID_KP) {
    return (uint16_t)(pid->kP * 100);
  } else if (typeKPID == PID_KI) {
    return pid->kI * 100;
  } else {
    return pid->kD * 100;
  }
}

uint16_t pid_setMultiplier(stPID_t *pid, uint8_t typeKPID, 
          bool isClockwise) {
  float dp = 0;
  if (isClockwise) dp = 0.01; else dp = -0.01;          
  if (typeKPID == PID_KP) {
    pid->kP += dp;
    return pid->kP * 100;
  } else if (typeKPID == PID_KI) {
    pid->kI += dp;
    return pid->kI * 100;
  } else {
    pid->kD += dp;
    return pid->kD * 100;   
  }
}
    
void pid_readEeprom(stPID_t *pid, float *akP, float *akI, float *akD, 
      uint16_t *atempSets, uint16_t *arefTemp1, uint16_t *arefTemp2,
      uint16_t *arefAdc1, uint16_t *arefAdc2) {
  pid->kP = eeprom_read_float(akP);
  pid->kI = eeprom_read_float(akI);
  pid->kD = eeprom_read_float(akD);
  pid->temp = eeprom_read_word(atempSets);
  pid->refTemp1 = eeprom_read_word(arefTemp1);
  pid->refTemp2 = eeprom_read_word(arefTemp2);
  pid->refAdc1 = eeprom_read_word(arefAdc1);
  pid->refAdc2 = eeprom_read_word(arefAdc2);
}

void pid_updateEeprom(stPID_t *pid, float *akP, float *akI, float *akD) {
  eeprom_update_float(akP, pid->kP);
  eeprom_update_float(akI, pid->kI);
  eeprom_update_float(akD, pid->kD);
}

void pid_fixEtalon(stPID_t *pid) {
  if (lcdMenu.param == 1) {
    pid->refAdc1 = pid->adc;
  } else if (lcdMenu.param == 0) {
    pid->refAdc2 = pid->adc;
  }
  pid_atributesConversion(pid);
}

void pid_atributesConversion(stPID_t *pid) {
  if ((pid->refAdc2 - pid->refAdc1) != 0) {
    pid->k = (float)(pid->refTemp2 - pid->refTemp1) / 
              (pid->refAdc2 - pid->refAdc1);
  } else {
    pid->k = 1.;
  }
  pid->b = pid->refTemp2 - pid->k * pid->refAdc2;
}

void pid_setEtalon(stPID_t *pid, bool isClockwise) {
  if (lcdMenu.param == 1) {
    if (isClockwise){
      pid->refTemp1 += 1;
    } else {
      pid->refTemp1 -= 1;
    }
    lcd_printInt(0, 0, pid->refTemp1, 3, true);
  } else {
    if (isClockwise) {
      pid->refTemp2 += 1;
    } else {
      pid->refTemp2 -= 1;
    }
    lcd_printInt(0, 1, pid->refTemp2, 3, true);
  }
}

void pid_tempConversion(stPID_t *pid, uint16_t adc) {
  pid->currentTemp = pid->k * adc + pid->b;
  pid->adc = (pid->adc + adc)/2;
  if (pid->isPowered == POWER_HEAT_ON) {
    pid_setPower(pid, pid_computePower(pid, 
          pid->currentTemp, pid->temp));
  }
}

void pid_setPower(stPID_t *pid, uint8_t power){
  pid->power = power;
  OCR1A = power * 31 + power/4;
}

void pid_setPowerRotation(stPID_t *pid, bool isClockwise) {
  if (isClockwise) {
    if (pid->power < 100) {
      pid->power += 5;
      pid_setPower(pid, pid->power);
    }
  } else {
    if (pid->power > 5) {
      pid->power -= 5;
      pid_setPower(pid, pid->power);
    }
  }
  lcd_printInt(10, 1, pid->power, 3, true);
}

void pid_updateEepromEtalons(stPID_t *pid, 
      uint16_t *arefTemp1, uint16_t *arefTemp2,
      uint16_t *arefAdc1, uint16_t *arefAdc2) {
  eeprom_update_word(arefAdc1, pid->refAdc1);
  eeprom_update_word(arefAdc2, pid->refAdc2);
  eeprom_update_word(arefTemp1, pid->refTemp1);
  eeprom_update_word(arefTemp2, pid->refTemp2);
  sound_beep(400, 2, 500);
}