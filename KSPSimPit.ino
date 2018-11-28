#include "KerbalSimpit.h"
#include <LiquidCrystal.h>
#include <Button.h>

#define LCD 0

Button stageBtn(11);

#if LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

KerbalSimpit mySimpit(Serial);

void setup() {
#if LCD
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("UPLOAD WINDOW");
#endif
  delay(2000);  // Wait two seconds to allow upload from IDE before we start

#if LCD
  lcd.clear();
  lcd.print("WAITING FOR LINK");
#endif
  // Open the serial connection.
  Serial.begin(38400);

  // Set initial pin states, and turn on the LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  int mode=LOW;

  // This loop continually attempts to handshake with the plugin.
  // It will keep retrying until it gets a successful handshake.
  while (!mySimpit.init()) {
    delay(100);
    digitalWrite(LED_BUILTIN,mode);
    if (mode==LOW)
      mode=HIGH;
    else
      mode=LOW;
  }
  
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_BUILTIN, LOW);
#if LCD
  lcd.clear();
  lcd.print("CONNECTED");
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  if (stageBtn.pressed())
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    mySimpit.activateAction(STAGE_ACTION);
  }
  delay(1);
}
