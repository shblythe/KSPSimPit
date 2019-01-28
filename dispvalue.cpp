#include <Arduino.h>

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


