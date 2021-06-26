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
#include "pin.h"
#include "radio_hx1.h"
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif


void RadioHx1::setup()
{
  // Configure pins
  pinMode(PTT_PIN, OUTPUT);
  pin_write(PTT_PIN, LOW); // low switches to Rx

  pinMode (PD, OUTPUT);
  digitalWrite(PD, HIGH); //Power Down, HIGH-sleep mode or low. This need to start low, then be set high before programming.

  pinMode (HL, OUTPUT);
  digitalWrite (HL, LOW); // LOW- High Power, HIGH- Low power
  
  pinMode(AUDIO_PIN, OUTPUT);
 
}

void RadioHx1::ptt_on()
{
  pin_write(PTT_PIN, HIGH);
    delay(800);   // The DRA818 takes 750 ms from PTT to full RF ?? for reporting why ?? spec says 20 msec??
}

void RadioHx1::ptt_off()
{
  pin_write(PTT_PIN, LOW);
  delay (200);// PTT hang time, 200 msec to be tested 
  digitalWrite(PD,HIGH ); //Power Down unit Sleep mode- Off
}
