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
 */

/* Credit to:
 *
 * cathedrow for this idea on using the ADC as a volt meter:
 * http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */ 
 
 /* 
 * Remove AREF, since we are using DS1280 & BME280, not using LM60
 * Frees up memory
 */

#ifdef AVR

#include "config.h"
#include "pin.h"
#include "sensors_avr.h"

/*BME280///////////////////////////////////////////////////////////////////////
BME280 I2C Test.ino

https://github.com/finitespace/BME280

This code shows how to record data from the BME280 environmental sensor
using I2C interface. This file is an example file, part of the Arduino
BME280 library.

GNU General Public License

Written: Dec 30 2015.
Last Updated: Oct 07 2017.

Connecting the BME280 Sensor:
Sensor              ->  Board
-----------------------------
Vin (Voltage In)    ->  3.3V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro
 */

//#include <BME280I2C.h>


// #define BME_BAUDRATE 9600  // Serial baud rate defined in config (GPSBAUDRATE)
/*
BME280I2C bme;    // Default is suggested weather monitoring mode
                   //Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure �1, temperature �1, humidity �1, filter off,

*/
// DS12b20 library set up
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 6  // Data wire is plugged into port 8 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//Multiple DS18B20 1-Wire digital temperature sensors with Arduino example code. More info: https://www.makerguides.com
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DSsensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
int deviceCount = 0;
//////////////////////////////////////////////////

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif


void sensors_setup()
{
// DS1280 can be set up if it is not present. 
  DSsensors.begin();
  delay(1000); // ?? not in example code- required to prevent erratic readings on startup value=85
  deviceCount =DSsensors.getDeviceCount();
  Wire.begin();


// BME280
/*
/* Serial inatiated in .ino file
  Serial.begin(GPS_BAUDRATE);
   while(!Serial) {} // Wait


  float timeout = millis() + 2000; // wait 2 secs
  //tStatStrg[5]= '1'; // start with BME is functional
    // set bit back to 0 on timeout. Future APRS Status byte
    
  while (!bme.begin()) {
   //Serial.print("Timeout: "); Serial.println (timeout - millis());
       
   if ((timeout - millis()) < 0) {
  // Serial.println("Could not find BME280 sensor!");
    // set bit back to 0 on timeout. Future APRS Status byte
  //  tStatStrg[5]= '0'; // start with BME is functional
  //  bmeStat = false; // BME280 not found
     break;
    }
    delay(50); // 
   }
   */
  }
 



int sensors_vin()
{
  analogReference(DEFAULT);      // Ref=5V
  analogRead(VMETER_PIN);        // Disregard the 1st conversion after changing ref (p.256)
  delay(10);                     // This is needed when switching references

  uint16_t adc = analogRead(VMETER_PIN); 
  uint16_t mV = 5000L * adc / 1024;
   
  // Vin = mV * R2 / (R1 + R2)
  int vin = (uint32_t)mV * (VMETER_R1 + VMETER_R2) / VMETER_R2;
  return vin+vincal;
}

int sensors_spvin()
{
  analogReference(DEFAULT);      // Ref=5V
  analogRead(SPVMETER_PIN);        // Disregard the 1st conversion after changing ref (p.256)
  delay(10);                     // This is needed when switching references

  uint16_t adc = analogRead(SPVMETER_PIN); 
  uint16_t mV = 5000L * adc / 1024;
   
  // Vin = mV * R2 / (R1 + R2)
  int spvin = (uint32_t)mV * (SPVMETER_R1 + SPVMETER_R2) / SPVMETER_R2;
  return spvin+spvincal;
}

int sensors_bvin()
{
  analogReference(DEFAULT);      // Ref=5V
  analogRead(BVMETER_PIN);        // Disregard the 1st conversion after changing ref (p.256)
  delay(10);                     // This is needed when switching references

  uint16_t adc = analogRead(BVMETER_PIN); 
  uint16_t mV = 5000L * adc / 1024;
   
  // Vin = mV * R2 / (R1 + R2)
  int bvin = (uint32_t)mV * (BVMETER_R1 + BVMETER_R2) / BVMETER_R2;
  return bvin+bvincal;
}
int  extTemp(int i)
  {
  DSsensors.requestTemperatures(); // Send the command to get temperatures
  int  Temp =(DSsensors.getTempFByIndex(i)); //fahreniet
 // int  Temp =(DSsensors.getTempCByIndex(i));// centigrate
 // Temp =(Temp * 9/5) +32; // Convert to fahreniet
  return Temp;  
  }

/*
  float bmeread()
  {
  float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Fahrenheit);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);
  return (pres, temp, hum);
  }

    float intTemp()
  {
  float temp(NAN);
   //BME280::TempUnit tempUnit(BME280::TempUnit_Fahrenheit);
  //  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  // Call back Celuis
   temp = bme.temp();
   temp =(temp * 9/5) +32; // Convert to fahremiet
  return temp;
  }

 float intHum()
  {
  float hum(NAN);
   hum = bme.hum();
  return hum;
  }

   float intPres()
  {
  float pres(NAN);
   // BME280::PresUnit presUnit(BME280::PresUnit_Pa); // millibars (hPascels hectopascals)
   // call back in pascal
  pres = bme.pres();
  return pres;
  }
  */


#endif // ifdef AVR
