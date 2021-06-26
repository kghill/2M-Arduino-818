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
 * 
 * Rev 7: Enhanced power managment. Added functions are:
 * valid_pos: used to skip Tx when invalid position.
 * frame2Tx: used to skip Tx when a frame is not Tx. Used in conjunction with spacing frames
 * power down moved  to end and after APRS flush; lets packet to finish.
 * Prvious power mangment features; extended reporting & power up & down logic.
 * 
 * Rev 8: Added Adaptive Beaconing
 */

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif

// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "aprs.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"

#include <SoftwareSerial.h>

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

//////////////////////////////////////////////////////////////////////////
float battVolt; //Battery Volatage
int frame = 4; //a frame allocates when and which frame to be sent.

bool frame2Tx= true;// determine if there is a need to Tx or skip
int valid_pos;
/*
// Single 1s batt Power Managment parameters used with Pro Mini board, run as long as possible scheme
#define BattWait 3.5   // Sleep mode, wait for battery to recharge, recommended UNO level 7v to 12 v
#define battLow  3.68  // switch to low power at this value,just below discharge knee
#define spvoltLow 4.0  // switch to extended APRS period, minium volatage of linear charger
#define BattHigh 4.5   // Lithium battery protection.
*/

// parameters used with Uno board  /////////////////////////////////////////////
// Uno configuration uses the DC jack for power; voltage sensor is on the 2M board (A2) 
#define battWait 7.0  // Sleep mode, wait for battery to recharge, recommended Uno level 7v to 12 v
#define battLow  7.5  // switch to low power when below this value
#define battHigh 14.0 // switch to low power to prevent over heating when connected to 12v battery


// Adaptive beaconing //////////////////////////////////////////////////////////

static int32_t secs_since_beacon= 0;
static int32_t last_beacon = 0;  

int slow_speed=     10;   // speed below which you are effectively still (mph)
int slow_aprs_rate= 600; // how often to transmit when still (600 secs= 10 min, 1200 secs= 20 min)

int fast_speed=     60;   // speed above for fast rate
int fast_aprs_rate= 60;  // fastest transmit rate (secs)

int turn_angle=    10; // minium turn angle
int turn_slope=    26;
int turn_time=     30; // lower setting caputres turns at lights in the city
int turn_threshold= 0;

int beacon_rate=    0;

//Parameters for course change detection
int old_heading_chg=   0; //
int heading_chg=   0;
///////////////////////////////////////////////////////////////////////////////

SoftwareSerial dra_serial(DRA_RXD, DRA_TXD);    //for 818 transceiver module

void setup()
{
  pinMode(LED_PIN, OUTPUT);// same pin as DRA_TXD
  pin_write(LED_PIN, LOW);
/*
  pinMode(vinSwitch, OUTPUT);
  pin_write(vinSwitch, HIGH); // Power on Vin pin
*/  
  dra_serial.begin(9600);

  Serial.begin(GPS_BAUDRATE);

  afsk_setup();
  gps_setup();
  sensors_setup();

// Telemetry Status, sometimes telemetry packet takes awhile to come from.
  tStatStrg[0] ='0';
  tStatStrg[1] ='0';
  tStatStrg[2] ='0';
  tStatStrg[3] ='0'; 
  tStatStrg[4] ='0'; 
  tStatStrg[5] ='0'; //Beaconing- not/adaptive beaconing in play
  tStatStrg[6] ='1'; // aprs rate over ride bc of high/low battery
  tStatStrg[7] ='0';// Low/High Power status bc of high/low battery
  tStatStrg[8] = '\0';// terminate string 

// Do not start until we get a valid time reference
  // for a slotted transmissions.
  if (APRS_SLOT >= 0) {  // remove "=" for unslotted
    do {
      while (! Serial.available())
      //delay(100); //either power_save or delay
      power_save();
    } while (! gps_decode(Serial.read()));
    
    next_aprs = millis() + 1000 *
      (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else {
    next_aprs = millis();
  }  
}

void get_pos()
{
  // Get a valid position from the GPS
  valid_pos = 0;
  uint32_t timeout = millis();
  gps_reset_parser();

  do {
        if (Serial.available())
            valid_pos = gps_decode(Serial.read());
  } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;
}
/////Adaptive Beaconing////////////////////////////////////////////////////
void heading_change()
{
    heading_chg= gps_course - old_heading_chg; // turning east is postive, west is negative
    heading_chg= abs(heading_chg); // create positve numbers only
     if (heading_chg > 180){
     heading_chg= 360 - heading_chg; 
     }
}
   
void loop()
{
  get_pos();

/// Adaptive beaconing- Variant of  http://www.ciinet.org/paul/aprs.html  ////////////////////

secs_since_beacon= ((int32_t)((millis()-last_beacon)/1000L));

 if (gps_speed < slow_speed) {// Still
  beacon_rate= slow_aprs_rate;
  tStatStrg[5] ='0';// Still,
 }
   else {
    if (gps_speed> fast_speed){// moving
      beacon_rate= fast_aprs_rate;
      tStatStrg[5] ='0';// Not
    }
    else{ // adjust beacon rate according to speed
      beacon_rate= (fast_aprs_rate * (fast_speed/gps_speed));
      tStatStrg[5] ='1';// Moving, adaptive beaconing in play
    }

// Course Change////////////////////////////////////////////////////
    heading_change();                                           // Corner pegging
    turn_threshold = turn_angle + turn_slope/(gps_speed+1);    // turn threshold speed-dependent
    if ((heading_chg > turn_threshold)&&(secs_since_beacon > turn_time)) {
      secs_since_beacon= beacon_rate; // beacon now
      tStatStrg[5] ='1';//Turning, adaptive beaconing in play
      } 
    }
   
if ( secs_since_beacon >= beacon_rate){next_aprs= millis();}  // beacon in reasonable time in all conditions speed/course change

////////////////////////////////////////////////////////////////////////
//Serial.print (battVolt); Serial.println (millis());
  battVolt = sensors_vin(); battVolt=battVolt/1000;
  
// if it is time and battery volatage sufficent report
  if (((int32_t) (millis() - next_aprs) >= 0) && ( battVolt > battWait )){ 

/*
// Start Transmission
Serial.print ("Frame: "); Serial.println (frame);
Serial.print ("Tx?: "); Serial.println (frame2Tx);
Serial.print ("Pos: "); Serial.println (valid_pos);
*/
//////////////////////////////////////////////////////////////////////////
 
/*    
 * Next two lines:
 * Normal: When lock(valid_pos), Tx and advance a frame
 * Invalid lock, by-passes all the Tx & frame logic- part of power managment
 * Invalid lock caused by a expired 2 second timer in get_pos 
*/
//   get_pos(); 
    if (valid_pos) {// Tx on lock (valid_pos), otherwise wait for lock, and do not advance a frame
    if (frame2Tx){// Tx a frame if required
      
  /////////////Power On /////////////////////////////////////////////////////////////////////////////////
  //Serial.println ("Power On");

//  PD needs to start low(Off), then be set high before programming.
  digitalWrite(PD, LOW); //Power Up unit- normale mode, Power down is done in radio hx1 in PTT off

  if ((battVolt < battLow) || (battVolt > battHigh)) {   //switch to low power with low battery to conserve power or too high to reduce heating
    digitalWrite (HL, HIGH);//  LOW- High Power, HIGH- Low power
    tStatStrg[7] ='0';
   } 
  else {
    digitalWrite (HL, LOW);//Low, switch to high power
    tStatStrg[7] ='1'; // 1 indicates high
  }

  delay(2000); //time for unit to power up, 2.5 tested, 2.0 tested, 1750 was a problem
  dra_serial.println(F("AT+DMOSETGROUP=0,144.3900,144.3900,0000,4,0000")); //Send config data
  delay(1000);

///////////////////////////////////////////////////////////////////////////////////////////// 

    if ((frame >= 1) && (frame <= 4)) {aprs_send();}
    if (frame == 5) {aprst_send();}
    if ((frame >= 6) && (frame <= 10)) {aprs_send();}
//    if  (frame == 10) {aprsw_send();}
//Serial.print ("Frame#= "); Serial.println (frame); 

/////////////////////////////////////////////////////////////////////
    while (afsk_flush()){ // wait for buffer to complete
     //power_save(); // use of power_save is problematic, why??
    }

#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif

} // **** skips a TX when frame2x false

    frame = frame +1;
    if (frame >=11)  {frame = 1;}

    // look ahead: set flag for next Tx if one
    frame2Tx = true; // all frames set for reporting (Tx)
   
///////////////////////////////////////////////////////////////////////////////////////  
/*
 *  The position of "}" here, is required to wait for a lock when a frame needs to be sent
 *  Do not change the position of "}"
 *  The if statement allows a power up of the GPS because of previous frame2Tx=false. 
 *  Without the if statement, a valid_pos advances a frame without a Tx
 */
} //****   
     if ((!valid_pos) && (!frame2Tx)) { frame2Tx = true;} //Tx, on the next cycle 
/////////////////////////////////////////////////////////////////////////////////////

//Power Mangement with 12volt system
    if (battVolt < battLow || battVolt > battHigh) { 
      beacon_rate= slow_aprs_rate;
      tStatStrg[6] ='1';// over ride adaptive reporting
    }
    else{
      tStatStrg[6] ='0'; // adaptive reporting period
    }
    
    old_heading_chg= gps_course; // ready for next course change
    next_aprs+=  (beacon_rate * 1000L); // update for next APRS period
    last_beacon= millis();
 

/////////////////////////////////////////////////////////////////////////////////////////////
//Serial.print ("Spd: "); Serial.print (gps_speed);
//Serial.print ("Dego: "); Serial.println (old_heading_chg);
//Serial.println (" ");

  } //****
 
   else {
    // Discard GPS data received during sleep window
    while (Serial.available()) {
      Serial.read();
    }
   }
  
power_save(); // Incoming GPS data or interrupts will wake us up
} //****
