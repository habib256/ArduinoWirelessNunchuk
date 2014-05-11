/*
 * ArduinoWirelessNunchukDemo.ino
 *
 * Copyright 2010-2014
 * Gabriel Bianconi, http://www.gabrielbianconi.com/ for the object interface idea
 * Michael Dreher great code that work with wireless Nunchuck controllers of third party vendors
 * Arnaud Verhille for mixing the two :-)  http://www.gistlabs.net
 *
 * Projects URL:
 * http://www.gabrielbianconi.com/projects/arduinonunchuk/
 * https://github.com/habib256/ArduinoWirelessNunchuk
 *
 *
 */

#include <Wire.h>
#include <ArduinoWirelessNunchuk.h>

#define BAUDRATE 9600

ArduinoWirelessNunchuk nunchuk = ArduinoWirelessNunchuk();

void setup()
{
  Serial.begin(BAUDRATE);
  nunchuk.init();
}

void loop()
{
  nunchuk.update();

  Serial.print(nunchuk.analogX, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.analogY, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelX, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelY, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelZ, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.zButton, DEC);
  Serial.print(' ');
  Serial.println(nunchuk.cButton, DEC);
}
