/* Every second, except end of minute: 800ms high, followed by 100ms low.
 * Let's correlate for that.
 *
 */
#include "Arduino.h"
#include "dcf77receiver.h"

Dcf77Receiver dcf(PB6);
Chronos::DateTime localTime;

void setup()
{
  dcf.init();
  while(!Serial1);
  Serial1.begin(230400);
  Serial1.println("Started...");
}

void loop()
{

}
