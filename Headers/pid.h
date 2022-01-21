/*
 * globals.h
 *
 * Created: 09.02.2021 13:58:42
 *  Author: Neb Konstantin Viktorovich
 */

#ifndef PID_H_
#define PID_H_

#define F_CPU 16000000UL
#define PID_KP 0
#define PID_KI 1
#define PID_KD 2

#define POWER_HEAT_OFF 0
#define POWER_HEAT_ON 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>

typedef struct {
    uint8_t isPowered;
    float kP;
    float kI;
    float kD;
    float dT;
    float integral;
    int16_t prevError;
    uint16_t temp;
    uint16_t currentTemp;
    float k;
    float b;
    uint16_t refTemp1;
    uint16_t refTemp2;
    uint16_t refAdc1;
    uint16_t refAdc2;
    uint16_t adc;
    uint8_t power;
} stPID_t;

void pid_init(stPID_t *pid);
uint8_t pid_computePower(stPID_t *pid, uint16_t input, uint16_t set);
void pid_clear(stPID_t *pid);
uint16_t pid_getMultiplier(stPID_t *pid, uint8_t typeKPID);
uint16_t pid_setMultiplier(stPID_t *pid, uint8_t typeKPID, bool isClockwise);
void pid_readEeprom(stPID_t *pid, float *akP, float *akI, float *akD,
        uint16_t *atempSets, uint16_t *arefTemp1, uint16_t *arefTemp2,
        uint16_t *arefAdc1, uint16_t *arefAdc2);
void pid_updateEeprom(stPID_t *pid, float *akP, float *akI, float *akD);
void pid_updateEepromEtalons(stPID_t *pid, 
      uint16_t *arefTemp1, uint16_t *arefTemp2,
      uint16_t *arefAdc1, uint16_t *arefAdc2);

void pid_fixEtalon(stPID_t *pid);
void pid_atributesConversion(stPID_t *pid);
void pid_setEtalon(stPID_t *pid, bool isClockwise);
void pid_tempConversion(stPID_t *pid, uint16_t adc);
void pid_setPower(stPID_t *pid, uint8_t power);
void pid_setPowerRotation(stPID_t *pid, bool isClockwise);

#endif /* PID_H_ */