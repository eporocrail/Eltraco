/*
 * 
  changelog:
  "2018-02-13". Display debugFlag during boot. Display all config data during boot.
  Serial monitor messages harmonised.

  https://github.com/Links2004/arduinoWebSockets websockets library needs to be installed.

  changelog:
  "2018-01-22" One software package. Decodertype selection. Userdate via webserver.

  "2018-01-20" Website for adjustment servo added. Now it is possible to adjust servos of turnout decoders
  with smartphone or tablet. A webserver resides on each turnout decoder and presents a webpage when
  the user points the browser to the IP-address or the decoder name. By means of a slider the angles are adjusted.
  Pushing a button acknowledges each angle.

  "2017-12-21" Software version display added

  dec 2017:
  new MQTT library. supports qos2. link for library: https://github.com/Imroy/pubsubclient
  Length of MQTT outgoing message equal to Rocnet message length.
  "yield()" in loop.
  display of published and received messages on serial monitor harmonised.

  Eltraco_SmartTO_v1.0
  Ready for publication

  /*************************************************************************************

  Aim is to develop a switching and signaling system for model railroad track control.
  Communication is wireless.

  To disconnect the decoder from the controlling software in space and time the MOSQUITTO
  MQTT broker is used.

  Hardware setup is as simple as possible. In stead of having one hardware module for diffferent applications
  the design aims for different PCB's specific to task where the Wemos has to be slotted into.

  Sofware is modularised. The modules are contained in one single package. Configuration of the function
  of a single decoder is done by the built-in Eltraco configuration website.

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

  eltraco sensor decoder            Scanning 9 sensors
  eltraco switch decoder            Controlling 8 outputs
  eltraco double turnout decoder    Controlling 2 turnouts
  eltraco single turnout decoder    Controlling 1 turnout and scanning 4 external sensors
  eltraco display decoder           Displaying text or showing the Rocrail time with an analogue clock

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

  Single   D0    relais
  Turnout  D1    current detector
           D2    sensor
           D3    sensor
           D4    sensor
           D5    sensor
           D6    servo
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

  Addressing:

  In the Rocnet protocol (IdH*256 + IdL) represents the Rocnet Node Number. This number ranges from 1 .. 65536.
  It represents the "Bus" number.
  Depending on the function performed the I/O pin of the decoder is addressed with "data3" for
  "output" functions or "data4" for "sensor" functions. Turnouts are controlled with an "output" function.
  The values of data3/data4 are in rocrail inserted in field "Address"

  For eltraco an I/O pin is addressed by the combination of receiveIdL and data3/data4.
  This way each individual pin of each individual decoder can be addressed.

  IP-address:
  The following IP-adress range is available:
  192.168.xxx.xxx


  IP-addres: Router 192.168.xxx.251
             First install Decoder  192.168.2.252
             Command Station 192.168.xxx.253
             Mosquitto 192.168.xxx.254

  For the decoders are available 192.168.xxx.2 - 192.168.2.250

  Each decoder is allocated a unique IP-address out of the range 192.168.xxx.2 - 192.168.2.250.
  The last triplet is the Rocrail "Bus" number.

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

  Sensor decoder:
  Each decoder has eight digital and one analogue sensor.

  The sensors report using "Address" numbers 1 .. 9.
  Use of the analogue sensor is to be programmed by the user.

  Table: Sensor tab Interface - the last part of the IP-address is inserted into field "Bus"
         port number is inserted into field "Address"
         e.g. (192.168.0.076 sensor 5 is inserted as Bus 76 Address 5)

  Output decoder
  Each decoder has eight outputs.

  Each decoder is allocated a unique IP-address out of the range 192.168.xxx.2 - 192.168.2.250.
  The last triplet is the Rocrail "Bus" number.

  The outputs are switched using "Address" numbers 1 .. 8.

  Table: Switches tab Interface - the last part of the IP-address is inserted into field "Bus"
         The individual port number is inserted into field "Address"
         e.g. (192.168.0.201 port 3 is inserted as Bus 201 Address 3)

  During testing it turned out that move orders from Rocrail arived so fast that an ongoing movement was
  interrupted. To cater for this incoming turnout orders are stored into a buffer. When ongoing movement is concluded,
  buffer is converted into turnout an order and the second turnout starts moving.


  Data exchange between webinterface and decoder.
  The configuration data have to be transmitted between user web page in browser and the decoder.
  Normally PHP is used on the server. With ESP8233 this is not possible.
  Websockets is the protocol to transmit data.
  The datastructure used is:

  the use of characters is:
  position 0:       # to designate "no servo angle data"
  position 1 and 2: group
  position 3:       member
  position 4 .. 6   data

  group:
  0 - IP-address
  1 - decoder type
  2 - flag

  member:
  group IP-address:   0 - decoder
                      1 - gateway
                      2 - mosquitto

        decoder type: 0 - double trunout
                      1 - single turnout
                      2 - switch
                      3 - sensor

        flag:         0 - new Wifi network
                      1 - debug

  data:               position
  group IP-address:   4 . . 6 characters
        decoder type: 4       1 character  
        flag          4       1 character (0=off, 1=on)

  Configuration data and servo angles are stored in EEPROM.
  Allocation of EEPROM memory to variables has to be planned.
  location variable
      0    servoPos[0][0]
      1    servoPos[0][1]
      2    servoPos[1][0]
      3    servoPos[1][1]
      4    ipMosquitto[3]
      5    ipDecoder[3]
      6    ipGateway[3]
      7    decoderType
      8    debugFlag

  Only one variable is stored in SPIFFS
      In file " wifi.txt" resetWifi is stored. (being "0" or "1")

  With configuration data outside the software it is possible to introduce mistakes.
  The most severe mishap would be to lock oneselves out from access to the WiFi manager.
  To provide an escape when this should occur, the "resetWifi" variable is not stored 
  in EEPROM but in SPIFFS. The advantage is that information in SPIFFS can be accessed
  by simply uploading a new file.
  Would the variable be in EEPROM than a new software upload would be required.

  The rest of the variables stay in EEPROM because EEPROM is a random access memory where
  ISPFFS is serial access memory.
  

  Author: E. Postma

  February 2018

   *****/
