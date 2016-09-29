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

#include <stdint.h>
#include <Arduino.h>
#include "thermistor.h"
#include "tick.h"
#include "nextion.h"

const uint8_t PIN_OIT=A12;
const uint8_t THERMISTOR_HOUSEKEEPING_INTERVAL_MS = 200;
const uint8_t THERMISTOR_HOUSEKEEPING_PHASE_MS    = 3;

/**
 * Constructor
 */
Thermistor::Thermistor(
    uint8_t  pin,
    uint16_t bias_resistor_ohms,
    uint16_t thermistor_r_25C_ohms,
    uint16_t thermistor_beta_coefficient_degK
    )
{
  _pin                              = pin;
  _bias_resistor_ohms               = bias_resistor_ohms;
  _thermistor_r_25C_ohms            = thermistor_r_25C_ohms;
  _thermistor_beta_coefficient_degK = thermistor_beta_coefficient_degK;
  pinMode(_pin, INPUT);
}

/**
 * Read the voltage from the pin and compute the temperature.
 *
 * @return thermistor temperature in 0.1 degree C.
 */
int16_t Thermistor::read_deci_degC() {
  uint16_t raw_adc_value = analogRead(_pin);
  float thermistor_ohms = _bias_resistor_ohms * ( ( 1024.0 / raw_adc_value ) - 1 );
  float steinhart = log(thermistor_ohms / _thermistor_r_25C_ohms ) / _thermistor_beta_coefficient_degK;
  steinhart += 1.0 / ( 25 + 273.15 );
  steinhart = 1 / steinhart;
  return (steinhart - 273.15) * 10;
}

/**
 * Read the voltage from the pin and compute the temperature.
 *
 * @return thermistor temperature in 0.1 degree F.
 */
int16_t Thermistor::read_deci_degF() {
  // FIXME: slight precision loss
  return (read_deci_degC() * 9 / 5) + 320;
}

/**
 * Read all thermistors and update all gauges.
 */
void Thermistor::housekeeping() {
  static Tick thermistor_housekeeping_tick(THERMISTOR_HOUSEKEEPING_INTERVAL_MS, THERMISTOR_HOUSEKEEPING_PHASE_MS);
  if (thermistor_housekeeping_tick.tocked()) {
    oit_g.val(oit_thermistor.read_deci_degF());
  }
}

//                            pin, bias, r25c, beta
Thermistor oit_thermistor(PIN_OIT,  217, 3075, 3820);
