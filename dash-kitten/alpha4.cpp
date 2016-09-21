#include <Adafruit_MAX31855.h>
#include <Adafruit_LEDBackpack.h>
#include "nextion.h"
#include "alpha4.h"

void Alpha4::init(void)
{
  alpha4.begin( 0x70 );
}

void Alpha4::housekeeping(void)
{
  return;
  int16_t egt_degC = Thermocouple_EGT.readCelsius();
  egt_g.val( egt_degC );

  char egt_degC_str[6];
  snprintf(egt_degC_str, 6, "%d", egt_degC);
  int8_t blank_spaces = 4 - strlen(egt_degC_str);
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
      c = egt_degC_str[i - blank_spaces];
    }
    alpha4.writeDigitAscii( i, c );
  }
  alpha4.writeDisplay();
}
