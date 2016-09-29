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
 * Abstraction for a thermistor.  Given the configured parameters the voltage
 * on an analog pin can be read and mapped to a temperature.
 */
class Thermistor
{
  private:
    uint8_t _pin;                                ///< Arduino Pin ID for this thermistor
    uint16_t _bias_resistor_ohms,                ///< Value of bias resistor in ohms
             _thermistor_r_25C_ohms,             ///< Thermistor characteristic R(25C) in ohms
             _thermistor_beta_coefficient_degK;  ///< Thermistor beta coefficient in degrees Kelvin
  public:
    Thermistor(
        uint8_t pin,
        uint16_t bias_resistor_ohms,
        uint16_t thermistor_r_25C_ohms,
        uint16_t thermistor_beta_coefficient_degK
        );
    int16_t read_deci_degC(void);
    int16_t read_deci_degF(void);
    static void housekeeping(void);
};

extern Thermistor oit_thermistor;
