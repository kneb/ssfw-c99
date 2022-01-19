/*
 * encoder.cpp
 *
 * Created: 08.02.2021 17:57:25
 *  Author: Neb Konstantin Viktorovich
 */
#include "Headers/globals.h"
#include "Headers/hd44780.h"
#include "Headers/encoder.h"

Encoder::Encoder(){
  this->buttonDown = 0;
}

void Encoder::getButtonStatus(){
  if ((PIN_ENCEDER_SW & ENCEDER_SW) == 0){
    _delay_ms(10); //time for contact unbounce
    if (((PIN_ENCEDER_SW & ENCEDER_SW) == 0) && (this->buttonDown == 0)){
      this->onClickButton();
      this->buttonDown = 1;
    }
  } else {
    this->buttonDown = 0;
  }
}

void Encoder::onClickButton(){
  switch(lcd.menu.level){ 
    case 0: //Dashboard
    switch(lcd.menu.param){
      case 0: //Soldering power on\off
        sound.beep(300, 1, 0);
        (solder.isPowered == 0) ? solder.setPowerOn() : solder.setPowerOff();   
        break;
      case 1: //Thermofan power on\off
        sound.beep(300, 1, 0);
        (thermoFan.isPowered == 0) ? thermoFan.setPowerOn() : thermoFan.setPowerOff();     
        break;
      case 5: //Go to Root menu 
        lcd.menu.level = 1;
        lcd.menu.param = 3;
        lcd.printMenu();
        break;
      default:
        sound.beep(200, 1, 0);
        lcd.swapIsEdit();
    }
    break;
    case 1: //Root menu
      switch(lcd.menu.param){
        case 0: //Calibration Solder
          lcd.menu.level = 2;
          lcd.menu.param = 2;
          lcd.printCalibration(CALIBRATION_SOLDER);
          break;
        case 1: //Calibration ThermoFan
          lcd.menu.level = 3;
          lcd.menu.param = 2;
          lcd.printCalibration(CALIBRATION_THERMOFAN);
          break;
        case 2: //Save current set's
          eeprom_update_byte(&ThermoFan::afanSets, thermoFan.fan);
          eeprom_update_word(&ThermoFan::atempSets, thermoFan.temp);
          eeprom_update_word(&Solder::atempSets, solder.temp);
          sound.beep(400, 2, 500);
          break;
        case 3: //Exit the root menu
          lcd.menu.level = 0;
          lcd.menu.param = 5;
          lcd.printMain();
          break;
        case 4: //PID Menu
          lcd.menu.level = 4;
          lcd.menu.param = 4;
          lcd.printPIDMenu();
          break;
      }
      break;
    case 2: //Calibration menu Solder
      switch(lcd.menu.param){
        case 0: //Set etalon2
        case 1: //Set etalon1
          sound.beep(200, 1, 0);
          lcd.swapIsEdit();
          if (lcd.menu.isEdit == 0) {
            solder.fixEtalon();
          }
          break;
        case 2: //Exit the calibration menu
          solder.readEeprom();
          lcd.menu.level = 1;
          lcd.menu.param = 0;
          lcd.printMenu();
          break;
        case 3: //Save etalons to eeprom
          solder.updateEeprom();
          break;
        case 4: //Set power solder on\off
          solder.setPowerFixOnOff();
          break;
        case 5: //Set power editing
          sound.beep(200, 1, 0);
          lcd.swapIsEdit();
          break;
      }      
      break;
    case 3: //Calibration menu ThermoFan
      switch(lcd.menu.param){
        case 0: //Set etalon2
        case 1: //Set etalon1
          sound.beep(200, 1, 0);
          lcd.swapIsEdit();
          if (lcd.menu.isEdit == 0) {
            thermoFan.fixEtalon();
          }
          break;
        case 2: //Exit the calibration menu
          thermoFan.readEeprom();
          lcd.menu.level = 1;
          lcd.menu.param = 1;
          lcd.printMenu();
          break;
        case 3: //Save etalons to eeprom
          thermoFan.updateEeprom();
          break;
        case 4: //Set power thermofan on\off
          thermoFan.setPowerFixOnOff();
          break;
        case 5: //Set power editing
          sound.beep(200, 1, 0);
          lcd.swapIsEdit();
          break;
      }      
      break;
    case 4: //PID Menu
      switch (lcd.menu.param){
        case 0: //Solder kP switch edit
        case 1: //TF kP switch edit
        case 2: //TF kI switch edit
        case 3: //TF kD switch edit
          lcd.swapIsEdit();
          break;
        case 4: //Exit
          lcd.menu.level = 1;
          lcd.menu.param = 4;
          lcd.printMenu();
          break;
        case 5: //Save
          thermoFan.PID.updateEeprom(&ThermoFan::akP, &ThermoFan::akI, &ThermoFan::akD);
          solder.PID.updateEeprom(&Solder::akP, &Solder::akI, &Solder::akD);
          sound.beep(400, 2, 500);
          break;
        case 6: //Solder kD switch edit
        case 7: //Solder kI switch edit
          lcd.swapIsEdit();
          break;
      }
      break;
  }
}

void Encoder::onRotation(bool isClockwise){
  if (lcd.menu.isEdit == 0){ //Moving cursors
    if (lcd.menu.level == 0){ //Dashboard
      lcd.printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true){       
        lcd.menu.param = (lcd.menu.param == 5) ? 0 : (lcd.menu.param + 1);
      } else {
        lcd.menu.param = (lcd.menu.param == 0) ? 5 : (lcd.menu.param - 1);    
      }
      lcd.printMenuCursor(CURSOR_TYPE_ARROW);
    } else if (lcd.menu.level == 1){ //Root menu
      lcd.printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true){       
        lcd.menu.param = (lcd.menu.param == 4) ? 0 : (lcd.menu.param + 1);
      } else {
        lcd.menu.param = (lcd.menu.param == 0) ? 4 : (lcd.menu.param - 1);    
      }
      lcd.printMenuCursor(CURSOR_TYPE_ARROW);
    } else if ((lcd.menu.level == 2)||(lcd.menu.level == 3)) { //Calibration menu Thermofan or Solder
      lcd.printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true){       
        lcd.menu.param = (lcd.menu.param == 5) ? 0 : (lcd.menu.param + 1);
      } else {
        lcd.menu.param = (lcd.menu.param == 0) ? 5 : (lcd.menu.param - 1);    
      }
      lcd.printMenuCursor(CURSOR_TYPE_ARROW);
    } else if (lcd.menu.level == 4){ //PID Menu 
      lcd.printMenuCursor(CURSOR_TYPE_EMPTY);
      if (isClockwise == true){       
        lcd.menu.param = (lcd.menu.param == 7) ? 0 : (lcd.menu.param + 1);
      } else {
        lcd.menu.param = (lcd.menu.param == 0) ? 7 : (lcd.menu.param - 1);    
      }
      lcd.printMenuCursor(CURSOR_TYPE_ARROW);
    }

  } else { //Edit param
    if (lcd.menu.level == 0){ //Dashboard
      switch (lcd.menu.param){
        case 2: //Changed fan
          thermoFan.setFan(isClockwise);
          break;
        case 3: //Changed Temp fan
          thermoFan.setTemp(isClockwise);
          break;
        case 4: //Changed Temp solder
          solder.setTemp(isClockwise);
          break;  
      }
    } else if (lcd.menu.level == 3){ //Calibration menu Thermofan
      switch (lcd.menu.param){
        case 0: //Changed etalon2
          thermoFan.setEtalon(isClockwise);
          break;
        case 1: //Changed etalon1
          thermoFan.setEtalon(isClockwise);
          break;
        case 5: //Changed power
          thermoFan.setPower(isClockwise);
          break;
      }
    } else if (lcd.menu.level == 2){ //Calibration menu Solder
      switch (lcd.menu.param){
        case 0: //Changed etalon2
          solder.setEtalon(isClockwise);
          break;
        case 1: //Changed etalon1
          solder.setEtalon(isClockwise);
          break;
        case 5: //Changed power
          solder.setPower(isClockwise);
          break;
      }
    } else if (lcd.menu.level == 4){ //PID menu
      switch (lcd.menu.param){
        case 0: //Solder kP
          solder.PID.setMultiplier(PID_Types::PID_KP, isClockwise);
          lcd.printInt(1, 1, solder.PID.getMultiplier(PID_Types::PID_KP), 3);
          break;
        case 1: //TF kP
          thermoFan.PID.setMultiplier(PID_Types::PID_KP, isClockwise);
          lcd.printInt(1, 0, thermoFan.PID.getMultiplier(PID_Types::PID_KP), 3);
          break;
        case 2: //TF kI
          thermoFan.PID.setMultiplier(PID_Types::PID_KI, isClockwise);
          lcd.printInt(6, 0, thermoFan.PID.getMultiplier(PID_Types::PID_KI), 3);
          break;
        case 3: //TF kD
          thermoFan.PID.setMultiplier(PID_Types::PID_KD, isClockwise);
          lcd.printInt(11, 0, thermoFan.PID.getMultiplier(PID_Types::PID_KD), 3);
          break;
        case 6: //Solder kI
          solder.PID.setMultiplier(PID_Types::PID_KD, isClockwise);
          lcd.printInt(11, 1, solder.PID.getMultiplier(PID_Types::PID_KD), 3);
          break;
        case 7: //Solder kI
          solder.PID.setMultiplier(PID_Types::PID_KI, isClockwise);
          lcd.printInt(6, 1, solder.PID.getMultiplier(PID_Types::PID_KI), 3);
          break;
      }
    }
  }
 
}
