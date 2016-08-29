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

#define EOC "\xff\xff\xff" // Nextion end of command
class NextionObject
{
  private:
    Stream *_stream;                   // Stream object where the LCD is connected
    String _suffix,                    // Print this after the value
           _id,                        // Name of the object on the display
           _lid,                       // Name of the label on the display
           _old_pco,                   // Store the old color to optimize repeated updates
           _old_txt;                   // Store the old text to optimize repeated updates
    uint8_t _scale,                    // Fixed decimal scaling factor
            _decimals;                 // Number of digits to display after the decimal
    int16_t _red_high,                 // Redline (top)
            _red_low,                  // Redline (bottom)
            _yellow_high,              // Yellowline (top)
            _yellow_low,               // Yellowline (bottom)
            _refresh_ms;               // Minimum time between refreshes
    uint32_t _last_update_time;

  public:
    NextionObject(
        Stream *stream,
        const char *id,
        const char *lid,
        const char *suffix,
        uint16_t scale,
        uint8_t  decimals,
        int16_t  red_low,
        int16_t  yellow_low,
        int16_t  yellow_high,
        int16_t  red_high,
        uint16_t refresh_ms
        );
    void txt(String text);
    void label(String text);
    void pco(String color);
    void val(int32_t value);
    void update_afr_target(uint8_t afr_target);
    void watchdog(void);
};
