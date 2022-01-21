/*
 * thermofan.h
 *
 * Created: 07.02.2021 2:34:45
 *  Author: Neb Konstantin Viktorovich
 */

#ifndef THERMOFAN_H_
#define THERMOFAN_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include "pid.h"

#define MAX_TFAN_TEMP 450
#define MIN_TFAN_TEMP 50
#define TFAN_COOLING 50

#define TF_ON_HOLDER 2
#define TF_HEAT_ON_FIX_POWER 3
#define TF_HEAT_COOLING 4

#define PORT_GERKON PINC
#define GERKON (1 << PINC3)

extern uint8_t EEMEM tf_afanSets;
extern uint16_t EEMEM tf_arefTemp1;
extern uint16_t EEMEM tf_arefTemp2;
extern uint16_t EEMEM tf_arefAdc1;
extern uint16_t EEMEM tf_arefAdc2;
extern uint16_t EEMEM tf_atempSets;
extern float EEMEM tf_akP;
extern float EEMEM tf_akI;
extern float EEMEM tf_akD;

typedef struct {
    uint8_t heatinStage;
    uint8_t fan;
    uint8_t fanBuf;
    stPID_t PID;
} stThermoFan_t;

void thermoFan_init();
void thermoFan_setPowerOn();
void thermoFan_setPowerOff();
void thermoFan_setPowerSleep();
void thermoFan_setPowerFixOnOff();
void thermoFan_setFan(uint8_t fan);
void thermoFan_setFanRotation(bool isClockwise);
void thermoFan_setFanPWM();
void thermoFan_setTemp(uint16_t temp);
void thermoFan_setTempRotation(bool isClockwise);
void thermoFan_getStatus();
void thermoFan_getCooling();
void thermoFan_readEeprom();

#endif /* THERMOFAN_H_ */