# 2M-Arduino-818


#### Note: A radio amateur license is required.

### Description:

The “2M Arduino 818” board was designed as a 2-Meter APRS unit for vehicle tracking, weather stations, and balloon launches. Other modes of transmission are possible such as RTTY, SSTV, etc.  

The major components of the board are the SA818 transceiver, the NEO-6M GPS module, and sensors. The board uses the Arduino form factor and can be configured for the Arduino Uno board for 12-volt applications or with the low power “Solar Pro Mini 1s” 3.7-volt board (coming soon). 

In conjunction with “Trackuino 818 Plus” software, the system is capable of sending position, weather, and telemetry packets. The “Trackuino 818 Plus” software is based on the original Trackuino software.

A link to the original software can be found here. 
https://github.com/trackuino/trackuino

The board includes a:

-	GPS NEO-6M GPS module
-	SA818 VHF Transceiver module for 500 mw or 1 W applications
-	7-element low pass filter for 2 meters
-	SMA Female 50-ohm connector
-	DS18b20 sensor to measure internal and/or external temperatures
-	Vin voltage sensor


Optional BME290: 
An optional BME280 sensor can be installed to measure temperature, humidity, barometric pressure. Software drivers for the BME280 are part of the “Trackuino 818 Plus” software.

Instead, the I2C interface can be used for other I2C compatible devices.

The board is configured for two voltage options:
-	7-to-12-volt source 
-	3.7-volt 1s lithium battery source


### Model: 2M Arduino 818-12v

The 2M Arduino 818-12v board configuration is used with the Arduino Uno board. The intent of this option is to use it in a vehicle, boat, plane, etc. where a 12-volt battery is available and APRS position reporting is desired.  The voltage source to the boards is thru the DC jack. The onboard voltage regulator is used to power the 818 and other components via the Vin Pin. The digital level translation jumper is set to 5 volts.  

### Note: 
The board needs adequate ventilation. If you plan to use the board where the ambient temperature is 100 F (38v) or above consider using a voltage adapter to provide an output voltage of 7 to 9 volts. The board does use the LM1085IT-ADJ regulator which provides overheating and over-current protection.

 
![image](https://user-images.githubusercontent.com/32202796/119375562-2f43c500-bc78-11eb-90f3-e60b64849ac1.png)


### Model: 2M Arduino 818-3.7v

The second option (Model: 2M Arduino 818-3.7v) is the low voltage, low power option. With this option, the regulator is not included and the digital level translation jumpers are set to 3.3 volts. The intent of this option is to use it for weather, balloon, and remote applications where solar panels and a 1s lithium battery are the power source. In this case, the APRS position, weather, and telemetry reporting are used.  

The “Solar Power Pro Mini 1s” processor was designed for this low-power option. 

WARNING: Do not use 2M Arduino 818-3.7v with the Arduino UNO board or equivalent.
///////////////////////////////////////////////////////////////////////////////////////////////////////

### Flexibility and Convivence

The “2M Arduino 818” board was designed around the flexibility of the SA181 transceiver module. With the 818-transceiver module, it can be programmed to different frequencies and transmission modes. The ability to change frequencies will accommodate different APRS areas and jurisdictions.  Along with the APRS transmission mode, other modes such RTTY, SSTV, etc. can be used by switching frequencies. 

Sensors are on one board along with a transceiver to provide a compact system for weather and voltage information.

The open-source Trackuino software has been modified to provide a weather packet frame, a telemetry packet frame, an interface to the SA818, and power management. The revised Trackuino version is the so-called “Trackuino 818 Plus”.

2M Arduino 818 board dimensions:
Width:  5.5 cm
Height:  4.0 cm
Length: 7.5 cm

Weight: 50 grams


# Trackuino Plus Software
## Overview:
The Trackuino Plus Software is a modified version of the original Trackuino software. The original version is found on github (https://github.com/trackuino/trackuino).  The Trackuino Plus Software has the following modifications: 
-	adaptive vehicle beaconing
-	telemetry reporting frame including bit status
-	weather reporting frame
-	addition of BME280 and DS18B20 sensors
-	addition of power management. Power management for UNO board is used to save battery life or to prevent over heating by switching to low power. 
-	flexible packet frame reporting
-	818 configuration; frequency

### Start-Up
After configuration and initial power-up the GPS module will take awhile to find a fix. After lock the tracker begins to reporting and the full frame sequence will take 10 minutes. The following are the visual indication that the unit is working:
-	On power-up the UNO Rx LED will be flashing
-	After a cold start the GPS Lock LED will start flashing once a second
-	After a GPS fix the PTT LED will flash


## Basic Configuration:

Configuration settings are found in the Arduino scketch (*.ino)  and in the configuration folder (config.h) in the library folder.  
Too edit the “config.h” file you will need to download a C editor. I would recommend Microsoft Visual Studio Code. You can download the editor at this link. https://code.visualstudio.com/ 

In the Arduino file *.ino, the adaptive beaconing, packet frame sequence, power management thresholds, and transmit frequency can be altered however, changing the settings are not necessary.
Config.h file

The first 100 lines are the User APRS settings. Beyond the first 100 lines are the hardware 2M hardware shield settings which I would recommend not changing.
The APRS settings are: 
-	Call sign		
-	Digipeating paths	 
- Period and slot transmission times
- APRS Table/Symbol

The following is an example of the user settings.
- Call Sign	MyCallSign	SSID	11
- Path:	WIDE1-1, Wide2-1		
- APRS Table: 	“/” primary	APRS Symbol:	“>” (vehicle)
- Time Slot-after the min:	40s	Reporting period (s):	60s

### APRS symbols
The APRS symbol designation comes in two parts, the symbol table ID and the symbol code. An APRS symbol table can be found at this link. https://www.yachttrack.org/info_camper/downloads/APRS_Symbol_Chart.pdf

## Arduino *.ino

The main thing to note is to set the Sketch book preference to the location of the folder that contains the “libraries” folder. The preference is found under the file tab of the Arduino editor.
In the Arduino file *.ino, the adaptive beaconing, packet frame sequence, power management thresholds, and 818 transmitter configurations can be altered. Changes will require some program knowledge and familiarity with Arduino software. Here is a link for further information. https://www.arduino.cc/en/Guide. The file provides comments for guidance.
 
### Adaptive beaconing
The following website provides information on APRS settings for SSID and beaconing parameters.   http://www.ciinet.org/paul/aprs.html. 

### Power Management
The parameters that can be adjusted are;
-	battWait: skips transmission until battery is recharged
-	battLow: switch low power
-	battHigh: used to reduce overheating

### Frame sequence
Allows the choice when, how often, and sequence of the different packets are transmitted. Transmission is skipped if power management is in effect. The following scenarios are possible by uncommenting the section of interest:
818 Transmitter configurations
Depending on the geographical location the APRS frequency differs. For the North American continent the frequency is set to “144.3900” . The commands for the 818 modules can be found at this link https://www.qsl.net/ta2ei/devreler/sa818/SA818%20programming%20manual.pdf


### Support

For questions or suggestions, you can contact us at:  ca.trackall@gmail.com


### Pricing and Options

Check out the following link for options and shipping costs.

https://www.tindie.com/products/ballooner/2m-arduino-818/




