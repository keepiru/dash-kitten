/**
 *  TurboKitten Digital Dashboard
 *
 *  This is an Arduino sketch which receives data from a MegaSquirt via CAN BUS
 *  and displays it on a Nextion LCD panel.
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

#include <mcp_can.h>
#include "canbus.h"
#include "dash_kitten.h"
#include "nextion.h"
#include "tick.h"

#define PIN_CAN_CS 9                   ///< chip select
#define PIN_CAN_INT 2                  ///< interrupt
//#define DEBUG_CAN_ID 1521            ///< If defined, dump frames sent to this CAN id

#define ADC_PAGE_1_INTERVAL_MS 50      ///< How frequently to transmit ADC Page 1 data
#define ADC_PAGE_1_PHASE_MS 15         ///< When to start first ADC Page 1 transmission
#define ADC_PAGE_1_ID 0x20             ///< Destination CAN ID for Page 1

#define ADC_PAGE_2_INTERVAL_MS 50      ///< How frequently to transmit ADC Page 2 data
#define ADC_PAGE_2_PHASE_MS 25         ///< When to start first ADC Page 2 transmission
#define ADC_PAGE_2_ID 0x21             ///< Destination CAN ID for Page 2

#define MS3_RTC_REQ_ADDR 28869304
#define MS3_RTC_WRITE_ADDR 644

MCP_CAN CAN0(PIN_CAN_CS);              ///< CAN BUS transceiver to communicate with ECU

/**
 * Boot-time initialization of CAN bus interface.
 */
void CanBus::init(void)
{
  CAN0.begin(CAN_500KBPS);             // Assumes module with 16MHz clock - use CAN_1000KBPS for 8MHz clock.
}

/**
 *  Sample ADCs and transmit them to CAN
 */
void CanBus::can_send_adc(
  uint16_t can_id,                     ///< CAN ID to send sample data
  uint8_t a,                           ///< Analog pin ID
  uint8_t b,                           ///< Analog pin ID
  uint8_t c,                           ///< Analog pin ID
  uint8_t d                            ///< Analog pin ID
  )
{
  uint16_t samples[4];
  samples[0] = htons(analogRead(a));
  samples[1] = htons(analogRead(b));
  samples[2] = htons(analogRead(c));
  samples[3] = htons(analogRead(d));
  CAN0.sendMsgBuf(can_id, 0, 8, (byte *) &samples);
}

DateTime dateTimeFromCan( uint8_t *rxBuf, int len ) {
  // the MS3 sends us 8 bytes, but the last one is always zero (should
  // be the century).
  DateTime ret( bcd_to_int( rxBuf[ 6 ] ) + 2000, // year
      bcd_to_int( rxBuf[ 5 ] ),                   // month
      bcd_to_int( rxBuf[ 4 ] ),                   // day
      bcd_to_int( rxBuf[ 2 ] ),                   // hour
      bcd_to_int( rxBuf[ 1 ] ),                   // minute
      bcd_to_int( rxBuf[ 0 ] ) );                 // second
  return ret;
}

void CanBus::can_send_rtc(
  DateTime * dt
  )
{
  uint8_t txBuf[8];
  
  txBuf[ 0 ] = dt->second();
  txBuf[ 1 ] = dt->minute();
  txBuf[ 2 ] = dt->hour();
  txBuf[ 3 ] = dt->dayOfWeek();
  txBuf[ 4 ] = dt->day();
  txBuf[ 5 ] = dt->month();
  txBuf[ 6 ] = dt->year() >> 8;
  txBuf[ 7 ] = dt->year() & 0xff;

  CAN0.sendMsgBuf( 0x9352838, 1, 8, txBuf );
}

/**
 *  Receive and process an incoming CAN frame.  Any known ids will be
 *  decoded and sent to the LCD.
 */
void CanBus::handleCANFrame(void)
{
  uint8_t len,                         ///< Number of bytes received
          rxBuf[8];                    ///< CAN BUS receive buffer
  uint32_t rxId,                       ///< CAN BUS device/arbitration ID
           val;                        ///< Temp buffer to decode a particular value

  CAN0.readMsgBuf(&len, rxBuf);
  rxId = CAN0.getCanId();

#ifdef DEBUG_CAN_ID
  if (rxId == DEBUG_CAN_ID) {
    Serial.print(rxId);
    Serial.print( F( " len: " ) );
    Serial.print( len );
    Serial.print( F( " data: " ) );
    uint8_t i;
    for (i = 0; i < len; i++) {
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    if( CAN0.isRemoteRequest() ) {
      Serial.print( " RR" );
    }
    if( CAN0.isExtendedFrame() ) {
      Serial.print( " X" );
    }
    Serial.println(" !");
  }
#endif

  switch (rxId) {
    case MS3_RTC_REQ_ADDR:
      {
        DateTime dt = RTC.now();
        can_send_rtc( &dt );
      }
      break;

    case MS3_RTC_WRITE_ADDR:
      {
        DateTime dt = dateTimeFromCan( rxBuf, len );
        RTC.adjust( dt );
        Serial.print( F( "RTC updated from MS3" ) );
      }
      break;

    case 1520:
      rpm_g.val(           ntohs(*( int16_t *) &rxBuf[6]));  // RPM 1rpm
      break;

    case 1521:
      spk_g.val( (int16_t) ntohs(*( int16_t *) &rxBuf[0]));  // SPK total advance 0.1deg
      art_g.val(                                rxBuf[4] );  // AFR target 0.1 ratio
      // Update the red/yellow lines for the AFR gauge.  Note that this frame doesn't update
      // the measured AFR, so there may be a slight flicker if the target changes rapidly.
      afr_g.update_afr_target(rxBuf[4]);
      break;

    case 1522:
      map_g.val(           ntohs(*(uint16_t *) &rxBuf[2]));  // MAP 0.1 kPa
      mat_g.val( (int16_t) ntohs(*( int16_t *) &rxBuf[4]));  // MAT 0.1degF
      clt_g.val( (int16_t) ntohs(*( int16_t *) &rxBuf[6]));  // CLT 0.1 degF, cast for sign
      break;

    case 1523:
      // TPS offset 0 int16_t 0.1pct
      bat_g.val(           ntohs(*(uint16_t *) &rxBuf[2]));  // BAT 0.1 volt
      afr_g.val(                                rxBuf[5] );  // AFR 0.1 ratio
      break;

    case 1524:
      // Knock input offset 0 uin16_t 0.1pct
      break;

    case 156583992: // WB tx from MS
    case 33920:     // WB tx from MS
    case 2131072:   // WB rx from WB
      // WB EGO
      break;

    case 1533:
    case 1537:
    case 1538:
      // more MS3 stuff
      break;

    case 1542:
      egt_g.val( (int16_t) ntohs(*( int16_t *) &rxBuf[0]));  // EGT 0.1 degF  (or degC ?), cast for sign
      break;

    case 1562:
      // FIXME: suspicious inline constant conversion factor
      // FIXME: floats are evil
      vss_g.val(           ntohs(*(uint16_t *) &rxBuf[0]) / 4.4);  // VSS unknown unit
      break;

    case 1571:
      // Ports: A, B, EH, K, MJ, P, T, CEL_err
      break;

    case 1572:
      val =                ntohs(*(uint16_t *) &rxBuf[2]) ;    // Knock retard 0.1deg
      if (val > 0)
        warn_g.txt("KNOCK");
      else
        warn_g.txt("");
      break;

    default:
      Serial.print( F( "unrecognized can id 0x" ) );
      Serial.print( rxId, HEX );
      Serial.print( " (" );
      Serial.print( rxId );
      Serial.println( ")" );
      break;
  }
}

/**
 * Perform periodic tasks for CanBus:  transmit ADC values; pick up
 * and process any waiting CAN frames.
 */
void CanBus::housekeeping(void)
{
  static Tick adc_page_1_tick(    ADC_PAGE_1_INTERVAL_MS,   ADC_PAGE_1_PHASE_MS),
              adc_page_2_tick(    ADC_PAGE_2_INTERVAL_MS,   ADC_PAGE_2_PHASE_MS);

  if (adc_page_1_tick.tocked()) {
    can_send_adc(ADC_PAGE_1_ID, A0, A1, A2, A3);
  }

  if (adc_page_2_tick.tocked()) {
    can_send_adc(ADC_PAGE_2_ID, A4, A5, A6, A7); // Note that A6 and A7 don't physically exist on DIP28-based AVRs
  }

  if (!digitalRead(PIN_CAN_INT)) { // CAN frame waiting in receive buffer
    handleCANFrame();
  }
}
