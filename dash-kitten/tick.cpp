/*
 *  TurboKitten Nextion display handler
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

#include <stdint.h>
#include <Arduino.h>

/*
 *  Simple periodic event scheduler
 *
 *  This is used to track when periodic tasks should run.  It does not actually
 *  run them - it just lets you know if the task is due to be run.  The
 *  interval specifies how often to run them.  The phase is used so mulitple
 *  tasks are staggered instead of all running at the same time.
 */
class Tick
{
  private:
    uint16_t _interval,      // Interval between ticks in milliseconds
             _phase;         // Time for the first tick in milliseconds since boot
    uint32_t _next_tick;     // Time for the next tick in milliseconds since boot

  public:
    Tick(
      uint16_t interval,
      uint16_t phase
    )
    {
      _interval  = interval;
      _phase     = phase;
      _next_tick = phase;    // If the system has already be up for phase ms then the timer will already be tocked.
    }

/*
 *  Check if the task is due to run.  If so, return true and schedule the next
 *  tick.  Otherwise return false.
 */
    bool tocked(void)
    {
      if (millis() > _next_tick) {
        // Determine timer's next tick.  If the system can't keep up then runs
        // will be skipped but the phase will be preserved.
        _next_tick = _phase + _interval + millis() / _interval * _interval;
        return (true);
      } else {
        return (false);
      }
    }
};

