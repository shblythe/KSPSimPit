#include <LiquidCrystal.h>
#include <Button.h>

#include <KerbalSimpit.h>
#include <PayloadStructs.h>

#define LCD 1

Button stageBtn(6);

#if LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

KerbalSimpit mySimpit(Serial);

float vvi=0;
float apoapsis;
float periapsis;
int32_t tApoapsis;
int32_t tPeriapsis;
int msgCount=0;

const char mult_chars[]="afpnum kMGTPE";
const int default_mult=6;

//convert a value to an integer with the specified number of digits, decimal places, and a multiplier, e.g. M, k etc.
//value - the value to convert
//dp - the number of decimal places
//str - the destination string
//len - the total length of the string, excluding '\0'
void dispValue(float value, int dp, char *str, int len, bool dashneg, bool scale)
{
  int numlen=len;
  if (scale)
    numlen-=1; // subtract 1 for the multiplier
  if (value<0 && dashneg)
  {
    for (int i=0; i<len; i++)
      str[i]='-';
    str[len]='\0';
    return;
  }
  else
    numlen=len-1; // subtract 1 for the sign
  int mult=default_mult;  // default multiplier is none (1)
  if (dp>0)
    numlen-=(dp+1);  // subtract the number of DPs, plus the space for the DP itself
  int maxnum=pow(10,numlen);
  float minnum=powf(10,-dp);
  if (scale)
  {
    while (fabsf(value)>maxnum)
    {
      value/=1000.0;
      mult+=1;
    }
    while (value!=0.0 && fabsf(value)<minnum)
    {
      value*=1000.0;
      mult-=1;
    }
  }
  if (dp>0)
  {
    dtostrf(value,numlen+dp+1,dp,str);
    if (scale)
      str[numlen+dp+1]=mult_chars[mult];
  }
  else    
  {
    char fmt[80];
    if (scale)
    {
      snprintf(fmt,80,"%%%dd%%c",numlen);
      snprintf(str,len+1,fmt,(int)value,mult_chars[mult]);
    }
    else
    {
      snprintf(fmt,80,"%%%dd",numlen);
      snprintf(str,len+1,fmt,(int)value);
    }
  }
}

void dispTime(int value, char *str, int len)
{
  snprintf(str,len+1,"%02d:%02d",value/60,value%60);
}

void callbackHandler(byte msgType, byte msg[], byte msgSize)
{
  msgCount++;
  switch (msgType) {
    case VELOCITY_MESSAGE:
      if (msgSize==sizeof(velocityMessage))
      {
        velocityMessage vel;
        vel=parseVelocity(msg);
        vvi=vel.vertical;        
      }
      break;
    case APSIDES_MESSAGE:
      if (msgSize==sizeof(apsidesMessage))
      {
        apsidesMessage aps;
        aps=parseApsides(msg);
        periapsis=aps.periapsis;
        apoapsis=aps.apoapsis;
      }
      break;
    case APSIDESTIME_MESSAGE:
      if (msgSize==sizeof(apsidesTimeMessage))
      {
        apsidesTimeMessage apt;
        apt=parseApsidesTime(msg);
        tPeriapsis=apt.periapsis;
        tApoapsis=apt.apoapsis;
      }
      break;      
  }
}

void setup() {
  stageBtn.begin();

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
  
#if LCD
  lcd.clear();
  lcd.print("CONNECTED");
#else
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_BUILTIN, LOW);
#endif

  mySimpit.inboundHandler(callbackHandler);
  mySimpit.registerChannel(VELOCITY_MESSAGE);
  mySimpit.registerChannel(APSIDES_MESSAGE);
  mySimpit.registerChannel(APSIDESTIME_MESSAGE);
}

void loop() {
  char line1[17];
  char line2[17];
  mySimpit.update();  // check for new serial message
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
    dispValue(vvi,0,vvi_string,5,false,false);
    snprintf(line1,17,"VVI%5s %5d   ",vvi_string,msgCount);
    snprintf(line2,17,"                ");
  }
  */
  {
    char ap_string[5],pe_string[5];
    char tap_string[6],tpe_string[6];
    dispValue(apoapsis,0,ap_string,4,true,true);
    dispValue(periapsis,0,pe_string,4,true,true);
    dispTime(tApoapsis,tap_string,5);
    dispTime(tPeriapsis,tpe_string,5);
    snprintf(line1,17,"A%4s:%5s     ",ap_string,tap_string);
    snprintf(line2,17,"P%4s:%5s   ",pe_string,tpe_string); 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  
  delay(100);
}
