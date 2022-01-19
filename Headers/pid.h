/*
 * globals.h
 *
 * Created: 09.02.2021 13:58:42
 *  Author: Neb Konstantin Viktorovich
 */

#ifndef PID_H_
#define PID_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

enum class PID_Types {
  PID_KP, PID_KI, PID_KD
};

class PIDRegulator{
  private:
    float kP;
    float kI;
    float kD;
    float dT;
    float integral;
    int16_t prevError;
  public:
    PIDRegulator();
    uint8_t computePower(uint16_t input, uint16_t set);
    void clear();
    uint16_t getMultiplier(PID_Types typeKPID);
    void setMultiplier(PID_Types typeKPID, bool isClockwise);
    void readEeprom(float *akP, float *akI, float *akD);
    void updateEeprom(float *akP, float *akI, float *akD);
};

#endif /* PID_H_ */