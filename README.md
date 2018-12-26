

  Aim is to develop a switching and signaling system for model railroad track control.
  Communication is wireless.

  To disconnect the decoder from the controlling software in space and time the MOSQUITTO
  MQTT broker is used.

  Hardware setup is as simple as possible. In stead of having one hardware module for diffferent applications
  the design aims for three different PCB's specific to task where the Wemos has to be slotted into.

  Sofware is modularised. It is gathered into one software package. The functionality of each separate decoder
  is configured by means of a website genrated by the decoder.

  *********************************************************************************************

  Target platform is Wemos D1 mini.

  11 digital input/output pins, all pins have interrupt/pwm/I2C/one-wire supported(except D0)
  1 analog input(3.2V max input)
  a Micro USB connection
  compatible with Arduino IDE.
  pins are active LOW but the Arduino IDE inverts this.

  All of the IO pins run at 3.3V:
  A levelshifter or voltage divider is required.

  For a relay a pin is followed by a transistor/IC. A relay is driven with 5V.

  For a sensor or servo each pin is followed by a 330R resistor to limit current to 10 mA.

  eltraco decoder     Function

  eltraco sensor decoder    Scanning 9 sensors
  eltraco switch decoder    Controlling 8 outputs
  eltraco turnout decoder   Controlling 2 turnouts

  GPIO usage:

  Decoder  pins     function

  Sensor   D0 .. D7     digital sensor
  
           A0           analogue sensor

  Switch   D0 .. D7     output (switch)

  Double   D0    relais 1
  
  Turnout  D1    current detector 1
           
           D2    current detector 2
           
           D3    not used
           
           D4    not used
           
           D5    relais 2
           
           D6    servo 1
           
           D7    servo 2
           
           D8    not used
           
           A0    not used
           
  Single   D0    relais 1
  
  Turnout  D1    current detector 1
           
           D2    sensor
           
           D3    sensor
           
           D4    sensor
           
           D5    sensor
           
           D6    servo 1
           
           D7    not used
           
           D8    not used
           
           A0    not used         

  ROCNET PROTOCOL

  packet payload content:
  byte 1  : groupId
  
  byte 2  : receiveIdH
  
  byte 3  : receiveIdL
  
  byte 4  : sendIdH
  
  byte 5  : sendIdL
  
  byte 6  : group
  
  byte 7  : code
  
  byte 8  : length
  
  byte 9  : data1
  
  byte 10 : data2
  
  byte 11 : data3
  
  byte 12 : data4
  

  --byte 1 only used for large network. Normally 0.

  --byte 2 only used when more than 255 decoders. Normally 0.

  --byte 3 Rocrail Server default Id is 1

  Broadcast Id = 0
  
  Decoder   Id = 2 ... 255   Not used for switching decoder

  --byte 4 only used when more than 255 decoders. Normally 0.

  --byte 5 Rocrail Server default Id is 1

  Decoder Id   = 2 ... 255 with restrictions as described under "addressing"

  --byte 6

  groups
  code   description     remark                     MQTT topic
  
  0      Host            Rocrail                    rocnet/ht
  
  7      Clock Fast      Clock                      rocnet/ck
  
  8      Sensor          Position determination     rocnet/sr
  
  9      Output                                     rocnet/ot


  --byte 7

  Code:  bit 7 = 0
  
     bit 6 and bit 5 represent Type of code
     
     bit 4 .. 0:  5 bit command 0 .. 31
     

  Type: 0 - test
  
  1 - request
  
  2 - event
  
  3 - reply
  

  Sensor

  Actions
  code description data 1  data 2  data 3  data 4  data n
  1    report      addrH¹  addrL¹  status  port    identifier (RFID)

  ¹) Address of the reporting loco.
  The sensor ID is set in the header; Sender.
  

  Output

  Type   Value
  
  switch   0
  
  light    1
  
  servo    2

  Actions
  code description data 1  data 2  data 3
  
  0      off       type    value   address
  
  1      on        type    value   address

  --byte 8 Netto number of following data bytes.
  

  At a speed of 200 KmH a loc runs 64 mm per second in scale H0 and 35 mm per second in scale N.
  To come to a reliable detection reaction a point sensor must be scanned at least 20 times per second in H0
  and at least 10 times per second in scale N.

  For scale H0 scan delay should be not larger than 50 miliseconds and for scale N not larger
  than 100 miliseconds. Default will be 20.

  The Wemos Module needs no 3.3V power supply. To the 5V pin external powersupply can be connected.

  Addressing:

  Addressing falls apart in a WiFi address, being an IP-address and a Rocnet address.

  Rocnet:

  In the Rocnet protocol (IdH*256 + IdL) represents the Rocnet Node Number. This number ranges from 1 .. 65536.
  It represents the "Bus" number.
  Depending on the function performed the I/O pin of the decoder is addressed with "data3" for
  "output" functions or "data4" for "sensor" functions. Turnouts are controlled with an "output" function.
  The values of data3/data4 are in rocrail inserted in field "Address"

  For eltraco an I/O pin is addressed by the combination of receiveIdL and data3/data4.
  This way each individual pin of each individual decoder can be addressed.

  A second way to address an IO pin is using a "fixed" receiveIdL and discriminating by means of data3/data4.
  Using this method provides in eltraco the possibility to address 255 turnouts by a consecutive number only.
  The default receiveIdL of Rocrail is "9". This receiveIdL is used to control turnouts. These addresses are
  used by the servo-tool to adjust the servo of a turnout.

  IP-address:
  The following IP-adress range is available:
  192.168.xxx.xxx


  IP-addres: Router 192.168.xxx.251
  
             Configuration website 192.168.xxx.252
             
             Command Station       192.168.xxx.253
             
             Mosquitto             192.168.xxx.254

  For the decoders are available 192.168.xxx.2 - 192.168.2.250

  Each decoder is allocated a unique IP-address out of the range 192.168.xxx.2 - 192.168.2.250.
  The last triplet is the Rocrail "Bus" number.

  Single turnout decoder:

Each decoder controls 1 turnout and reports occupation of it.

For the turnout decoder occupation of the turnout is reported at "Address" 1.

Table: Switches tab Interface - the last part of the IP-address is inserted into field "Bus"
       "1" is inserted into field "Address".
       e.g. (192.168.0.154 turnout is inserted as Bus 154 Address 1)

Table: Sensor tab Interface - the last part of the IP-address is inserted into field "Bus"
       "1" is inserted into field "Address". The external sensors report using "Address" numbers 2 .. 5.
       e.g. (192.168.0.154 turnout is inserted as Bus 154 Address 1, the second external sensor of the same
       decoder is inserted as Bus 154 Address 3)

Double turnout decoder:

Each decoder controls 2 turnouts and reports occupation of them.

For the turnout decoder occupation of the first turnout is reported at "Address" 1 and of the second
turnout at "Address" 2.

Table: Switches tab Interface - the last part of the IP-address is inserted into field "Bus"
       For the first turnout "1" is inserted into field "Address". For the second one "2" is inserted
       e.g. (192.168.0.154 turnout 2 is inserted as Bus 154 Address 2)

Table: Sensor tab Interface - the last part of the IP-address is inserted into field "Bus"
       For the first turnout "1" is inserted into field "Address". For the second one "2" is inserted
       e.g. (192.168.0.154 turnout 2 is inserted as Bus 154 Address 2)

Operation

During testing it turned out that move orders from Rocrail arrived so fast that an ongoing movement was
interrupted. To cater for this, incoming turnout orders are stored into a buffer. When ongoing movement is concluded,
buffer content is converted into a turnout order and the next turnout starts moving.

Sensor decoder:
Each decoder has eight digital and one analogue sensor.

The sensors report using "Address" numbers 1 .. 9.
Use of the analogue sensor is to be programmed by the user.

Table: Sensor tab Interface - the last part of the IP-address is inserted into field "Bus"
       port number is inserted into field "Address"
       e.g. (192.168.0.076 sensor 5 is inserted as Bus 76 Address 5)

Operation

The state of the sensor is controlled by interrupt. Not only the state of the sensor but also the point in time of activation is stored.
If the sensor is still in the activated state after the debounce period has passed the Sensor Message is sent. 
The state of the sensor will change to "inactive" by interrupt. After the minimum activation time has passed this inactive state is signalled 
with a Sensor Message.

Interrupt reacts on changes to drive the sensor state.

D0 can NOT be used with Interrupt. This reduces the amount of interrupt driven sensors per module to 7. (D1 .. D7).

D0 has to be watched with "polling".

Switch decoder
Each decoder has eight outputs.

Each decoder is allocated a unique IP-address out of the range 192.168.xxx.2 - 192.168.2.250.
The last triplet is the Rocrail "Bus" number.

The outputs are switched using "Address" numbers 1 .. 8.

Table: Switches tab Interface - the last part of the IP-address is inserted into field "Bus"
       The individual port number is inserted into field "Address"
       e.g. (192.168.0.201 port 3 is inserted as Bus 201 Address 3)

Operation

Via configuration it is possible to have an output switched ON during 200 mSec. An additional relais attached to that
output can be used to control a solenoid activated device.

To signal the state of a switch, a sensor message is transmitted after each switch action. 

Configuration of decoder

The Wemos module can be used in "ST" mode and "AP" mode simultaneously.
In "ST" mode it is possible to login onto an existing WiFi network for normal operations.
In "AP" mode it is possible to generate its own WiFi network. This network is used for configuration of the decoder.
Each module has its own configuration webserver on a shared WiFi configuration network with its own IP address.

The websever is based on the "EmbAJAX" library. This library facilitates dataexchange between webserver on the decoder
and a browser on a client. The user experience is as much as possible comparable with webservers on PC like hardware.

Configuration data and servo angles are stored in EEPROM.
Allocation of EEPROM memory to variables has to be planned.
  location variable
      0    AngleA1
      1    AngleB1
      2    Delay1
      3 .  Swap1
      4    AngleA2
      5    AngleB2
      6    Delay2
      7 .  Swap2
      8    decoderType
      9    debugFlag

  GENERALOFFSET+ 0 .. 25 ST WiFi SSID
  GENERALOFFSET + SSIDOFFSET + 0 .. 8 ST WiFi Password
  GENERALOFFSET + SSIDOFFSET + PWDOFFSET `+ 0 .. 6 ST WiFi IP address triplets
  GENERALOFFSET + SSIDOFFSET + PWDOFFSET + IPOFFSET + 0 .. 8 Output puls 
  GENERALOFFSET + SSIDOFFSET + PWDOFFSET + IPOFFSET + OUTPUTOFFSET + 0 .. 8 Switch active puls length 
  GENERALOFFSET + SSIDOFFSET + PWDOFFSET + IPOFFSET + OUTPUTOFFSET + PULSLENGTHOFFSET+ 0 .. 8 Output sensor message
  TOTALOFFSET 

  GENERALOFFSET      - 20
  SSIDOFFSET         - 30
  PWDOFFSET          - 20
  IPOFFSET           - 10
  OUTPUTOFFSET        -10
  PULSLENGTHOFFSET    -20
  OUTPUTSENSOROFFSET  -10

The ST WiFi password is generated out of "ELTRACO" and the decoderId.
The AP WiFi SSID is generated out of "ELTRACO" and the decoderID. This also aplies to the related password.

There are two ways to transfer Software to the decoder.
Via USB cable and after inital software installation also "Over The Air"(OTA).
Each decoder has its own OTA name which is generated outof "ELTRACO" and the decoderId.


Author: E. Postma

November 2018


