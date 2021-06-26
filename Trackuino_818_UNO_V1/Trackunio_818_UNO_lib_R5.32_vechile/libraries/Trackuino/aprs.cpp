/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"
#include <stdio.h>
#include <stdlib.h>
//#include <EEPROM.h>
#include <BME280I2C.h>
#include <Wire.h>

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module functions

float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}
/*
void tStatBits()
}
char string[bit] = status;
return string;
}
*/

int sequence = 0;
int txc= 0;
char tStatStrg [9];

// Exported functions
void aprs_send()
{
  char temp[12];                   // Temperature (int/ext)
  const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('!');                //  "!" Report with out timestamp, "/" Report w/ timestampno APRS messaging. $ = NMEA raw data
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
/*
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
*/
//  ax25_send_string("5054.98N");   // Fixed coordinate, GPS Format
  ax25_send_string(gps_aprs_lat);   // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_string(APRS_TABLE);       // Primary Table '/' , Alternate Table '\' 
//  ax25_send_string("11402.37W");  // Fixed coordinate, GPS format
  ax25_send_string(gps_aprs_lon);   // Lon: 000deg and 25.80 min
  ax25_send_string(APRS_SYMBOL);    // Symbole define in config file
 // ax25_send_byte('O');            // Symbol: O=balloon, >=Car, -=QTH, _= Weather
  
  snprintf(temp, 4, "%03d", (int)(gps_course + 0.5)); 
  ax25_send_string(temp);             // Course (degrees)
  ax25_send_byte('/');                // and
  snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
  ax25_send_string(temp);             // speed (knots)
  ax25_send_string("/A=");            // Altitude (meters). Goes anywhere in the comment area
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
  ax25_send_string(temp);

  txc= txc+1;
  if (txc>= 1000){txc=0;} // roll over to maintain three bytes
  ax25_send_string("/Tc");
  snprintf(temp, 4, "%03d", (txc));   //Tx count "position"
  ax25_send_string(temp);
/*
  ax25_send_string("/Ti=");
  snprintf(temp, 6, "%ld", (long) intTemp());  //BME temperature
  ax25_send_string(temp);


  ax25_send_string("/Te=");
  snprintf(temp, 6, "%d", extTemp());   //DS1280 sensor
  ax25_send_string(temp);
  

   ax25_send_string("/Vbat=");  
  snprintf(temp, 6, "%03d", sensors_bvin()/10); // solar panel voltage
  ax25_send_string(temp);
 

  ax25_send_string("/Vin=");
  snprintf(temp, 6, "%d", sensors_vin()/100);
  ax25_send_string(temp);
*/
  ax25_send_byte(' ');  
  ax25_send_string(APRS_COMMENT);     // Comment


  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
}


void aprsw_send() // weather packet
{
  char temp[12];                   // Temperature (int/ext)
  const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('@');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data, Use @ for weather "/" may work
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
  ax25_send_string("5054.83N");       // Fixed coordinate
//  ax25_send_string(gps_aprs_lat);   // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                // Symbol table
  ax25_send_string("11402.65W");      // Fixed coordinate
//  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte('_');                // Symbol: O=balloon, >=Car, -=QTH, _= Weather
  ax25_send_string(".../...");          // Blank out wind direction/speed
  ax25_send_string("g...");             // Zero wind gust
  ax25_send_byte('t');

 // bmeread(Pres, Temp, Hum);
 // snprintf(temp, 6, "%ld", (long) intTemp());  //BME temperature
  snprintf(temp, 3, "%d", extTemp(0));   //DS1280 sensor
  ax25_send_string(temp);
  ax25_send_string("r...");              // Zero rainfall data
  ax25_send_string("p...");
  ax25_send_string("P...");
  ax25_send_byte('h');                         // Hundity
  snprintf(temp, 3, "%ld", (long) intHum());  //BME Humdity
  ax25_send_string(temp);
  ax25_send_byte('b');                          // Pressure
  snprintf(temp, 5, "%ld", (long) intPres());  //BME Pressure
  ax25_send_string(temp);
  ax25_send_string(APRS_COMMENT);              // Arduino
  
  ax25_send_footer();                  // include in custom APRS data
  ax25_flush_frame();                 // Tell the modem to go
}

void aprst_send() // telemetry packet
{
  char temp[12];                   // Temperature (int/ext)
  const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('T');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data, Use @ for weather "/" may work
  ax25_send_byte('#');

  sequence = sequence + 1;   
  if(sequence>=1000) {sequence=0;} // roll over to maintian 3 bytes
  snprintf(temp, 4, "%03d", (sequence));   //Telemetry sequence #
  ax25_send_string(temp);
   ax25_send_byte(',');

  snprintf(temp, 6, "%03d", sensors_vin()/10); // Battery voltage
  ax25_send_string(temp);
  ax25_send_byte(','); 
  
  snprintf(temp, 6, "%d", extTemp(0));   //DS1280 sensor
  ax25_send_string(temp);
  ax25_send_byte(',');
/*
  snprintf(temp, 6, "%d", extTemp(1));   //DS1280 sensor
  ax25_send_string(temp);
  ax25_send_byte(',');
*/

  ax25_send_string("0,"); //blank 3rd analog  

  ax25_send_string("0,"); //blank 4th analog  

  ax25_send_string("0,"); //blank 5th analog

/*
  snprintf(temp, 3, "%d", (long) intHum());  //BME Humdity
  ax25_send_string(temp);
  ax25_send_byte(',');

  snprintf(temp, 5, "%d", (long) intPres()/100);  //BME Pressure, converted to kilopascel
  ax25_send_string(temp);
  ax25_send_byte(',');

  snprintf(temp, 6, "%d", (long) intTemp());  //BME temperature
  ax25_send_string(temp);
  ax25_send_byte(',');
*/

  ax25_send_string(tStatStrg);
  ax25_send_byte(','); 
 
 ax25_send_string(APRS_COMMENT);              // Arduino

  ax25_send_footer();                  // include in custom APRS data
   ax25_flush_frame();                 // Tell the modem to go
}
