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

#include <mcp_can.h>

/**
 * Handle all interaction with CAN bus.
 *
 * @todo Possibly split ADC handling to a separate class.
 */
class CanBus
{
  public:
    static void init(void);
    static void can_send_adc(
        uint16_t can_id,
        uint8_t a,
        uint8_t b,
        uint8_t c,
        uint8_t d
        );
    static void handleCANFrame(void);
    static void housekeeping(void);
};
