/*
 * LedGauge.h
 *
 *  Created on: 23 Apr 2019
 *      Author: stephen
 */

#ifndef LEDGAUGE_H_
#define LEDGAUGE_H_

class LedGauge
{
private:
  int m_redLed;
  int m_yelLed;
  int m_grnLed;

  const float m_greenThreshold=50.0f;	// below this % green is not lit
  const float m_yellowThreshold=20.0f;	// below this % yellow is not lit
  const float m_redThreshold=5.0f;	// below this % red flashes, but is extinguished at 0
  const unsigned int m_flashPeriodMs=200;
  const unsigned int m_flashMarkMs=100;
public:
  LedGauge(int red, int yel, int grn);
  void illuminate(float value, float maxValue);
};

#endif /* LEDGAUGE_H_ */
