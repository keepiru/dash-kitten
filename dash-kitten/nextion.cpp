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

#define PIN_LCD_RX 19                  ///< Nextion LCD receive pin
#define PIN_LCD_TX 18                  ///< Nextion LCD transmit pin

#define HOUSEKEEPING_INTERVAL_MS 200  ///< How frequently to run watchdog, display refresh, etc
#define HOUSEKEEPING_PHASE_MS 100     ///< When to start first housekeeping run

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <math.h>
#include "dash_kitten.h"
#include "nextion.h"
#include "tick.h"

SoftwareSerial lcdstream(PIN_LCD_RX, PIN_LCD_TX);  ///< Serial stream to communicate with LCD

/**
 *  Constructor
 */
NextionObject::NextionObject(
  Stream *stream,                      ///< The Stream object where the LCD is connected
  const char *id,                      ///< The name of the object on the display
  const char *lid,                     ///< Name of the label on the display
  const char *suffix,                  ///< Print this after the value
  uint16_t scale,                      ///< Fixed decimal scaling factor
  uint8_t  decimals,                   ///< Number of digits after the decimal
  int16_t  red_low,                    ///< Display red if the PV goes under this
  int16_t  yellow_low,                 ///< Display yellow if the PV goes under this
  int16_t  yellow_high,                ///< Display yellow if the PV goes over this
  int16_t  red_high,                   ///< Display red if the PV goes over this
  uint16_t refresh_ms                  ///< Minimum time between refreshes
)
{
  _stream      = stream;
  _suffix      = suffix;
  _id          = id;
  _lid         = lid;
  _scale       = scale;
  _decimals    = decimals;
  _red_low     = red_low;
  _yellow_low  = yellow_low;
  _yellow_high = yellow_high;
  _red_high    = red_high;
  _refresh_ms  = refresh_ms;
}

/**
 *  Update the text on the object.
 */
void NextionObject::txt(
  String text                          ///< text - New value to show
)
{
  // Ignore update if the current value is recent enough.
  if (_old_txt == text && millis() < _last_update_time + _refresh_ms * 5) return;
  _last_update_time = millis();
  _old_txt = text;
  _stream->print(_id);
  _stream->print(".txt=\"");
  _stream->print(text);
  _stream->print("\"" EOC);
}

/**
 *  Update the object's label.
 */
void NextionObject::label(
  String text                          ///< Label text
)
{
  _stream->print(_lid);
  _stream->print(".txt=\"");
  _stream->print(text);
  _stream->print("\"" EOC);
}

/**
 *  Change the color on the object.  The change takes effect on the next
 *  refresh.
 */
void NextionObject::pco(
  String color                         ///< color - New color as name "RED" or RGB(5,6,5) value "63488"
)
{
  static uint32_t last_color_refresh_time_ms;
  // Don't bother updating if the color has been recently set to this value.
  if (color != _old_pco || millis() > last_color_refresh_time_ms + 5 * _refresh_ms) {
    last_color_refresh_time_ms = millis();
    _stream->print(_id);
    _stream->print(".pco=");
    _stream->print(color);
    _stream->print(EOC);
  }
  _old_pco = color;
}

/**
 *  Update the text on an object with a fixed-point value.  The red/yellow
 *  lines will be checked, and the text color set if needed.
 */
void NextionObject::val(
  int32_t value                        ///< New value in raw, unscaled units
)
{

  if (value > _red_high || value < _red_low) { // Red takes precedence
    pco("RED");
  } else if (value > _yellow_high || value < _yellow_low) {
    pco("YELLOW");
  } else {
    pco("GREEN");
  }

  // Print the value with fixed-decimal interpretation
  uint8_t i;
  for (i = 0; i < _decimals; i++) { // left-shift-decimal by _decimals (beware of overflows)
    value *= 10;
  }
  String val_s, out_s;
  val_s.reserve(12);
  out_s.reserve(12);
  val_s = value / _scale;                                        // Scale after left-shift-decimal to preserve precision
  out_s = val_s.substring(0, val_s.length() - _decimals);                // print the part left of decimal
  if (_decimals > 0) {                                                   // If we ARE printing a fraction...
    out_s += ".";                                                        // ... print the decimal ...
    out_s += val_s.substring(val_s.length() - _decimals);                // ... and the part right of decimal
  }
  out_s += _suffix;
  txt(out_s);
}

/**
 *  Update the AFR gauge's redline/yellowline based on the current AFR target.
 *  This should only be called on the AFR gauge.
 */
void NextionObject::update_afr_target(
  uint8_t afr_target               ///< New AFR target
)
{
  // FIXME: inline constants
  _red_low     = afr_target - 10;
  _yellow_low  = afr_target -  5;
  _yellow_high = afr_target +  5;
  _red_high    = afr_target + 10;
}

/**
 *  Check this gauge's freshness.  If it's expired, clear the value.
 *  No data is better than bad data.
 */
void NextionObject::watchdog()
{
  if (millis() - _last_update_time > _refresh_ms * 5) {
    pco("GRAY");
    txt("---");
  }
}

/**
 * Perform boot-time initialization of communication and ensure
 * display is in a sane state.
 */
void NextionObject::init(void)
{
  lcdstream.begin(115200);
  lcdstream.print(EOC);         // Terminate any partial command sent before boot
  warn_g.pco("RED");
  delay(1000);   // Display the kittens for an extra second
  lcdstream.print("page main0" EOC);
}

/**
 *  Refresh all the label values.
 */
void NextionObject::refresh_labels(void)
{
  map_g.label("MAP kPa");
  afr_g.label("AFR");
  rpm_g.label("RPM");
  art_g.label("AFR trgt");
  vss_g.label("VSS mph");
  spk_g.label("Advance");
  egt_g.label("EGT degF");
}

/**
 *  Check the watchdog on all gauges.
 */
void NextionObject::check_watchdogs()
{

  map_g.watchdog();
  afr_g.watchdog();
  rpm_g.watchdog();
  art_g.watchdog();
  vss_g.watchdog();
  spk_g.watchdog();
  egt_g.watchdog();
  clt_g.watchdog();
  mat_g.watchdog();
  oit_g.watchdog();
  bat_g.watchdog();
  clk_g.watchdog();
  warn_g.watchdog();
  lcdstream.print("clk.val=0" EOC);  // Reset the Nextion watchdog
}

/**
 * Read the current time from the RTC and display it.
 */
void NextionObject::update_clock() {
  DateTime dt = RTC.now();
  char buf[ 10 ];
  snprintf(buf, sizeof(buf), "%02u:%02u:%02u",
    dt.hour(), dt.minute(), dt.second());
  clk_g.txt( buf );
}

/**
 * Perform periodic maintenance: check all watchdogs and refresh
 * labels.
 */
void NextionObject::housekeeping(void)
{
  static Tick housekeeping_tick(HOUSEKEEPING_INTERVAL_MS, HOUSEKEEPING_PHASE_MS);

  if (housekeeping_tick.tocked()) {
    refresh_labels();
    check_watchdogs();
    update_clock();
  }
}

/*
 *  Set up the gauges.
 *  v0,v1,v2 are guages on the left top; l0,l1,l2 are labels to the right of v0,v1,v2;
 *  v3,v4,v5,v6 are gauges on the top right; l3,l4,l5,l6 are labels to the right of v3,v4,v5,v6;
 *  b0,b1,b2,b3 are along the bottom;
 *  warn is a full-width line of text.
 *  Redlines / yellowlines are specified in raw, unscaled units.
 */
//                                   id,  lid, suffix, scale, decimals, red_low, yellow_low, yellow_high, red_high, ref
NextionObject map_g(&lcdstream,    "v0", "l0",     "",    10,        0,       0,          0,        2600,    2800, 20),
              afr_g(&lcdstream,    "v1", "l1",     "",    10,        1,     100,        105,         150,     155, 100),
              rpm_g(&lcdstream,    "v2", "l2",     "",     1,        0,       0,          0,        6500,    7000, 100),
              vss_g(&lcdstream,    "v3", "l3",     "",     1,        0,  -32768,     -32768,       32767,   32767, 200),
              art_g(&lcdstream,    "v4", "l4",     "",    10,        1,  -32768,     -32768,       32767,   32767, 200),
              spk_g(&lcdstream,    "v5", "l5",     "",    10,        1,  -32768,     -32768,       32767,   32767, 50),
              egt_g(&lcdstream,    "v6", "l6",     "",     1,        0,     500,        700,        1600,    1800, 100),
              clt_g(&lcdstream,    "b0",   "", "cltF",    10,        0,     600,       1500,        2000,    2200, 1010),
              mat_g(&lcdstream,    "b1",   "", "matF",    10,        0,     200,        400,        1400,    1600, 200),
              oit_g(&lcdstream,    "b2",   "", "oilF",     1,        0,  -32768,     -32768,       32767,   32767, 1000),
              bat_g(&lcdstream,    "b3",   "",    "v",    10,        1,     120,        130,         147,     150, 500),
              clk_g(&lcdstream,    "c0",   "",     "",     1,        0,  -32768,     -32768,       32767,   32767, 200),
              warn_g(&lcdstream, "warn",   "",     "",     0,        0,       0,          0,           0,       0, 50);

