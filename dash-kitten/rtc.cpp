#include <Wire.h>
#include <RTClib.h>
#include <RTC_DS3231.h>

RTC_DS3231 RTC;

#define MS3_RTC_REQ_ADDR 28869304
#define MS3_RTC_WRITE_ADDR 644

U8
u8FromBcd( char bcd ) {
  return U8( ((bcd >> 4) * 10) + (bcd & 0xf));
}

U8 intToBcd( U16 in ) {
  return( (in % 10) | (((in % 100) / 10) << 4) );
}

void RTC::init(void)
{
  RTC.begin();

  if( !RTC.isrunning() ) {
    Serial.println( F( "RTC is NOT running!" ) );
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust( DateTime( __DATE__, __TIME__ ));
  }

  RTC.BBSQWEnable( false );
  RTC.enable32kHz( true );
  RTC.enable32kHz( false );

  char datastr[100];
  RTC.getControlRegisterData( datastr[0]  );
  Serial.print( F( "control register data for RTC: " ) );
  Serial.println( datastr );

  DateTime now = RTC.now();
  Serial.print( F( "RTC is currently:" ) );
  printDateTime( &now );
  printRtcTemp();

  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  digitalWrite( A4, HIGH );     // pull them up so we can use for I2C
  digitalWrite( A5, HIGH );     // ditto
  Wire.begin();
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();
}

void
txRtcFrame( DateTime * dt ) {
  U8 txBuf[ 8 ];
  static int first = 1;

  if( first ) {
    Serial.print( F( "sending: " ) );
    Serial.print( dt->hour() );
    Serial.print( ":" );
    Serial.print( dt->minute() );
    Serial.print( ":" );
    Serial.print( dt->second() );
    Serial.print( " " );
    Serial.print( dt->month() );
    Serial.print( "/" );
    Serial.print( dt->day() );
    Serial.print( "/" );
    Serial.print( dt->year() );
    Serial.print( " (" );
    Serial.print( dt->dayOfWeek() );
    Serial.println( ")");
  }

  txBuf[ 0 ] = dt->second();
  txBuf[ 1 ] = dt->minute();
  txBuf[ 2 ] = dt->hour();
  txBuf[ 3 ] = dt->dayOfWeek();
  txBuf[ 4 ] = dt->day();
  txBuf[ 5 ] = dt->month();
  int year = dt->year();
  txBuf[ 6 ] = (year) >> 8;
  txBuf[ 7 ] = (year) & 0xff;

  if( first ) {
    int i;
    for( i = 0; i < 8; i++ ) {
      Serial.print( txBuf[ i ], HEX );
      Serial.print( " " );
    }
    Serial.println();
    first = 0;
  }

  //  CAN0.sendMsgBuf( 28869304, 1, 8, txBuf );
  CAN0.sendMsgBuf( 0x9352838, 1, 8, txBuf );
}



void
printDateTime( DateTime * dt ) {
  Serial.print( dt->year(), DEC );
  Serial.print( '/');
  Serial.print( dt->month(), DEC );
  Serial.print( '/');
  Serial.print( dt->day(), DEC );
  Serial.print( ' ');
  Serial.print( dt->hour(), DEC) ;
  Serial.print( ':');
  Serial.print( dt->minute(), DEC );
  Serial.print( ':');
  Serial.print( dt->second(), DEC );
  Serial.println();

  Serial.print( F( " since midnight 1/1/1970 = " ) );
  Serial.print( dt->unixtime() );
  Serial.print( "s = " );
  Serial.print( dt->unixtime() / 86400L) ;
  Serial.println( "d" );
}


void
rtcTimeForDisplay( DateTime * dt, char * buf ) {
  int h = dt->hour();
  int m = dt->minute();
  int s = dt->second();

  int i = 0;
  buf[ i++ ] = ( h / 10 ) + '0';
  buf[ i++ ] = ( h % 10 ) + '0';
  buf[ i++ ] = ':';
  buf[ i++ ] = ( m / 10 ) + '0';
  buf[ i++ ] = ( m % 10 ) + '0';
  buf[ i++ ] = ':';
  buf[ i++ ] = ( s / 10 ) + '0';
  buf[ i++ ] = ( s % 10 ) + '0';
  buf[ i++ ] = 0;
}

void
displayTime() {
  DateTime dt = RTC.now();
  char buf[ 10 ];
  rtcTimeForDisplay( &dt, buf );
  time_g.txt( buf );
}

void
printRtcTemp() {
  RTC.forceTempConv( true );
  float temp_float = RTC.getTempAsFloat();
  int16_t temp_word = RTC.getTempAsWord();
  int8_t temp_hbyte = temp_word >> 8;
  int8_t temp_lbyte = temp_word &= 0x00FF;

  //Display temps
  Serial.print( F( "Temp as float: " ) );
  Serial.print( temp_float, DEC );
  Serial.println();
  Serial.print( F( "Temp as word: " ) );
  Serial.print( temp_hbyte, DEC );
  Serial.print( "." );
  Serial.print( temp_lbyte, DEC );
  Serial.println();
}

DateTime
dateTimeFromCan( U8 *rxBuf, int len ) {
  // the MS3 sends us 8 bytes, but the last one is always zero (should
  // be the century).

  DateTime ret( u8FromBcd( rxBuf[ 6 ] ) + 2000,
      u8FromBcd( rxBuf[ 5 ] ),
      u8FromBcd( rxBuf[ 4 ] ),
      u8FromBcd( rxBuf[ 2 ] ),
      u8FromBcd( rxBuf[ 1 ] ),
      u8FromBcd( rxBuf[ 0 ] ) );
  return ret;
}
