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
 * D Find out why it crashes
 *   D Aha!  I've stopped it crashing by running "update" in loop without any delay, but only updating the display if there have
 *      been messages received since the last update - making it a bit more real-time I guess.
 * * Add more functionality from KSPLCD
 *    D SAS control button (pin 8)
 *    - Mode button to switch between display modes LAUNCH/ORBIT
 *    - Autopilot?  Or should I focus on making this a manual controller for now?
 * - Other manual controls
 *    - Indicator LEDs for various functions, fuel levels etc.
 *    - Throttle control VR
 *    - Joysticks
 *    - Watch some videos and find out what others have got!
 */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Button.h>

#include <KerbalSimpit.h>
#include "KSPData.h"
#include "dispvalue.h"
#include "Shift595.h"

#define LCD 1

Button modeBtn(6);
Button stageBtn(10);
Button sasBtn(8);
bool sas=false;

#if LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

KerbalSimpit mySimpit(Serial);
KSPData *kspData;
Shift595 shifter(9,13,7);

enum {
	MODE_FIRST,
	MODE_ORBIT=MODE_FIRST,
	MODE_TEST,
	// Add more modes above here
	MODE_LAST
};

int mode=MODE_ORBIT;

void setup() {
  stageBtn.begin();
  sasBtn.begin();
  modeBtn.begin();

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
  if (sasBtn.pressed())
  {
    if (sas)
      mySimpit.deactivateAction(SAS_ACTION);
    else
      mySimpit.activateAction(SAS_ACTION);
    sas=!sas;
  }
  if (modeBtn.pressed())
  {
    mode++;
    if (mode==MODE_LAST)
      mode=MODE_FIRST;
  }
  /*
  {
    char vvi_string[6];
    dispValue(kspData->get_vvi(),0,vvi_string,5,false,false);
    snprintf(line1,17,"VVI%5s %5d   ",vvi_string,kspData->get_msgCount());
    snprintf(line2,17,"                ");
  }
  */
  if (kspData->get_msgCount()>0)
  {
    if (mode==MODE_ORBIT)
    {
      char ap_string[5],pe_string[5];
      char tap_string[6],tpe_string[6];
      dispValue(kspData->get_apoapsis(),0,ap_string,4,true,true);
      dispValue(kspData->get_periapsis(),0,pe_string,4,true,true);
      dispTime(kspData->get_tApoapsis(),tap_string,5);
      dispTime(kspData->get_tPeriapsis(),tpe_string,5);
      snprintf(line1,17,"A%4s:%5s     ",ap_string,tap_string);
      snprintf(line2,17,"P%4s:%5s     ",pe_string,tpe_string);
    }
    else
    {
	snprintf(line1,17,"TEST MODE LINE 1");
	snprintf(line2,17,"TEST MODE LINE 2");
    }
    kspData->clear_msgCount();
  }
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  
}
