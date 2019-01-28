class Shift595
{
  int m_latchPin;
  int m_clockPin;
  int m_dataPin;
  unsigned int m_state;

  void shift_state();
  
public:
  Shift595(int latchPin, int clockPin, int dataPin);
  void setBit(int bitNum);
  void clearBit(int bitNum);
  void allOn();
  void allOff();
};


