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
#include <stdbool.h>

extern uint8_t encoderButtonDown;
void encoder_onClickButton();
void encoder_onRotation(bool isClockwise);
void encoder_getButtonStatus();

#endif /* ENCODER_H_ */