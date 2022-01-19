/*
 * solder.cpp
 *
 * Created: 05.02.2021 15:57:08
 *  Author: Neb Konstantin Viktorovich
 */ 

#include "Headers/globals.h"
#include "Headers/solder.h"

uint16_t EEMEM Solder::arefTemp1 = 20;
uint16_t EEMEM Solder::arefTemp2 = 150;
uint16_t EEMEM Solder::arefAdc1 = 100;
uint16_t EEMEM Solder::arefAdc2 = 500;
uint16_t EEMEM Solder::atempSets = 250;
float EEMEM Solder::akP = 0.4;
float EEMEM Solder::akI = 0.3;
float EEMEM Solder::akD = 0.2;

Solder::Solder(){
  this->currentTemp = 0;
  this->timerSleep = 0;
  this->isPowered = SOL_HEAT_OFF;
}

void Solder::setPowerOn(){
  this->isPowered = SOL_HEAT_ON;
  LED_PORT &= ~LED_SOL; //Led solder on
  TCCR1A |= (1 << COM1B1); //PWM Temp on
  this->PID.clear();
}

void Solder::setPowerOff(){
  this->isPowered = SOL_HEAT_OFF;
  LED_PORT |= LED_SOL; //Led solder off
  TCCR1A &= ~(1 << COM1B1); //PWM Temp off
}

void Solder::setPowerFixOnOff(){
  if (this->isPowered != SOL_HEAT_ON_FIX_POWER){
    this->isPowered = SOL_HEAT_ON_FIX_POWER;
    LED_PORT &= ~LED_SOL; //Led solder on
    TCCR1A |= (1 << COM1B1); //PWM Temp on
  } else {
    this->setPowerOff();
  }
}

void Solder::setPowerSleep(){
  this->isPowered = SOL_HEAT_SLEEP;
  TCCR1A &= ~(1 << COM1B1); //PWM Temp off
  sound.beep(300, 3, 400);
}

void Solder::getStatus(){
  if (this->isPowered == SOL_HEAT_SLEEP){
    LED_PORT ^= LED_SOL; //Led solder blink
  }
}

void Solder::setTemp(uint16_t temp){
  this->temp = temp;
  lcd.printInt(12, 1, solder.temp, 3);
}

void Solder::setTemp(bool isClockwise){
  if (isClockwise){
    if (this->temp < MAX_SOLDER_TEMP){
      this->temp += 5;
      this->setTemp(this->temp);
    }
  } else {
    if (this->temp > MIN_SOLDER_TEMP){
      this->temp -= 5;
      this->setTemp(this->temp);
    }
  }
}

void Solder::setPower(uint8_t power){
  this->power = power;
  OCR1B = 31*power;
}

void Solder::setPower(bool isClockwise){
  if (isClockwise){
    if (this->power < 100){
      this->power += 1;
      this->setPower(this->power);
    }
  } else {
    if (this->power > 1){
      this->power -= 1;
      this->setPower(this->power);
    }
  }
  lcd.printInt(10, 1, this->power, 3);
}

void Solder::tempConversion(uint16_t adc){
  this->currentTemp = this->k*adc + this->b;
  this->adc = (this->adc+adc)/2;
  if (this->isPowered == SOL_HEAT_ON){
    this->setPower(this->PID.computePower(this->currentTemp, this->temp));
  }
}

void Solder::atributesConversion(){
  if ((this->refAdc2-this->refAdc1) != 0){
    this->k = (float)(this->refTemp2-this->refTemp1) / 
              (this->refAdc2-this->refAdc1);
  } else {
    this->k = 1.;
  }
  this->b = this->refTemp2 - this->k*this->refAdc2;
}

void Solder::setEtalon(bool isClockwise){
  if (lcd.menu.param == 1){
    if (isClockwise){
      this->refTemp1 += 1;
    } else {
      this->refTemp1 -= 1;
    }
    lcd.printInt(0, 0, this->refTemp1, 3);
  } else {
    if (isClockwise){
      this->refTemp2 += 1;
    } else {
      this->refTemp2 -= 1;
    }
    lcd.printInt(0, 1, this->refTemp2, 3);
  }
}

void Solder::fixEtalon(){
  if (lcd.menu.param == 1){
    this->refAdc1 = this->adc;
  } else if (lcd.menu.param == 0) {
    this->refAdc2 = this->adc;
  }
  this->atributesConversion();
}

void Solder::readEeprom(){
  this->PID.readEeprom(&Solder::akP, &Solder::akI, &Solder::akD);
  this->temp = eeprom_read_word(&Solder::atempSets);
  this->refTemp1 = eeprom_read_word(&Solder::arefTemp1);
  this->refTemp2 = eeprom_read_word(&Solder::arefTemp2);
  this->refAdc1 = eeprom_read_word(&Solder::arefAdc1);
  this->refAdc2 = eeprom_read_word(&Solder::arefAdc2);
  this->atributesConversion();
}

void Solder::updateEeprom(){
  eeprom_update_word(&Solder::arefAdc1, this->refAdc1);
  eeprom_update_word(&Solder::arefAdc2, this->refAdc2);
  eeprom_update_word(&Solder::arefTemp1, this->refTemp1);
  eeprom_update_word(&Solder::arefTemp2, this->refTemp2);
  sound.beep(400, 2, 500);
}
