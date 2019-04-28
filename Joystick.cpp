/*
 * Joystick.cpp
 *
 *  Created on: 23 Apr 2019
 *      Author: stephen
 */

#include "Joystick.h"
#include <Arduino.h>

Joystick::Joystick (unsigned char channelX, unsigned char channelY, unsigned char channelZ):
m_channelX(channelX),m_channelY(channelY),m_channelZ(channelZ)
{
  m_zeroX=analogRead(m_channelX);
  m_zeroY=analogRead(m_channelY);
  m_zeroZ=analogRead(m_channelZ);
}

int Joystick::process(int raw)
{
  if (raw>-m_deadZone && raw<m_deadZone)
    return 0;
  if (raw<0)
    raw+=m_deadZone;
  else if (raw>0)
    raw-=m_deadZone;
  return raw*m_factor;
}

int Joystick::readX(void)
{
  return process(analogRead(m_channelX)-m_zeroX);
}

int Joystick::readY(void)
{
  return process(analogRead(m_channelY)-m_zeroY);
}

int Joystick::readZ(void)
{
  return process(analogRead(m_channelZ)-m_zeroZ);
}
