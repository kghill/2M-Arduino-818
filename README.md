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
•	7-to-12-volt source 
•	3.7-volt 1s lithium battery source


### Model: 2M Arduino 818-12v

The 2M Arduino 818-12v board configuration is used with the Arduino Uno board. The intent of this option is to use it in a vehicle, boat, plane, etc. where a 12-volt battery is available and APRS position reporting is desired.  The voltage source to the boards is thru the DC jack. The onboard voltage regulator is used to power the 818 and other components via the Vin Pin. The digital level translation jumper is set to 5 volts.  

 
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

### Support

For questions or suggestions, you can contact us at:  ca.trackall@gmail.com


### Pricing and Options

Price is built upon the addition of the basic board. Check out the following link for options and shipping costs.

https://www.tindie.com/products/ballooner/2m-arduino-818/





