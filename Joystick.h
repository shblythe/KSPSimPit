/*
 * Joystick.h
 *
 *  Created on: 23 Apr 2019
 *      Author: stephen
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

class Joystick
{
private:
  int m_zeroX;
  int m_zeroY;
  int m_zeroZ;
  const int m_deadZone=10;
  const int m_factor=63;
  unsigned char m_channelX;
  unsigned char m_channelY;
  unsigned char m_channelZ;
  int process(int raw);

public:
  Joystick (unsigned char channelX, unsigned char channelY, unsigned char channelZ);
  int readX();
  int readY();
  int readZ();
};

#endif /* JOYSTICK_H_ */
