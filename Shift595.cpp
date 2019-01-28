#include <Arduino.h>

#include "Shift595.h"

void Shift595::shift_state()
{
  digitalWrite(m_latchPin,LOW);
  shiftOut(m_dataPin,m_clockPin,MSBFIRST,m_state);
  digitalWrite(m_latchPin,HIGH);
}

Shift595::Shift595(int latchPin, int clockPin, int dataPin):m_latchPin(latchPin),m_clockPin(clockPin),m_dataPin(dataPin)
{
  m_state=0;
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void Shift595::setBit(int bitNum)
{
  byte mask=1<<bitNum;
  m_state|=mask;
  shift_state();
}

void Shift595::clearBit(int bitNum)
{
  byte mask=~(1<<bitNum);
  m_state&=mask;
  shift_state();
}

void Shift595::allOn()
{
  m_state=-1;
  shift_state();
}

void Shift595::allOff()
{
  m_state=0;
  shift_state();
}


