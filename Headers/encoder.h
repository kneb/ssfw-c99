/*
 * encoder.h
 *
 * Created: 08.02.2021 17:52:14
 *  Author: Neb Konstantin Viktorovich
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>
#include <util/delay.h>

class Encoder{
  private:
    uint8_t buttonDown;
    void onClickButton();
  public:
    Encoder();
    void onRotation(bool isClockwise);
    void getButtonStatus();
};


#endif /* ENCODER_H_ */