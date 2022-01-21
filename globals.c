/*
 * globals.cpp
 *
 * Created: 09.02.2021 13:59:12
 *  Author: Neb Konstantin Viktorovich
 */

#include "Headers/globals.h"

stThermoFan_t thermoFan;
stSolder_t solder;
stSound_t sound;

uint8_t tim = 0;

void init() {
  PORTB = 0b00110001;
  DDRB = 0b00111111;
  DDRC = 0b00000000;
  DDRD = 0b11110011;
  PORTC = 0b00010000;
  
  hd44780_init();
  lcd_init();
  solder_init();
  thermoFan_init();
  
  MCUCR |= (1 << ISC00)|(1 << ISC10); //any logical change on INT0 and INT1
  GICR |= (1 << INT0);

  TCCR0 = (1 << CS00)|(1 << CS02); //tim0 divider 1024
  TIMSK = (1 << TOIE0)|(1 << OCIE1B)|(1 << TOIE1);

  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13)|(1 << WGM12)|(1 << CS12)|(1 << CS10);
  ICR1 = 3124;
  OCR1B = 1500;

  TCCR2 = (1 << WGM21)|(1 << WGM20)|(1 << CS22)|(1 << CS21);

  ADMUX = (1 << REFS1)|(1 << REFS0); //internal 2.56v //128kHz
  ADCSRA = (1 << ADEN)|(1 << ADIE)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); 
}

ISR(INT1_vect) { //Vibro sensor hakko
  GICR &= ~(1 << INT1);
  solder.timerSleep = 0;
  if (solder.PID.isPowered == SOL_HEAT_SLEEP) {
    solder_setPowerOn();
    sound_beep(100, 3, 200);
  }
}

ISR(INT0_vect) { //the encoder has been turned
  GICR &= ~(1 << INT0); //disable ext.int. on INT0
  uint8_t b = (PINC & 32) >> 5;
  _delay_ms(1);
  uint8_t a = (PIND & 4) >> 2;
  if (a == b) {
    encoder_onRotation(false);
  } else {
    encoder_onRotation(true);
  }
  GIFR |= (1 << INTF0);
  GICR |= (1 << INT0); //enable ext.int. on INT0
}

ISR(TIMER0_OVF_vect) { //Timer0 at frequency ~61Hz (~16,4ms)
  sound_getBeep();
  if (tim < 61) {   
    tim ++;
    if ((tim % 30) == 0) {
      thermoFan_getStatus();
      solder_getStatus();
      if (lcdMenu.level == 0) {
        lcd_printIconsStatus();
      }
    }
    if ((tim % 15) == 0) {
      thermoFan_getCooling();
    }
  } else { //The code is executable with a frequency of one second
    tim = 0;
    if (lcdMenu.level == 0) { //Dashboard
      lcd_printInt(1, 0, thermoFan.PID.currentTemp, 3, true);
      lcd_printInt(1, 1, solder.PID.currentTemp, 3, true);
    } else if (lcdMenu.level == 3) { //Calibration menu thermofan
      lcd_printInt(10, 0, thermoFan.PID.currentTemp, 3, true);
      if(lcdMenu.isEdit == 1) {
        if (lcdMenu.param == 0) {
          lcd_printInt(5, 1, thermoFan.PID.adc, 4, true);
        } else if (lcdMenu.param == 1) {
          lcd_printInt(5, 0, thermoFan.PID.adc, 4, true);
        }                  
      }
    } else if (lcdMenu.level == 2) { //Calibration menu solder
      lcd_printInt(10, 0, solder.PID.currentTemp, 3, true);
      if(lcdMenu.isEdit == 1) {
        if (lcdMenu.param == 0) {
          lcd_printInt(5, 1, solder.PID.adc, 4, true);
        } else if (lcdMenu.param == 1) {
          lcd_printInt(5, 0, solder.PID.adc, 4, true);
        }                  
      }
    }     
    if (solder.PID.isPowered == POWER_HEAT_ON) {
      if (solder.timerSleep < DELAY_SLEEP) {
        solder.timerSleep += 1;
      } else {
        solder_setPowerSleep();
      }
      GICR |= (1 << INT1);
    }
  }
}

void sound_getBeep(){
  if (sound.beepCount > 0) {
    if (sound.beepTim > 0) {
      sound.beepTim--;
    } else {
      SOUND_PORT ^= SOUND_PIN;
      sound.beepCount--;
      sound.beepTim = (sound.beepCount%2 == 0) ? sound.beepDurationDelay : 
                                                 sound.beepDuration;
    }
  }  
}

void sound_beep(uint16_t duration_ms, uint8_t count, uint16_t delay_ms) {  
  sound.beepDuration = duration_ms/16;
  sound.beepTim = sound.beepDuration;
  sound.beepDurationDelay = delay_ms/16;
  sound.beepCount = count*2-1;
  SOUND_PORT &= ~SOUND_PIN;
}

ISR(ADC_vect) {
  if ((ADMUX & 1) == 1) {
    pid_tempConversion(&thermoFan.PID, ADCW);
  } else {
    pid_tempConversion(&solder.PID, ADCW);
  }
}

ISR(TIMER1_COMPB_vect) {
  _delay_us(800);
  ADMUX &= 0xFE;
  ADCSRA |= (1 << ADSC); //start ADC converter
}

ISR(TIMER1_OVF_vect) {
  _delay_us(800);
  ADMUX |= 1;
  ADCSRA |= (1 << ADSC); //start ADC converter
}