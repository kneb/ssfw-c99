/*
 * solder.h
 *
 * Created: 07.02.2021 2:33:41
 *  Author: Neb Konstantin Viktorovich
 */

#ifndef SOLDER_H_
#define SOLDER_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include "pid.h"

#define MAX_SOLDER_TEMP 410
#define MIN_SOLDER_TEMP 50
#define DELAY_SLEEP 180 //in sec (3 min)

#define SOL_HEAT_SLEEP 2
#define SOL_HEAT_ON_FIX_POWER 3

#define PORT_VIBRO PIND
#define VIBRO (1 << PIND3)

extern uint16_t EEMEM sol_arefTemp1;
extern uint16_t EEMEM sol_arefTemp2;
extern uint16_t EEMEM sol_arefAdc1;
extern uint16_t EEMEM sol_arefAdc2;
extern uint16_t EEMEM sol_atempSets;
extern float EEMEM sol_akP;
extern float EEMEM sol_akI;
extern float EEMEM sol_akD;

typedef struct {
    uint8_t timerSleep;
    stPID_t PID;
} stSolder_t;

void solder_init();
void solder_setPowerOn();
void solder_setPowerOff();
void solder_setPowerFixOnOff();
void solder_setTemp(uint16_t temp);
void solder_setTempRotation(bool isClockwise);
void solder_setPowerSleep();
void solder_setPower(uint8_t power);
void solder_setPowerRotation(bool isClockwise);
void solder_readEeprom();
void solder_getStatus();

#endif /* SOLDER_H_ */