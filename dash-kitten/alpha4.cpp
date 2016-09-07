#include <Adafruit_MAX31855.h>
#include <Adafruit_LEDBackpack.h>
#include "nextion.h"
#include "alpha4.h"

#define MS3_RTC_REQ_ADDR 28869304
#define PIN_THERMO_CS 10

Adafruit_MAX31855 Thermo( PIN_THERMO_CS );

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void Alpha4::init(void)
{
  alpha4.begin( 0x70 );
}

void Alpha4::housekeeping(void)
{
  int16_t egt_degC = Thermo.readCelsius();
  egt_g.val( egt_degC );

  String egt_degC_str(egt_degC);
  int8_t blank_spaces = 4 - egt_degC_str.length();
  for( int i = 0; i < 4; i++ ) {
    char c;
    if (blank_spaces < 0) {
      // If the EGT is really > 9999 you probably have bigger problems than the display.  However,
      // we'll handle it in case of a sensor error, etc.
      c = '-';
    } else if (i < blank_spaces) {
      // Prevent leading zeroes.
      c = ' ';
    } else {
      c = egt_degC_str.charAt(i - blank_spaces);
    }
    alpha4.writeDigitAscii( i, c );
  }
  alpha4.writeDisplay();
}
