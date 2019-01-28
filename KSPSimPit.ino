/*
 * TODO:
 * D Check into a git repo (github?)
 *    D git init first
 *    D create repo
 *    D push it
 * * Create a singleton class to handle all the callbacks
 *    The idea is that that this class would have the one callback function, and different functions (or classes) could register
 *    to listen to particular messages.  It would also handle registering itself and the channels.
 *    D first version
 *    - can I (or should I) improve the singleton pattern, or will it do?
 * D Move the dispValue and dispTime functions out
 * - Find out why it crashes
 */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Button.h>

#include <KerbalSimpit.h>
#include "KSPData.h"
#include "dispvalue.h"
#include "Shift595.h"

#define LCD 1

Button stageBtn(6);

#if LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

KerbalSimpit mySimpit(Serial);
KSPData *kspData;
Shift595 shifter(9,13,7);

void setup() {
  stageBtn.begin();
  shifter.allOn();

#if LCD
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("WAITING FOR LINK");
#endif
  // Open the serial connection.
  Serial.begin(38400);

#if !LCD
  // Set initial pin states, and turn on the LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
#endif
  int mode=LOW;

  // This loop continually attempts to handshake with the plugin.
  // It will keep retrying until it gets a successful handshake.
  while (!mySimpit.init()) {
    delay(100);
#if !LCD
    digitalWrite(LED_BUILTIN,mode);
#endif
    if (mode==LOW)
      mode=HIGH;
    else
      mode=LOW;
  }
  shifter.allOff();
  
#if LCD
  lcd.clear();
  lcd.print("CONNECTED");
#else
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_BUILTIN, LOW);
#endif
  kspData=new KSPData(&mySimpit);
}

void loop() {
  char line1[17];
  char line2[17];
  kspData->update();  // check for new serial message
  // put your main code here, to run repeatedly:
  if (stageBtn.pressed())
  {
#if !LCD
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
#endif
    mySimpit.activateAction(STAGE_ACTION);
  }
  /*
  {
    char vvi_string[6];
    dispValue(kspData->get_vvi(),0,vvi_string,5,false,false);
    snprintf(line1,17,"VVI%5s %5d   ",vvi_string,msgCount);
    snprintf(line2,17,"                ");
  }
  */
  {
    char ap_string[5],pe_string[5];
    char tap_string[6],tpe_string[6];
    dispValue(kspData->get_apoapsis(),0,ap_string,4,true,true);
    dispValue(kspData->get_periapsis(),0,pe_string,4,true,true);
    dispTime(kspData->get_tApoapsis(),tap_string,5);
    dispTime(kspData->get_tPeriapsis(),tpe_string,5);
    snprintf(line1,17,"A%4s:%5s     ",ap_string,tap_string);
    snprintf(line2,17,"P%4s:%5s   ",pe_string,tpe_string); 
  }
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  
  delay(10);
}
