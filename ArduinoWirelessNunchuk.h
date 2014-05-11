/*
 * ArduinoNunchuk.h - Improved Wii Nunchuk library for Arduino
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

#ifndef ArduinoWirelessNunchuk_H
#define ArduinoWirelessNunchuk_H

// adapt to your hardware config
// use port pins port C2 and C3 as power supply of the Nunchuck (direct plug using wiichuck adapter)
//#define POWER_VIA_PORT_C2_C3 0 

#ifndef CPU_FREQ
#define CPU_FREQ 16000000L
#endif

#define USE_NEW_WAY_INIT 1 // use "The New Way" of initialization <http://wiibrew.org/wiki/Wiimote#The_New_Way>
#define WII_IDENT_LEN ((byte)6)
#define WII_TELEGRAM_LEN ((byte)6)
#define WII_NUNCHUCK_TWI_ADR ((byte)0x52)

#include <Arduino.h>
#include <Wire.h>

class ArduinoWirelessNunchuk
{
public:

    int analogX;
    int analogY;
    int accelX;
    int accelY;
    int accelZ;
    int zButton;
    int cButton;

    void init();
    void update();

private:
    uint8_t outbuf[WII_TELEGRAM_LEN]; // array to store arduino output
    int cnt;

    byte nunchuck_init (unsigned short timeout);
    byte readControllerIdent(byte* pIdent);
    void clearTwiInputBuffer(void);
    void send_zero ();
    char nunchuk_decode_byte (char x);
};

#endif
