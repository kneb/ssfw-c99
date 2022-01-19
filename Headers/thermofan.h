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

#define TF_HEAT_OFF 0
#define TF_HEAT_ON 1
#define TF_ON_HOLDER 2
#define TF_HEAT_ON_FIX_POWER 3
#define TF_HEAT_COOLING 4

#define PORT_GERKON PINC
#define GERKON (1 << PINC3)

class ThermoFan{
  private:
    uint8_t heatinStage;
  public:
    static uint16_t EEMEM arefTemp1; 
    static uint16_t EEMEM arefTemp2;
    static uint16_t EEMEM arefAdc1;
    static uint16_t EEMEM arefAdc2;
    static uint16_t EEMEM atempSets;
    static uint8_t EEMEM afanSets;
    static float EEMEM akP;
    static float EEMEM akI;
    static float EEMEM akD;

    uint8_t fan;
    uint8_t fanBuf;
    uint16_t temp;
    uint16_t currentTemp;
    uint8_t isPowered;
    float k;
    float b;
    uint16_t refTemp1; 
    uint16_t refTemp2;
    uint16_t refAdc1;
    uint16_t refAdc2;
    uint16_t adc;
    uint8_t power;

    PIDRegulator PID;

    ThermoFan();
    void setPowerOn();
    void setPowerOff();
    void setPowerSleep();
    void setPowerFixOnOff();
    void setPower(uint8_t power);
    void setPower(bool isClockwise);
    void setFan(uint8_t fan);
    void setFan(bool isClockwise);
    void setFanPWM();
    void setTemp(uint16_t temp);
    void setTemp(bool isClockwise);
    void getStatus();
    void getCooling();
    void tempConversion(uint16_t adc);
    void atributesConversion();
    void setEtalon(bool isClockwise);
    void fixEtalon();
    void readEeprom();
    void updateEeprom();
};


#endif /* THERMOFAN_H_ */