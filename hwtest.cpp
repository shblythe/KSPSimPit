#include "Arduino.h"
#include "hwtest.h"

void hwtest_setup()
{
	Serial.begin(115200);
	Serial.println("hello");
}

void hwtest_loop()
{

}