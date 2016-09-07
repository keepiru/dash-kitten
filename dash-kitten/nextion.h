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

/**
 *  This is an Arduino library which handles updating display objects (text,
 *  gauge, etc) on a Nextion LCD over a serial port.
 */
class NextionObject
{
  private:
    Stream *_stream;
    String _suffix,
           _id,
           _lid,
           _old_pco,                   // Store the old color to optimize repeated updates
           _old_txt;                   // Store the old text to optimize repeated updates
    uint8_t _scale,
            _decimals;
    int16_t _red_high,
            _red_low,
            _yellow_high,
            _yellow_low,
            _refresh_ms;
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

    static void init(void);
    static void refresh_labels(void);
    static void check_watchdogs(void);
    static void housekeeping(void);
};

extern NextionObject map_g,
  afr_g,
  rpm_g,
  vss_g,
  art_g,
  spk_g,
  egt_g,
  clt_g,
  mat_g,
  clk_g,
  bat_g,
  warn_g;
