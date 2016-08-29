/*
 *  TurboKitten Digital Dashboard
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

#include <Arduino.h>
#include <stdint.h>
#include "led.h"

/**
 * Constructor
 */
LED::LED(
  uint8_t pin                          ///< Arduino Pin ID for this LED
  )
{
  _pin = pin;
}

/**
 * Turn on the LED for a specified duration.  Turning it back off happens in
 * housekeeping.
 */
void LED::illuminate(
    uint32_t duration                  ///< How long to keep the LED lit, milliseconds.
    )
{
  _shutoff_time = max(_shutoff_time, millis() + duration);
  digitalWrite(_pin, HIGH);
}

/**
 * Turn off LED if the shutoff time has passed.
 */
void LED::housekeeping(void)
{
  if ((int32_t) _shutoff_time - millis() < 0) {  // wrap-safe comparison
    digitalWrite(_pin, LOW);
    _shutoff_time = millis();                    // prevent accidental-on after 2**31 milliseconds
  }
}
