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
#include "led.h"
#include "tick.h"

static const uint8_t PIN_WARNING                      = 11; ///< Warning LED pin number
static const uint8_t PIN_ERROR                        = 12; ///< Error LED pin number
static const uint8_t LED_HOUSEKEEPING_INTERVAL_MS     = 20; ///< How often to check for turn-off
static const uint8_t LED_HOUSEKEEPING_PHASE_MS        =  1; ///< Turn-off phase

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
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

/**
 * Perform periodic tasks for all LEDs.  Specifically, ensure they are turned
 * off after their illumination time has passed.
 */
void LED::housekeepingzug(void)
{
  static Tick led_housekeeping_tick(LED_HOUSEKEEPING_INTERVAL_MS, LED_HOUSEKEEPING_PHASE_MS);

  if (led_housekeeping_tick.tocked()) {
    Warning_LED.deluminate();
    Error_LED.deluminate();
  }
}

/**
 * Turn off LED if the shutoff time has passed.
 */
void LED::deluminate(void)
{
  if ((int32_t) _shutoff_time - (int32_t) millis() < 0) {  // wrap-safe comparison
    pinMode(_pin, INPUT);
    digitalWrite(_pin, HIGH);
    _shutoff_time = millis();                    // prevent accidental-on after 2**31 milliseconds
  }
}

LED Warning_LED(PIN_WARNING);
LED Error_LED(PIN_ERROR);
