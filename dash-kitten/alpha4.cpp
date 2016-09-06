#include <Adafruit_MAX31855.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#define PIN_THERMO_CS 10
Adafruit_MAX31855 Thermo( PIN_THERMO_CS );

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void Alpha4::init(void)
{
  alpha4.begin( 0x70 );
}

void Alpha4::housekeeping(void)
{
  double c = Thermo.readCelsius();
  egt_g.val( (int) c );
  Serial.print( "thermocouple returned " );
  if( isnan( c ) ) {
    Serial.println( "NaN" );
  } else {
    Serial.println( c );
  }
  char buf[ 4 ];
  int intC = (int) c;
  for( int i = 0; i < 4; i++ ) {
    char val = (intC % 10) + '0';
    buf[ 3 - i ] = val;
    Serial.println( val );
    intC /= 10;
  }
  bool foundNonZero = false;
  for( int i = 0; i < 4; i++ ) {
    if( buf[ i ] == '0' ) {
      if( !foundNonZero ) {
        alpha4.writeDigitAscii( i, ' ' );
      } else {
        alpha4.writeDigitAscii( i, buf[ i ] );
      }
    } else {
      alpha4.writeDigitAscii( i, buf[ i ] );
      foundNonZero = true;
    }
  }
  alpha4.writeDisplay();
}
