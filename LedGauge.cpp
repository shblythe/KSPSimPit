/*
 * LedGauge.cpp
 *
 *  Created on: 23 Apr 2019
 *      Author: stephen
 */

#include "LedGauge.h"
#include <SoftPWM.h>
#include <Arduino.h>

LedGauge::LedGauge(int red, int yel, int grn):m_redLed(red),m_yelLed(yel),m_grnLed(grn)
{
  pinMode(m_redLed,OUTPUT);	digitalWrite(m_redLed,LOW);
  pinMode(m_yelLed,OUTPUT);	digitalWrite(m_yelLed,LOW);
  SoftPWMSet(m_grnLed,0);
}

void LedGauge::illuminate(float value, float maxValue)
{
  float perc=value/maxValue*100.0f;
  SoftPWMSet(m_grnLed,perc>m_greenThreshold?50:0);
  digitalWrite(m_yelLed,perc>m_yellowThreshold);
  digitalWrite(m_redLed,perc>m_redThreshold || (value>0.1f && millis()%m_flashPeriodMs < m_flashMarkMs));
}
