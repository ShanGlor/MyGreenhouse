/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * MyGreenhouse - A MySensors Gateway / Greenhouse controller
 *
 * REVISION HISTORY
 * Version 1.0 - Rait Lotamõis
 *
 * DESCRIPTION
 * First iteration of a Greenhouse controller sketch.
 * Supports: 
 * 2 relays with individual push buttons
 * BH1750 Light sensor
 * Moisture sensor
 * 
 */

#include "Settings.h"
#include <MySensors.h>
#include <Vcc.h>
#include <menu.h>
#include <TimerOne.h>
#include <menuIO/u8g2Out.h>
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

/**
* Air Temperature, Humidity and Dew point sensor
**/
#ifdef MG_USE_AHUM
#include <Wire.h>
#include "aHum.h"
MyMessage msgAHumHum(0,V_HUM);
MyMessage msgAHumTmp(0,V_TEMP);
MyMessage msgAHumDew(0,V_TEMP);
long aHumTimer = AHUM_MEASUREMENT_INTERVAL;
#endif //#ifdef MG_USE_AHUM

/**
* Nutrient solution EC sensor
**/
#ifdef MG_USE_PPM
#include "PPM.h"
MyMessage msgPpm(0,V_EC);
MyMessage msgPpmUnit(0,V_VAR1);
MyMessage msgPpmK(0,V_VAR2);
long ppmTimer = PPM_MEASUREMENT_INTERVAL;

// Menu items
MENU(ecMenu,"EC Sensor",doNothing,noEvent,noStyle
    ,OP("Calibrate",doNothing,noEvent)
    ,EXIT("<Back")
);
#endif

/**
* Relays
**/
#ifdef MG_USE_RLY_1 || MG_USE_RLY_2
MyMessage msgRly(0,V_STATUS);
bool rlyState1, rlyState2;

// Menu items
TOGGLE(rlyState1,setRly1,"Relay 1: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",1,doNothing,noEvent)
  ,VALUE("Off",0,doNothing,noEvent)
);

TOGGLE(rlyState2,setRly2,"Relay 2: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",1,doNothing,noEvent)
  ,VALUE("Off",0,doNothing,noEvent)
);

MENU(rlyMenu,"Relays",doNothing,noEvent,noStyle
    ,SUBMENU(setRly1)
    ,SUBMENU(setRly2)
    ,EXIT("<Back")
);

#endif


double lastPH = 6.4;

/**
* General variables
**/
uint16_t contrast = LCD_DEFAULT_CONTRAST;
bool receivedConfig = false;
bool metric = true;
long reportTimer = REPORT_INTERVAL;
long timerMillis = 0;
bool asterisk = false;

/**
* Menu setup
**/
using namespace Menu;

// LCD params

U8G2_UC1701_MINI12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock(D6)=*/ 13, /* data(D7)=*/ 11, /* cs(CS)=*/ 10, /* dc(RS)=*/ 9, /* reset(RST)=*/ 8);

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[] MEMMODE={
    {{0,0},{0,1,1}},//bgColor
    {{1,1},{1,0,0}},//fgColor
    {{1,1},{1,0,0}},//valColor
    {{1,1},{1,0,0}},//unitColor
    {{0,1},{0,0,1}},//cursorColor
    {{0,1},{1,0,0}},//titleColor
};

MENU(settingsMenu,"Settings",doNothing,noEvent,noStyle
    ,FIELD(contrast,"Contrast","",0,225,1,0,doNothing,noEvent,wrapStyle)
    ,EXIT("<Back")
);

MENU(mainMenu,"MENU",doNothing,noEvent,wrapStyle
#ifdef MG_USE_RLY_1 || MG_USE_RLY_2
    ,SUBMENU(rlyMenu)
#endif
    ,SUBMENU(settingsMenu)
#ifdef MG_USE_PPM
    ,SUBMENU(ecMenu)
#endif
    ,EXIT("<Exit")
);

ClickEncoder clickEncoder(encA,encB,encBtn,ENC_STEPS_PER_NOTCH);
ClickEncoderStream encStream(clickEncoder,1);
MENU_INPUTS(in,&encStream);

MENU_OUTPUTS(out,MENU_MAX_DEPTH
    ,U8G2_OUT(u8g2,colors,LCD_FONT_X,LCD_FONT_Y,LCD_OFFSET_X,LCD_OFFSET_Y,{0,0,LCD_WIDTH/LCD_FONT_X,LCD_HEIGHT/LCD_FONT_Y})
    ,NONE//SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MENU_MAX_DEPTH,in,out);

// Main screen
result mainScreen(menuOut& o,idleEvent e) {
  if (e == idling){
    // Draw graphics
    int lineY = 1;
    u8g2.drawFrame(0,0,128,64);
    
    //Title
    lineY +=11;
    u8g2.drawStr(21,lineY," MyGreenhouse "); u8g2.drawStr(120,lineY, asterisk?"*":"");
  
    // pH & PPM values
#ifdef MG_USE_PPM
    lineY += 15;
    char pH[3];
    char EC[4];
    u8g2.drawStr(5,lineY,"pH:");u8g2.drawStr(25,lineY,dtostrf(lastPH, 3, 1, pH));
    u8g2.drawStr(70,lineY,"EC:");u8g2.drawStr(90,lineY,itoa(lastPPM, EC, 10));//u8g2.drawStr(110,lineY,"s/cm");
#endif

#ifdef MG_USE_AHUM
    // Air Humidity and Temp values
    lineY += 10;
    char hum[4];
    char temp[4];
    u8g2.drawStr(5,lineY,"aHum:");u8g2.drawStr(38,lineY, strcat(itoa(lastAHumHum, hum, 10),"%"));
    u8g2.drawStr(70,lineY,"T:");u8g2.drawStr(85,lineY,strcat(dtostrf(lastAHumTemp, 4, 1, temp),"\260C"));
#endif

#ifdef MG_USE_RLY_1 || MG_USE_RLY_2
    // Relay output values
    lineY += 10;
    u8g2.drawStr(5,lineY,"Pump1:");u8g2.drawStr(45,lineY,rlyState1?"ON":"OFF");
    u8g2.drawStr(70,lineY,"Pump2:");u8g2.drawStr(105,lineY,rlyState2?"ON":"OFF");
#endif
    
    nav.idleChanged=true;

  }
}
//** End Menu setup **//

/**
* Sketch Setup
**/
void setup()
{
#ifdef MG_USE_AHUM
    // Initialise aHum sensor
    Wire.begin();
#endif

#ifdef MG_USE_RLY_1
    pinMode(RLY_PIN_1, OUTPUT);
    rlyState1 = loadState(RLY_ID_1);
    digitalWrite(RLY_PIN_1, rlyState1?RLY_ON:RLY_OFF);
#endif
#ifdef MG_USE_RLY_2
    pinMode(RLY_PIN_2, OUTPUT);
    rlyState2 = loadState(RLY_ID_2);
    digitalWrite(RLY_PIN_2, rlyState2?RLY_ON:RLY_OFF);
#endif

#ifdef MG_USE_PPM
    // Initialize the PPM sensor
    pinMode(PPM_POWER_PIN, OUTPUT );
    pinMode(PPM_GROUND_PIN,OUTPUT);
    digitalWrite(PPM_GROUND_PIN,LOW);
    pinMode(PPM_SENSE_PIN,INPUT);

    ppmK = loadState(PPM_CAL_ID);
#endif

    // Initialize the timer interrupt for tracking time
    pinMode(encBtn,INPUT_PULLUP);
    Timer1.initialize(ONE_MSECOND_IN_MICROS);
    Timer1.attachInterrupt(timerIsr);

    // Initialize LCD for ArduinoMenu4
    u8g2.begin();
    u8g2.setFont(LCD_FONT);
    u8g2.setBitmapMode(0);

    nav.idleTask = mainScreen;//point a function to be used when menu is suspended
    nav.timeOut = 10;
    Serial.println("setup done.");Serial.flush();

    // Start with the main screen
    nav.idleOn(mainScreen);
}

/**
* Presentation
**/
void presentation()
{
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo("MyGreenhouse", "1.0");

    // Register Relays
#ifdef MG_USE_RLY_1
    present(RLY_ID_1, S_BINARY);
#endif
#ifdef MG_USE_RLY_2
    present(RLY_ID_2, S_BINARY);
#endif

#ifdef MG_USE_AHUM
    // Register aHum sensors
    present(AHUM_HUM_ID, S_HUM);
    present(AHUM_TMP_ID, S_TEMP);
    present(AHUM_DEW_ID, S_CUSTOM);
#endif
  
}

/**
* Main Loop
**/
void loop()
{
    nav.doInput();
    if (nav.changed(0)) {
        u8g2.setContrast(contrast);
        u8g2.firstPage();
        do nav.doOutput(); while(u8g2.nextPage());
    }

#ifdef MG_USE_RLY_1
    if (rlyState1 != digitalRead(RLY_PIN_1)?RLY_ON:RLY_OFF){
        digitalWrite(RLY_PIN_1, rlyState1?RLY_ON:RLY_OFF);
        saveState(RLY_ID_1, rlyState1?true:false);
        send(msgRly.setSensor(RLY_ID_1).set(rlyState1?true:false), false);
    }
#endif
#ifdef MG_USE_RLY_2
    if (rlyState2 != digitalRead(RLY_PIN_2)?RLY_ON:RLY_OFF){
        digitalWrite(RLY_PIN_2, rlyState2?RLY_ON:RLY_OFF);
        saveState(RLY_ID_2, rlyState2?true:false);
        send(msgRly.setSensor(RLY_ID_2).set(rlyState2?true:false), false);
    }
#endif

#ifdef MG_USE_PPM
    // Get PPM level if it is time
    if (ppmTimer == 0){
#ifdef MY_DEBUG
        Serial.println("Taking PPM measurements: ");
#endif
        readPPMValues(lastAHumTemp);
#ifdef MY_DEBUG
        Serial.print(lastPPM);
        Serial.println("ppm");
        Serial.print(lastEC);
        Serial.println("s/cm");
#endif
        ppmTimer = PPM_MEASUREMENT_INTERVAL;
    }
#endif //MG_USE_PPM

#ifdef MG_USE_AHUM
    if(aHumTimer == 0){
        readAHumValues();
        aHumTimer = AHUM_MEASUREMENT_INTERVAL;
    }
#endif

  // Send reports it if is time
    if (reportTimer == 0){
#ifdef MY_DEBUG
        Serial.println("Sending reports.");
#endif
        sendReports();
        reportTimer = REPORT_INTERVAL;
    }
  
}

/**
* Send all reports using last known values
**/
void sendReports(){

#ifdef MG_USE_PPM
    // Send PPM
    send(msgPpm.set(lastPPM));
#endif

#ifdef MG_USE_AHUM
    // Send SHT21 sensors data
    send(msgAHumTmp.set(lastAHumTemp,1));
    send(msgAHumHum.set(lastAHumHum,1));
    send(msgAHumDew.set(lastAHumDew,1));
#endif

}

/**
* Incoming messages from the GW
**/
void receive(const MyMessage &message)
{
    if (message.isAck()) {
#ifdef MY_DEBUG
        Serial.println("This is an ack from gateway");
#endif
        return;
    }

#ifdef MG_USE_RLY_1 || MG_USE_RLY_2
    if (message.type==V_STATUS) {
        // Change relay state
        int outputPin = (message.sensor == RLY_ID_1)?RLY_PIN_1:RLY_PIN_2;
        digitalWrite(outputPin, message.getBool()?RLY_ON:RLY_OFF);
        // Store state in eeprom
        saveState(message.sensor, message.getBool());
#ifdef MY_DEBUG
        // Write some debug info
        Serial.print("Incoming change for relay:");
        Serial.print(message.sensor);
        Serial.print(", New status: ");
        Serial.println(message.getBool());
#endif
        // Update the state variables
        rlyState1 = loadState(RLY_ID_1);
        rlyState2 = loadState(RLY_ID_2);
    }
#endif
}



/**
* Interrupt Service Routines
**/
void timerIsr() {
    clickEncoder.service();
    timerMillis++;
    if (timerMillis == 500 || timerMillis == 1000)
        asterisk = !asterisk;
    if (timerMillis == 1000){
        countDownOneSecond();
        timerMillis = 0;
    }
}

void countDownOneSecond(void) {
#ifdef MG_USE_PPM
    if (ppmTimer > 0){
        ppmTimer--;
    }
#endif
#ifdef MG_USE_AHUM
    if (aHumTimer > 0){
        aHumTimer--;
    }
#endif
    if (reportTimer > 0) {
        reportTimer--;
    }
}
