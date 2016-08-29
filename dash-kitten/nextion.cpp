/*
 *  TurboKitten Digital Dashboard
 *
 *  This is an Arduino library which handles updating display objects on a
 *  Nextion LCD over a serial port.
 *
 *  This currently only handles text objects.  The #val method will convert the
 *  value based on the _scale factor and display a fixed _decimals number of places
 *  after the point.
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
#include <SoftwareSerial.h>
#include <math.h>
#include "nextion.h"

/*
 * Wrapper for an object (text, gauge, etc) on a Nextion display
 */
NextionObject::NextionObject(
  Stream *stream,                  // The Stream object where the LCD is connected
  const char *id,                  // The name of the object on the display
  const char *lid,                 // Name of the label on the display
  const char *suffix,              // Print this after the value
  uint16_t scale,                  // Fixed decimal scaling factor
  uint8_t  decimals,               // Number of digits after the decimal
  int16_t  red_low,                // Display red if the PV goes under this
  int16_t  yellow_low,             // Display yellow if the PV goes under this
  int16_t  yellow_high,            // Display yellow if the PV goes over this
  int16_t  red_high,               // Display red if the PV goes over this
  uint16_t refresh_ms              // Minimum time between refreshes
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

/*
 *  Update the text on the object.
 */
void NextionObject::txt(
  String text                      // text - New value to show
)
{
  // Ignore update if the current value is recent enough.
  if (millis() < _last_update_time + _refresh_ms) return;
  _last_update_time = millis();

  if (_old_txt == text) return;
  _old_txt = text;
  _stream->print(_id + ".txt=\"" + text + "\"" + EOC);
}

/*
 *  Update the object's label
 */
void NextionObject::label(
  String text                      // Label text
)
{
  _stream->print(_lid + ".txt=\"" + text + "\"" + EOC);
}

/*
 *  Change the color on the object.  The change takes effect on the next refresh.
 */
void NextionObject::pco(
  String color                     // color - New color as name "RED" or RGB(5,6,5) value "63488"
)
{
  if (color != _old_pco) {         // Don't bother updating if the color is already set
    _stream->print(_id + ".pco=" + color + EOC);
  }
  _old_pco = color;
}

/*
 *  Update the text on the object with a fixed-point value.  The
 *  red/yellow lines will be checked, and the text color set if needed.
 */
void NextionObject::val(
  int32_t value                    // New value in raw, unscaled units
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
  String val_s = String(value / _scale);              // Scale after left-shift-decimal to preserve precision
  String out_s = val_s.substring(0, val_s.length() - _decimals);         // print the part left of decimal
  if (_decimals > 0) {                                                   // If we ARE printing a fraction...
    out_s += ".";                                                        // ... print the decimal ...
    out_s += val_s.substring(val_s.length() - _decimals);                // ... and the part right of decimal
  }
  out_s += _suffix;
  txt(out_s);
}

/*
 *  Update the AFR gauge's redline/yellowline based on the current AFR target.
 *  This should only be called on the AFR gauge.
 */
void NextionObject::update_afr_target(
  uint8_t afr_target               // New AFR target
)
{
  // FIXME: inline constants
  _red_low     = afr_target - 10;
  _yellow_low  = afr_target -  5;
  _yellow_high = afr_target +  5;
  _red_high    = afr_target + 10;
}

/*
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
