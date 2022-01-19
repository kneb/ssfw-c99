/*
 * globals.cpp
 *
 * Created: 09.02.2021 13:59:12
 *  Author: Neb Konstantin Viktorovich
 */

#include "Headers/pid.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

PIDRegulator::PIDRegulator(){
  this->integral = 0.;
  this->prevError = 0;
  this->dT = 0.2;
}

uint8_t PIDRegulator::computePower(uint16_t input, uint16_t set){
  int16_t error = set - input;
  this->integral = this->integral + error * this->dT * this->kI;
  if (this->integral > 100){ this->integral = 100.;}
  if (this->integral < 0){ this->integral = 0.;}
  float diff = (error - prevError) / this->dT;
  this->prevError = error;
  int16_t result = error * this->kP + this->integral + diff * this->kD;
  if (result > 100) { result = 100;}
  if (result < 0) { result = 0;}
  return result;
}

void PIDRegulator::clear(){
  this->integral = 0;
  this->prevError = 0;
}
    
uint16_t PIDRegulator::getMultiplier(PID_Types typeKPID){
  if (typeKPID == PID_Types::PID_KP){
    return (uint16_t)(this->kP*100);
  } else if (typeKPID == PID_Types::PID_KI){
    return this->kI*100;
  } else {
    return this->kD*100;
  }
}

void PIDRegulator::setMultiplier(PID_Types typeKPID, 
          bool isClockwise){
  float dp = 0;
  if (isClockwise) dp = 0.01; else dp = -0.01;          
  if (typeKPID == PID_Types::PID_KP){
    this->kP += dp;
  } else if (typeKPID == PID_Types::PID_KI){
    this->kI += dp;
  } else {
    this->kD += dp;   
  }
}
    
void PIDRegulator::readEeprom(float *akP, float *akI, float *akD){
  this->kP = eeprom_read_float(akP);
  this->kI = eeprom_read_float(akI);
  this->kD = eeprom_read_float(akD);
}

void PIDRegulator::updateEeprom(float *akP, float *akI, float *akD){
  eeprom_update_float(akP, this->kP);
  eeprom_update_float(akI, this->kI);
  eeprom_update_float(akD, this->kD);
}