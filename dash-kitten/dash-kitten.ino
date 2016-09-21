/**
 *  TurboKitten Digital Dashboard
 *
 *  This is an Arduino sketch which receives data from a MegaSquirt via CAN BUS
 *  and displays it on a Nextion LCD panel.
 *
 *  Copyright (C) 2015-2016  Chris "Kai" Frederick
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "alpha4.h"
#include "led.h"
#include "nextion.h"
#include "canbus.h"
#include "dash_kitten.h"
#include <Wire.h>

Alpha4 a4;

/**
 *  Boot-time initialization.
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Boot");

  Wire.begin();
  Serial.println("1");
  RTC.begin();
  Serial.println("2");
  if( !RTC.isrunning() ) {
    Serial.println( F( "RTC is NOT running!  Set time via MS3." ) );
  }
  Serial.println("3");

  a4.init();
  Serial.println("4");
  CanBus::init();
  Serial.println("5");
  NextionObject::init();
  Serial.println("6");
  digitalWrite(13, HIGH);
}

/**
 *  The main loop
 */
void loop()
{
//  Serial.println("loop");
  NextionObject::housekeeping();
  CanBus::housekeeping();
  LED::housekeepingzug();
  a4.housekeeping();
}
