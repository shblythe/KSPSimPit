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
 * D Add more functionality from KSPLCD
 *    D SAS control button (pin 8)
 *    D Mode button to switch between display modes LAUNCH/ORBIT
 * - Autopilot?  Or should I focus on making this a manual controller for now?
 * D Other manual controls - currently supported by KerbalSimpit
 *    D SAS/RCS switches
 *    D Indicator LEDs
 *    	D Fuel
 *    	D Mono
 *    	D Electricity
 *    	D Ablator
 *    D Joystick rotation
 *    D Rotation/Translation switch
 *    D Rotation/Translation joystick button reverser
 * - Other manual controls - not yet supported by KerbalSimpit
 *    - SAS mode leds
 *    - SAS mode buttons
 * - Work out what to do with CAG buttons
 * D Dim LEDs with software PWM?
 * - Alphanumeric display
 * - Mode buttons
 * - Reset button
 * - Should reset when comms stops
 * * Issues
 *    D throttle doesn't work
 *    - issue with ACTIONSTATUS_MESSAGE not being seen when status is 0
 *    	- until this is fixed, need to keep brake on permanently or SAS etc. don't work
 *    - SAS and RCS buttons are reversed, would be nicer to find a class which handles this
 *    D stage button has broken - seems to not work when other actions are on, e.g. SAS, RCS
 *    - EVA doesn't work, using either the controller or keyboard - can't turn RCS on
 *    - Switching vessels doesn't work properly
 *    - The gauges don't really belong in KSPData
 */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Button.h>

#include <KerbalSimpit.h>
#include "KSPData.h"
#include "dispvalue.h"
#include "IO.h"
#include <SoftPWM.h>
#include "Joystick.h"

Joystick *js;

//Buttons
Button stageBtn(BTN_STAGE);
Button brakesSw(SW_BRAKES);
Button stageArmSw(SW_STAGE_ARM);
Button sasSw(SW_SAS);
Button rcsSw(SW_RCS);
Button tranRotSw(SW_TRAN_ROT);
Button tranRotBtn(BTN_TRAN_ROT);

const struct rotationMessage rotTran0={0,0,0,0x07};

void init_buttons()
{
  stageBtn.begin();
  brakesSw.begin();
  stageArmSw.begin();
  sasSw.begin();
  rcsSw.begin();
  tranRotSw.begin();
  tranRotBtn.begin();
}

void init_leds()
{
  pinMode(LED_COMMS,OUTPUT);	digitalWrite(LED_COMMS,HIGH);
  pinMode(LED_STAGE,OUTPUT);	digitalWrite(LED_STAGE,LOW);
  pinMode(LED_SAS,OUTPUT);	digitalWrite(LED_SAS,LOW);
  pinMode(LED_RCS,OUTPUT);	digitalWrite(LED_RCS,LOW);
  SoftPWMBegin();
}

//Button modeBtn(6);
//Button sasBtn(8);
bool sas=false;

typedef enum {
  STG_OFF,	// the switch may be on, but we've staged, or just started, so must be switch off before arming
  STG_READY,	// the switch has been off, so next time it's switched on, we'll arm
  STG_ARMED	// we're armed, and ready to stage
} StagingMode;
StagingMode stgMode=STG_OFF;

#if LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

KerbalSimpit mySimpit(Serial);
KSPData *kspData;

enum {
	MODE_FIRST,
	MODE_ORBIT=MODE_FIRST,
	MODE_TEST,
	// Add more modes above here
	MODE_LAST
};

int mode=MODE_ORBIT;

void setup() {
  init_buttons();
  init_leds();
  js=new Joystick(JOY_X,JOY_Y,JOY_Z);


#if LCD
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("WAITING FOR LINK");
#endif
  // Open the serial connection.
  Serial.begin(38400);

#if !LCD
  // Set initial pin states, and turn on the LED
  digitalWrite(LED_COMMS, HIGH);
#endif
  int mode=LOW;

  // This loop continually attempts to handshake with the plugin.
  // It will keep retrying until it gets a successful handshake.
  while (!mySimpit.init()) {
    delay(100);
#if !LCD
    digitalWrite(LED_COMMS,mode);
#endif
    if (mode==LOW)
      mode=HIGH;
    else
      mode=LOW;
  }
  
#if LCD
  lcd.clear();
  lcd.print("CONNECTED");
#else
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_COMMS, LOW);
#endif
  kspData=new KSPData(&mySimpit);
}

void loop() {
  char line1[17];
  char line2[17];
  kspData->update();  // check for new serial message

  // put your main code here, to run repeatedly:

  if (stgMode==STG_OFF)
  {
    if (!stageArmSw.read())
      stgMode=STG_READY;
    digitalWrite(LED_STAGE,LOW);
  }
  else if (stgMode==STG_READY)
  {
    mySimpit.deactivateAction(STAGE_ACTION);
    if (stageArmSw.read())
      stgMode=STG_ARMED;
  }
  else	// STG_ARMED
  {
    if (!stageArmSw.read())
      stgMode=STG_OFF;
    else
    {
      digitalWrite(LED_STAGE,HIGH);
      if (stageBtn.pressed())
      {
	mySimpit.activateAction(STAGE_ACTION);
	stgMode=STG_OFF;
      }
    }
  }
  if (brakesSw.read() && (kspData->get_actionStatus() & BRAKES_ACTION) == 0)
    mySimpit.activateAction(BRAKES_ACTION);
  else if (!brakesSw.read() && (kspData->get_actionStatus() & BRAKES_ACTION)!=0)
    mySimpit.deactivateAction(BRAKES_ACTION);
  if (!sasSw.read() && (kspData->get_actionStatus() & SAS_ACTION) == 0)
    mySimpit.activateAction(SAS_ACTION);
  else if (sasSw.read() && (kspData->get_actionStatus() & SAS_ACTION)!=0)
    mySimpit.deactivateAction(SAS_ACTION);
  if (!rcsSw.read() && (kspData->get_actionStatus() & RCS_ACTION) == 0)
    mySimpit.activateAction(RCS_ACTION);
  else if (rcsSw.read() && (kspData->get_actionStatus() & RCS_ACTION)!=0)
    mySimpit.deactivateAction(RCS_ACTION);

  if ((tranRotSw.read() && tranRotBtn.read()) || (!tranRotSw.read() && !tranRotBtn.read()))
  {
    struct rotationMessage rot;
    rot.pitch=-js->readY();
    rot.roll=js->readZ();
    rot.yaw=js->readX();
    rot.mask=0x07;
    mySimpit.send(ROTATION_MESSAGE,(byte*)&rot,sizeof(rot));
    mySimpit.send(TRANSLATION_MESSAGE,(byte*)&rotTran0,sizeof(rotTran0));
  }
  else
  {
    struct translationMessage tran;
    tran.X=-js->readX();
    tran.Y=js->readY();
    tran.Z=-js->readZ();
    tran.mask=0x07;
    mySimpit.send(TRANSLATION_MESSAGE,(byte*)&tran,sizeof(tran));
    mySimpit.send(ROTATION_MESSAGE,(byte*)&rotTran0,sizeof(rotTran0));
  }

  int throttle=analogRead(JOY_THROTTLE)*32;
  mySimpit.send(THROTTLE_MESSAGE,(byte*)&throttle,sizeof(throttle));
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
#if LCD
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
#endif
}
