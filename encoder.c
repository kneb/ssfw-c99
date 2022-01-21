/*
 * encoder.cpp
 *
 * Created: 08.02.2021 17:57:25
 *  Author: Neb Konstantin Viktorovich
 */
#include "Headers/globals.h"
#include "Headers/hd44780.h"
#include "Headers/encoder.h"

uint8_t encoderButtonDown = 0;

void encoder_getButtonStatus() {
  if ((PIN_ENCEDER_SW & ENCEDER_SW) == 0) {
    _delay_ms(10); //time for contact unbounce
    if (((PIN_ENCEDER_SW & ENCEDER_SW) == 0) && (encoderButtonDown == 0)) {
      encoder_onClickButton();
      encoderButtonDown = 1;
    }
  } else {
    encoderButtonDown = 0;
  }
}

void encoder_onClickButton() {
  switch(lcdMenu.level) { 
    case 0: //Dashboard
    switch(lcdMenu.param) {
      case 0: //Soldering power on\off
        sound_beep(300, 1, 0);
        (solder.PID.isPowered == 0) ? solder_setPowerOn() : solder_setPowerOff();   
        break;
      case 1: //Thermofan power on\off
        sound_beep(300, 1, 0);
        (thermoFan.PID.isPowered == 0) ? thermoFan_setPowerOn() : thermoFan_setPowerOff();     
        break;
      case 5: //Go to Root menu 
        lcdMenu.level = 1;
        lcdMenu.param = 3;
        lcd_printMenu();
        break;
      default:
        sound_beep(200, 1, 0);
        lcd_swapIsEdit();
    }
    break;
    case 1: //Root menu
      switch(lcdMenu.param) {
        case 0: //Calibration Solder
          lcdMenu.level = 2;
          lcdMenu.param = 2;
          lcd_printCalibration(CALIBRATION_SOLDER);
          break;
        case 1: //Calibration ThermoFan
          lcdMenu.level = 3;
          lcdMenu.param = 2;
          lcd_printCalibration(CALIBRATION_THERMOFAN);
          break;
        case 2: //Save current set's
          eeprom_update_byte(&tf_afanSets, thermoFan.fan);
          eeprom_update_word(&tf_atempSets, thermoFan.PID.temp);
          eeprom_update_word(&sol_atempSets, solder.PID.temp);
          sound_beep(400, 2, 500);
          break;
        case 3: //Exit the root menu
          lcdMenu.level = 0;
          lcdMenu.param = 5;
          lcd_printMain();
          break;
        case 4: //PID Menu
          lcdMenu.level = 4;
          lcdMenu.param = 4;
          lcd_printPIDMenu();
          break;
      }
      break;
    case 2: //Calibration menu Solder
      switch(lcdMenu.param) {
        case 0: //Set etalon2
        case 1: //Set etalon1
          sound_beep(200, 1, 0);
          lcd_swapIsEdit();
          if (lcdMenu.isEdit == 0) {
            pid_fixEtalon(&solder.PID);
          }
          break;
        case 2: //Exit the calibration menu
          solder_readEeprom();
          lcdMenu.level = 1;
          lcdMenu.param = 0;
          lcd_printMenu();
          break;
        case 3: //Save etalons to eeprom
          pid_updateEepromEtalons(&solder.PID, &sol_arefTemp1, &sol_arefTemp2, 
                                  &sol_arefAdc1, &sol_arefAdc2);
          break;
        case 4: //Set power solder on\off
          solder_setPowerFixOnOff();
          break;
        case 5: //Set power editing
          sound_beep(200, 1, 0);
          lcd_swapIsEdit();
          break;
      }      
      break;
    case 3: //Calibration menu ThermoFan
      switch(lcdMenu.param) {
        case 0: //Set etalon2
        case 1: //Set etalon1
          sound_beep(200, 1, 0);
          lcd_swapIsEdit();
          if (lcdMenu.isEdit == 0) {
            pid_fixEtalon(&thermoFan.PID);
          }
          break;
        case 2: //Exit the calibration menu
          thermoFan_readEeprom();
          lcdMenu.level = 1;
          lcdMenu.param = 1;
          lcd_printMenu();
          break;
        case 3: //Save etalons to eeprom
          pid_updateEepromEtalons(&thermoFan.PID, &tf_arefTemp1, &tf_arefTemp2, 
                                  &tf_arefAdc1, &tf_arefAdc2);
          break;
        case 4: //Set power thermofan on\off
          thermoFan_setPowerFixOnOff();
          break;
        case 5: //Set power editing
          sound_beep(200, 1, 0);
          lcd_swapIsEdit();
          break;
      }      
      break;
    case 4: //PID Menu
      switch (lcdMenu.param) {
        case 0: //Solder kP switch edit
        case 1: //TF kP switch edit
        case 2: //TF kI switch edit
        case 3: //TF kD switch edit
          lcd_swapIsEdit();
          break;
        case 4: //Exit
          lcdMenu.level = 1;
          lcdMenu.param = 4;
          lcd_printMenu();
          break;
        case 5: //Save
          pid_updateEeprom(&thermoFan.PID, &tf_akP, &tf_akI, 
                            &tf_akD);
          pid_updateEeprom(&solder.PID, &sol_akP, &sol_akI, 
                            &sol_akD);
          sound_beep(400, 2, 500);
          break;
        case 6: //Solder kD switch edit
        case 7: //Solder kI switch edit
          lcd_swapIsEdit();
          break;
      }
      break;
  }
}

void encoder_onRotation(bool isClockwise) {
  if (lcdMenu.isEdit == 0) { //Moving cursors
    if (lcdMenu.level == 0) { //Dashboard
      lcd_printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true) {       
        lcdMenu.param = (lcdMenu.param == 5) ? 0 : (lcdMenu.param + 1);
      } else {
        lcdMenu.param = (lcdMenu.param == 0) ? 5 : (lcdMenu.param - 1);    
      }
      lcd_printMenuCursor(CURSOR_TYPE_ARROW);
    } else if (lcdMenu.level == 1) { //Root menu
      lcd_printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true) {       
        lcdMenu.param = (lcdMenu.param == 4) ? 0 : (lcdMenu.param + 1);
      } else {
        lcdMenu.param = (lcdMenu.param == 0) ? 4 : (lcdMenu.param - 1);    
      }
      lcd_printMenuCursor(CURSOR_TYPE_ARROW);
    } else if ((lcdMenu.level == 2)||(lcdMenu.level == 3)) { 
      //Calibration menu Thermofan or Solder
      lcd_printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true) {       
        lcdMenu.param = (lcdMenu.param == 5) ? 0 : (lcdMenu.param + 1);
      } else {
        lcdMenu.param = (lcdMenu.param == 0) ? 5 : (lcdMenu.param - 1);    
      }
      lcd_printMenuCursor(CURSOR_TYPE_ARROW);
    } else if (lcdMenu.level == 4){ //PID Menu 
      lcd_printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true) {       
        lcdMenu.param = (lcdMenu.param == 7) ? 0 : (lcdMenu.param + 1);
      } else {
        lcdMenu.param = (lcdMenu.param == 0) ? 7 : (lcdMenu.param - 1);    
      }
      lcd_printMenuCursor(CURSOR_TYPE_ARROW);
    }

  } else { //Edit param
    if (lcdMenu.level == 0) { //Dashboard
      switch (lcdMenu.param) {
        case 2: //Changed fan
          thermoFan_setFanRotation(isClockwise);
          break;
        case 3: //Changed Temp fan
          thermoFan_setTempRotation(isClockwise);
          break;
        case 4: //Changed Temp solder
          solder_setTempRotation(isClockwise);
          break;  
      }
    } else if (lcdMenu.level == 3) { //Calibration menu Thermofan
      switch (lcdMenu.param) {
        case 0: //Changed etalon2
        case 1: //Changed etalon1
          pid_setEtalon(&thermoFan.PID, isClockwise);
          break;
        case 5: //Changed power
          pid_setPowerRotation(&thermoFan.PID, isClockwise);
          break;
      }
    } else if (lcdMenu.level == 2) { //Calibration menu Solder
      switch (lcdMenu.param) {
        case 0: //Changed etalon2
        case 1: //Changed etalon1
          pid_setEtalon(&solder.PID, isClockwise);
          break;
        case 5: //Changed power
          solder_setPower(isClockwise);
          break;
      }
    } else if (lcdMenu.level == 4) { //PID menu
      uint8_t x = 0; 
      uint8_t y = 0; 
      uint8_t pidk = 0;
      stPID_t *pid = NULL;
      switch (lcdMenu.param) {
        case 0: //Solder kP
          x = 1; y = 1; pidk = PID_KP; pid = &solder.PID;
          break;
        case 1: //TF kP
          x = 1; y = 0; pidk = PID_KP; pid = &thermoFan.PID;
          break;
        case 2: //TF kI
          x = 6; y = 0; pidk = PID_KI; pid = &thermoFan.PID;
          break;
        case 3: //TF kD
          x = 11; y = 0; pidk = PID_KD; pid = &thermoFan.PID;
          break;
        case 6: //Solder kI
          x = 11; y = 1; pidk = PID_KD; pid = &solder.PID;
          break;
        case 7: //Solder kI
          x = 6; y = 1; pidk = PID_KI; pid = &solder.PID;
          break;
      }
      lcd_printInt(x, y, pid_setMultiplier(pid, pidk, isClockwise), 3, true);
    }
  }
 
}
