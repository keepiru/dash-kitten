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

/**
 * This is a simple LED handler.  It allows multiple sources to turn on a
 * shared LED and ensure it stays on for a specified duration.
 */
class LED
{
  private:
    uint8_t _pin;                      ///< Arduino Pin ID for this LED
    uint32_t _shutoff_time;            ///< Turn off the LED after this time, milliseconds since boot

  public:
    LED(uint8_t pin);
    void illuminate(uint32_t duration);
    void deluminate(void);
    static void housekeepingzug(void);
};

extern LED Warning_LED;
extern LED Error_LED;
