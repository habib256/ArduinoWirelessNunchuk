/*
 * ArduinoNunchuk.cpp - Improved Wii Nunchuk library for Arduino
 *
 * Copyright 2010-2014
 * Gabriel Bianconi, http://www.gabrielbianconi.com/ for the object interface idea
 * Michael Dreher great code that work with wireless Nunchuck controllers of third party vendors
 * Arnaud Verhille for mixing the two :-)
 *
 * Projects URL:
 * http://www.gabrielbianconi.com/projects/arduinonunchuk/
 * https://github.com/habib256/ArduinoWirelessNunchuk
 *
 * Based on the following resources:
 *   http://www.windmeadow.com/node/42
 *   http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *   http://wiibrew.org/wiki/Wiimote/Extension_Controllers
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include "ArduinoWirelessNunchuk.h"


void ArduinoWirelessNunchuk::init()
{

// power supply of the Nunchuck via port C2 and C3
#ifdef POWER_VIA_PORT_C2_C3
  PORTC &= ~ _BV(PORTC2);
  PORTC |= _BV(PORTC3);
  DDRC |= _BV(PORTC2) | _BV(PORTC3); // make outputs
  delay(100); // wait for things to stabilize
#endif

  Wire.begin();// initialize i2c

  // we need to switch the TWI speed, because the nunchuck uses Fast-TWI
  // normally set in hardware\libraries\Wire\utility\twi.c twi_init()
  // this is the way of doing it without modifying the original files
#define TWI_FREQ_NUNCHUCK 400000L
  TWBR = ((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;

  if (!ArduinoWirelessNunchuk::nunchuck_init(0)) { // write the initialization handshake
    //Serial.println("\nInitializing with The New Way ... OK");
  } else {
    //Serial.println("\nInitializing with The New Way ... Failed");
  }

#ifndef USE_NEW_WAY_INIT
  //Serial.println("\nInitializing with The Old Way");
#endif

  // display the identification bytes, must be "00 00 A4 20 00 00" for the Nunchuck
  byte i;
  if (ArduinoWirelessNunchuk::readControllerIdent(outbuf) == 0)
  {
   // Serial.print("Ident=");
    for (i = 0; i < WII_TELEGRAM_LEN; i++)
    {
     // Serial.print(outbuf[i], HEX);
     // Serial.print(' ');
    }
    //Serial.println();
  }
  //Serial.println("Finished setup");
  delay(10);

  ArduinoWirelessNunchuk::update();
}



void ArduinoWirelessNunchuk::update()
{
  Wire.requestFrom (WII_NUNCHUCK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuck

  for (cnt = 0; (cnt < WII_TELEGRAM_LEN) && Wire.available (); cnt++)
  {
    outbuf[cnt] = ArduinoWirelessNunchuk::nunchuk_decode_byte (Wire.read ()); // read byte as an integer
  }

  ArduinoWirelessNunchuk::clearTwiInputBuffer();

  // If we recieved the 6 bytes, then go print them
  if (cnt >= WII_TELEGRAM_LEN)
  {
    ArduinoWirelessNunchuk::analogX = outbuf[0];
    ArduinoWirelessNunchuk::analogY = outbuf[1];
    ArduinoWirelessNunchuk::accelX = outbuf[2] * 2 * 2;
    ArduinoWirelessNunchuk::accelY = outbuf[3] * 2 * 2;
    ArduinoWirelessNunchuk::accelZ = outbuf[4] * 2 * 2;

    // byte outbuf[5] contains bits for z and c buttons
    // it also contains the least significant bits for the accelerometer data
    // so we have to check each bit of byte outbuf[5]

    if ((outbuf[5] >> 0) & 1)
    {
      ArduinoWirelessNunchuk::zButton = 0;
    } else {
      ArduinoWirelessNunchuk::zButton = 1;
    }
    if ((outbuf[5] >> 1) & 1)
    {
      ArduinoWirelessNunchuk::cButton = 0;
    } else {
      ArduinoWirelessNunchuk::cButton = 1;
    }

    if ((outbuf[5] >> 2) & 1)
    {
      ArduinoWirelessNunchuk::accelX += 2;
    }
    if ((outbuf[5] >> 3) & 1)
    {
      ArduinoWirelessNunchuk::accelX += 1;
    }

    if ((outbuf[5] >> 4) & 1)
    {
      ArduinoWirelessNunchuk::accelY += 2;
    }
    if ((outbuf[5] >> 5) & 1)
    {
      ArduinoWirelessNunchuk::accelY += 1;
    }

    if ((outbuf[5] >> 6) & 1)
    {
      ArduinoWirelessNunchuk::accelZ += 2;
    }
    if ((outbuf[5] >> 7) & 1)
    {
      ArduinoWirelessNunchuk::accelZ += 1;
    }
  }

  ArduinoWirelessNunchuk::send_zero (); // write the request for next bytes
  delay (20);
}


// params:
// timeout: abort when timeout (in ms) expires, 0 for unlimited timeout
// return: 0 == ok, 1 == timeout
byte ArduinoWirelessNunchuk::nunchuck_init (unsigned short timeout)
{
  byte rc = 1;

#ifndef USE_NEW_WAY_INIT
  // look at <http://wiibrew.org/wiki/Wiimote#The_Old_Way> at "The Old Way"
  Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
  Wire.write (byte(0x40)); // writes memory address
  Wire.write (byte(0x00)); // writes sent a zero.
  Wire.endTransmission (); // stop transmitting
#else
  // disable encryption
  // look at <http://wiibrew.org/wiki/Wiimote#The_New_Way> at "The New Way"

  unsigned long time = millis();
  do
  {
    Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
    Wire.write (byte(0xF0)); // writes memory address
    Wire.write (byte(0x55)); // writes data.
    if (Wire.endTransmission() == 0) // stop transmitting
    {
      Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
      Wire.write (byte(0xFB)); // writes memory address
      Wire.write (byte(0x00)); // writes sent a zero.
      if (Wire.endTransmission () == 0) // stop transmitting
      {
        rc = 0;
      }
    }
  }
  while (rc != 0 && (!timeout || ((millis() - time) < timeout)));
#endif

  return rc;
}

// params:
// ident [out]: pointer to buffer where 6 bytes of identification is stored. Buffer must be at least 6 bytes long.
// A list of possible identifications can be found here: <http://wiibrew.org/wiki/Wiimote#The_New_Way>
// return: 0 == ok, 1 == error
byte ArduinoWirelessNunchuk::readControllerIdent(byte* pIdent)
{
  byte rc = 1;

  // read identification
  Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
  Wire.write (0xFA); // writes memory address of ident in controller
  if (Wire.endTransmission () == 0) // stop transmitting
  {
    byte i;
    Wire.requestFrom (WII_NUNCHUCK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuck
    for (i = 0; (i < WII_TELEGRAM_LEN) && Wire.available (); i++)
    {
      pIdent[i] = Wire.read(); // read byte as an integer
    }
    if (i == WII_TELEGRAM_LEN)
    {
      rc = 0;
    }
  }
  return rc;
}

void ArduinoWirelessNunchuk::clearTwiInputBuffer(void)
{
  // clear the read buffer from any partial data
  while ( Wire.available ())
    Wire.read ();
}

void ArduinoWirelessNunchuk::send_zero ()
{
  // I don't know why, but it only works correct when doing this exactly 3 times
  // otherwise only each 3rd call reads data from the controller (cnt will be 0 the other times)
  for (byte i = 0; i < 3; i++)
  {
    Wire.beginTransmission (WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
    Wire.write (byte(0x00)); // writes one byte
    Wire.endTransmission (); // stop transmitting
  }
}


char ArduinoWirelessNunchuk::nunchuk_decode_byte (char x)
{
#ifndef USE_NEW_WAY_INIT
  x = (x ^ 0x17) + 0x17;
#endif
  return x;
}

