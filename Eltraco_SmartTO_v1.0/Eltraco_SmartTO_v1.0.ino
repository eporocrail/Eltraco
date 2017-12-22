/*

  changelog:
  "2017-12-21" Software version display added
              
  dec 2017-2
  "char" for numbers is WRONG!
  change to byte

  dec 2017:
  new MQTT library. supports qos2. link for library: https://github.com/Imroy/pubsubclient
  Length of MQTT outgoing message equal to Rocnet message length.
  "yield()" in loop.
  display of published and received messages on serial monitor harmonised.
  change "byte" to "char"


  Eltraco_SmartTO_v1.0
  Ready for publication

  /*************************************************************************************

  Aim is to develop a switching and signaling system for model railroad track control.
  Communication is wireless.

  To disconnect the decoder from the controlling software in space and time the MOSQUITTO
  MQTT broker is used.

  Hardware setup is as simple as possible. In stead of having one hardware module for diffferent applications
  the design aims for three different PCB's specific to task where the Wemos has to be slotted into.

  Sofware is modularised. Instead of having one sketch for diffferent applications the design aims for
  three different sketches specific to task.

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

  Turnout  D0    relais 1
           D1    current detector 1
           D2    current detector 2
           D3    not used
           D4    not used
           D5    relais 2
           D6    servo 1
           D7    servo 2
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

  A second way to address an IO pin is using a "fixed" receiveIdL and discriminating by means of data3/data4.
  Using this method provides in eltraco the possibility to address 255 turnouts by a consecutive number only.
  The default receiveIdL of Rocrail is "9". This receiveIdL is used to control turnouts. These addresses are
  used by the servo-tool to adjust the servo of a turnout.

  IP-address:
  The following IP-adress range is available:
  192.168.xxx.xxx


  IP-addres: Router 192.168.xxx.251
             Servo tool 192.168.xxx.252
             Command Station 192.168.xxx.253
             Mosquitto 192.168.xxx.254

  For the decoders are available 192.168.xxx.2 - 192.168.2.250 except 192.168.xxx.9

  Smart turnout decoder:

  Each decoder controls 2 turnouts and reports occupation of them.

  Each of both turnouts is allocated a unique consecutive number out of the range 1 .. 256.
  This is the Rocrail "Address" number. The Rocrail default "Bus" number is "9". This Bus is used to control
  turnouts.

  For the turnout decoder occupation of the first turnout is reported at "Address" 1 and of the second
  turnout at "Address" 2.

  Table: Switches tab Interface - "9" is inserted into field "Bus"
         The unique number of the turnout is inserted into field "Address"
         e.g. (turnout 23 is inserted as Bus 9 Address 23)

  Table: Sensor tab Interface - "9" is inserted into field "Bus"
         The unique number of the turnout is inserted into field "Address"
         e.g. (sensor for turnout 23 is inserted as Bus 9 Address 23)

  Each of all other kinds of decoder is allocated a unique IP-address out of the range
  192.168.xxx.2 - 192.168.2.250. The last triplet is the Rocrail "Bus" number.

  Sensor decoder:
  Each decoder has eight digital and one analogue sensor.

  Each decoder is allocated a unique IP-address out of the range 192.168.xxx.2 - 192.168.2.250.
  The last triplet is the Rocrail "Bus" number.

  The sensors report using "Address" numbers 1 .. 9.

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
  interrupted. incoming turnout orders are stored into buffer. When ongoing movement is concluded, buffer is converted
  into turnout order.

  Author: E. Postma

  September 2017

   *****/

#include <PubSubClient.h>
#include <Servo.h>
#include <stdlib.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
extern "C" {
#include "user_interface.h"
}

Servo servo[2];
WiFiClient espClient;

#define WIFI_TX_POWER 0 // TX power of ESP module (0 -> 0.25dBm) (0...85)

// do not use as turnoutNr1 or turnoutNr2: 1 and 9
static byte turnoutNr1 = 21;                               // also used in IP address decoder (check if IP address is available)
static byte turnoutNr2 = 22;
static char wiFiHostname[] = "ELTRACO-ST-21";              // Hostname displayed in OTA port
static String version = "2017-12-21";
static String decoderType = "Double turnout";
///////////////////////
/*
   Define which Wifi Network is to be used
   0 = Network 0 train room
   1 = Network 1 test room
   2 = Mobile demo layout

*/
#define WIFI_NETWORK 2

//////////////////////////// wifi network selection //////////////
#if WIFI_NETWORK == 0
static const char *ssid = "SSID0";                                          // ssid WiFi network
static const char *password = "PWD0";                                       // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                        // IP address Mosquitto
IPAddress decoder(192, 168, 2, turnoutNr1);                                 // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                          // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet mask

#endif

#if WIFI_NETWORK == 1
static const char *ssid = "SSID1";                                          // ssid WiFi network
static const char *password = "PWD1";                                       // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                        // IP address Mosquitto
IPAddress decoder(192, 168, 2, turnoutNr1);                                 // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                          // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet mask

#endif

#if WIFI_NETWORK == 2
static const char *ssid = "SSID2";                                          // ssid WiFi network
static const char *password = "PWD2";                                       // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 1, 254);                                      // IP address Mosquitto
IPAddress decoder(192, 168, 1, turnoutNr1);                                 // IP address decoder
IPAddress gateway(192, 168, 1, 251);                                        // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet mask

#endif
PubSubClient client(espClient, mosquitto);
//////////////////////////////////////// decoder function selection ///////////////////////////////////////////////////
static const String topicPub = "rocnet/sr";                                  // rocnet/rs for sensor
static const String topicSub1 = "rocnet/ot";                                 // rocnet/ot for turnout control
static const String topicSub2 = "rocnet/cf";                                 // rocnet/cf for servo configuration
static const String topicSub3 = "rocnet/sr";                                 // rocnet/rs for sensor

// Turnout decoder
static const byte turnoutBus = 9;                                           // Rocrail Busnumber for turnout
static const byte turnoutNr = 2;                                            // number of turnouts for the PCB
static const byte addressTu[turnoutNr] = {turnoutNr1, turnoutNr2};          // address of both turnouts
static const byte servoPin[turnoutNr] = {D6, D7};                           // servo pin number
static byte servoPos[turnoutNr][2] = {
  {45, 135},
  {20, 140}
};                                                                          // pos straight, pos thrown
static boolean inverted[turnoutNr] = {false, false};                        // if angleservoPos 0 > servoPos 1 than inverted = true
static const byte relais[turnoutNr] =  {D0, D5};                            // relais pin number
static byte servoDelay[turnoutNr] = {40, 40};                               // controls speed servo movement, higher is slower
// sensor
static const byte sensorNr = 2;                                             // amount of sensors
static const byte addressSr[sensorNr] = {turnoutNr1, turnoutNr2};           // sensor addresses
static const byte sensor[sensorNr] = {D1, D2};                              // sensor pins with each a pull-up resistor

//////////////////////////////////////// end of decoder function selection ///////////////////////////////////////////////////

static const int msgLength = 12;                                            // message number of bytes
static byte msgOut[msgLength];                                              // outgoing messages
static boolean sendMsg = false;                                             // set true to publish message
static boolean forMe = false;                                               // flag for handling incoming message
static boolean firstMsg = true;                                             // flag to throw away first message (dump from rocrail)

struct {
  byte targetId = 0;
  boolean order = false;
  boolean orderOld = false;
  boolean orderNew = false;
} buf;

struct {
  byte targetId = 0;
  boolean in = false;
  boolean executed = false;
  boolean msgMove = false;
  boolean msgStop = false;
} order;

static boolean sensorStatus[sensorNr];                                     // status sensor pins
static boolean sensorStatusOld[sensorNr];                                  // old status sensor pins
static unsigned long sensorProcessTime[sensorNr];                          // sensor timer
static byte sensorCountOff[sensorNr];                                      // counter negative sensor values
static boolean scan = false;                                               // sensorvalue
static const byte scanDelay = 5;                                           // delay in sensor scan processing
static const byte scanNegativeNr = 15;                                     // number of negative scan values for negative sensorstatus

static String(turnoutOrder) = "";                                          // used with debugging
static unsigned int relaisSwitchPoint[turnoutNr];                          // relais switch start time
static unsigned int relaisSwitchDelay[turnoutNr];                          // calculated relais switch time
static boolean turnoutInit[turnoutNr];                                     // servo initiation flag
static byte currentPosition[turnoutNr];                                    // servo position
static byte targetPosition[turnoutNr];                                     // servo position to be reached
static unsigned long servoMoveTime[turnoutNr];                             // servo timer
static byte turnoutId = 0;                                                 // id turnout
static boolean turnoutMoving = false;

static byte servoId = 0;                                                   // id servo
static byte servoAngle;                                                    // angle servo
static byte servoAngleOld;
static byte ackStraight = 0;                                               // ack straight for servo adjust
static byte ackThrown = 0;                                                 // ack thrown for servo adjust

static boolean debugFlag = true;                                           // display debug messages
static boolean configFlag = true;                                          // control servoconfiguration

///////////////////////////////////////////////////////////////set-up//////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println();

  system_phy_set_max_tpw(WIFI_TX_POWER); //set as lower TX power as possible

  WiFi.hostname(wiFiHostname);
  SetupWifi();

  EEPROM.begin(512);
  if ((EEPROM.read(255) == 0xFF) && (EEPROM.read(256) == 0xFF)) {                 //eeprom empty, first run
    Serial.println(" ******* EPROM EMPTY....Setting Default EEPROM values *****");
    for (int index = 0; index < 512; index++)
      EEPROM.write(index, 0);
    EEPROM.commit();
    delay(10000);
    WriteEEPROM();
  }

  ReadEEPROM();                                                                  //read the servo data

  memset(sensorStatus, 0, sizeof(sensorStatus));                                 // initialising arrays sensor decoder
  memset(sensorStatusOld, 0, sizeof(sensorStatusOld));
  memset(sensorProcessTime, 0, sizeof(sensorProcessTime));
  memset(msgOut, 0, sizeof(msgOut));
  memset(sensorCountOff, 0, sizeof(sensorCountOff));

  msgOut[2] = 1;                                                                 // intializing fixed content outgoing message
  msgOut[4] = turnoutBus;
  msgOut[5] = 8;
  msgOut[6] = 1;
  msgOut[7] = 4;

  memset(relaisSwitchPoint, 0, sizeof(relaisSwitchPoint));
  memset(relaisSwitchDelay, 0, sizeof(relaisSwitchDelay));
  memset(servoMoveTime, 0, sizeof(servoMoveTime));
  memset(currentPosition, 0, sizeof(currentPosition));
  memset(targetPosition, 0, sizeof(targetPosition));
  memset(turnoutInit, 0, sizeof(turnoutInit));

  for (byte index = 0; index < sensorNr; index++) {                              // initialising sensor pins
    pinMode(sensor[index], INPUT_PULLUP);
  }
  for (byte index = 0; index < turnoutNr ; index++) {                            // initialising relais pins
    pinMode(relais[index], OUTPUT);
    digitalWrite(relais[index], LOW);
  }
  for (byte index = 0; index < turnoutNr ; index++) {                            // initialising relais switch points
    if ((servoPos[index][0]) > (servoPos[index][1])) inverted[index] = true;
    relaisSwitchPoint[index] = ((servoPos[index][0] + servoPos[index][1]) / 2);
  }
  for (byte index = 0; index < turnoutNr ; index++) {                            // attaching servos
    servo[index].attach (servoPin[index]);
  }

  StartPosition();                                                               // move servosto initial position

  client.set_callback(Callback);

  //// begin of OTA ////////
  ArduinoOTA.setPort(8266);                                                      // Port defaults to 8266
  ArduinoOTA.setHostname(wiFiHostname);                                          // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setPassword((const char *)"123");                                 // No authentication by default
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  ////// end of OTA ////////
  buf.orderNew = true;
  order.executed = true;
}

///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  ArduinoOTA.handle();                                                          // OTA handle must stay here in loop
  yield();

  CheckBuffer();
  ProcessOrder();
  if (configFlag == false) ServoAdjust();
  ScanSensor();

  if (!client.connected()) {                                                   // maintain connection with Mosquitto
    Reconnect();
  }
  client.loop();                                                               // content of client.loop can not be moved to function
  if (sendMsg == true) {                                                       // set sendMsg = true  to transmit message
    if (debugFlag == true) {
      Serial.println();
      Serial.print(F("Publish msg  ["));
      Serial.print(topicPub);
      Serial.print(F(" - DEC, dotted] ==> "));
      for (int index = 0 ; index < msgLength ; index++) {
        Serial.print((msgOut[index]), DEC);
        if (index < msgLength - 1) Serial.print(F("."));
      }
      Serial.println();
    }
    if (client.publish(MQTT::Publish(topicPub, msgOut, msgLength).set_qos(2)) != true) {     // message is published with qos2, highest
      Serial.println(F("fault publishing"));                                                 // guarantee for delivery of message to Mosquitto
    } else sendMsg = false;
  }
}
///////////////////////////////////////////////////////////// end of program loop ///////////////////////
/*
   SoftwareVersion

    function : display on serial monitor decoder type and sofware version

    called by: reconnect

*/
void SoftwareVersion() {
  Serial.println();
  Serial.print("\n===================================================================");
  Serial.print("\n");
  Serial.print("\n        EEEEE  LL   TTTTTT  RRR        A        CCC     OO");
  Serial.print("\n        EE     LL     TT    RR RR    AA AA     CC     OO  OO");
  Serial.print("\n        EEE    LL     TT    RRR     AAAAAAA   CC     OO    OO");
  Serial.print("\n        EE     LL     TT    RR RR   AA   AA    CC     OO  OO");
  Serial.print("\n        EEEEE  LLLLL  TT    RR  RR  AA   AA     CCC     OO");
  Serial.print("\n");
  Serial.print("\n===================================================================");
  Serial.println();
  Serial.print("\n                    decoder: ");
  Serial.println(decoderType);
  Serial.println();
  Serial.print("\n                    version: ");
  Serial.print(version);
  Serial.println();
  Serial.print("\n-------------------------------------------------------------------");
  Serial.println();
} // end of SoftwareVersion

/*

   ScanSensor

   function : read status of input pins. adjust status of sensor, generate outgoing message,
              sensorCountOff used to decide after how many scans "LOW" the sensor status is adjusted
              required for current detector
              set sendMsg = true to send message, set sendMsg = false for scan loop

   called by: loop

*/
void ScanSensor() {
  for (byte index = 0; index < sensorNr; index++) {
    if ( millis() > sensorProcessTime[index]) {
      sensorProcessTime[index] = millis() + scanDelay;
      sensorStatusOld[index] = sensorStatus[index];
      scan = !digitalRead(sensor[index]);
      if (scan == false) {
        sensorCountOff[index]++;
      }
      if (scan == true) {
        sensorCountOff[index] = 0;
        sensorStatus[index] = true;
      }
      if ((sensorCountOff[index]) > scanNegativeNr) {
        sensorStatus[index] = false;
      }
      if ((sensorStatusOld[index]) != (sensorStatus[index])) {
        sendMsg = true;
        msgOut[10] = sensorStatus[index];
        msgOut[11] = addressSr[index];
      }
    }
  }
} // end of Scansensor



/*

   CheckBuffer

   function : converts buffer into turnout order at right time

   called by: loop

*/
void CheckBuffer() {
  if (order.executed == true) {
    if (buf.orderNew == true) {
      buf.orderNew = false;
      order.in = buf.order;
      order.targetId = buf.targetId;
      order.executed = false;
      order.msgMove = false;
      order.msgStop = false;
    }
  }
} //end of CheckBuffer


/*
   ProcessOrder

   function : start action

   called by: loop
   calls    : Thrown, Straight

*/
void ProcessOrder() {
  if (order.executed == false) {
    if ((order.in == 1)) Thrown(order.targetId);
    if ((order.in == 0)) Straight(order.targetId);
  }
} // end of ProcessOrder()

/*
   Thrown

   function : move servo into thrown position
              compare current servo position with ordered position, signal start servo movement,
              switch relais in middle of servo movement, signal end of movement, set flag order executed

   called by: ProcessOrder

   calls    : txMsgMove, txMsgMoveStop

*/
byte Thrown(byte turnoutId) {
  currentPosition[turnoutId] = servo[turnoutId].read();
  if ((currentPosition[turnoutId]) != (servoPos[turnoutId][1]) && millis() >= servoMoveTime[turnoutId]) {
    if (inverted[turnoutId] == true) {
      servo[turnoutId].write((currentPosition[turnoutId]) - 1);
    } else servo[turnoutId].write((currentPosition[turnoutId]) + 1);
    servoMoveTime[turnoutId] = millis() + servoDelay[turnoutId];
    if (order.msgMove == false) {
      order.msgMove = true;
      TxMsgMove(order.targetId);
    }
    if ((currentPosition[turnoutId]) == (relaisSwitchPoint[turnoutId])) digitalWrite(relais[turnoutId], HIGH);
  }
  if ((currentPosition[turnoutId]) == (servoPos[turnoutId][1])) {
    order.executed = true;
    if (order.msgStop == false) {
      order.msgStop = true;
      TxMsgMoveStop(order.targetId);
    }
  }
} // end of Thrown

/*

   function : move servo into straight position
              compare current servo position with ordered position, signal start servo movement
              switch relais in middle of servo movement, signal end of movement, set flag order executed

   called by: ProcessOrder

   calls    : txMsgMove, txMsgMoveStop

*/
byte Straight(byte turnoutId) {
  currentPosition[turnoutId] = servo[turnoutId].read();
  if ((currentPosition[turnoutId]) != (servoPos[turnoutId][0]) && millis() >= servoMoveTime[turnoutId]) {
    if (inverted[turnoutId] == true) {
      servo[turnoutId].write((currentPosition[turnoutId]) + 1);
    } else servo[turnoutId].write((currentPosition[turnoutId]) - 1);
    servoMoveTime[turnoutId] = millis() + servoDelay[turnoutId];
    if (order.msgMove == false) {
      order.msgMove = true;
      TxMsgMove(order.targetId);
    }
    if ((currentPosition[turnoutId]) == (relaisSwitchPoint[turnoutId])) digitalWrite(relais[turnoutId], LOW);
  }
  if ((currentPosition[turnoutId]) == (servoPos[turnoutId][0])) {
    order.executed = true;
    if (order.msgStop == false) {
      order.msgStop = true;
      TxMsgMoveStop(order.targetId);
    }
  }
} // end of Straight

/*
   ServoAdjust

   function : adjust straight and thrown position of servo. positions sent by servo tool.

   called by: loop

*/
void ServoAdjust() {
  if ((servoAngle) != (servoAngleOld)) {
    Serial.print("angle: ");
    Serial.println(servoAngle);
    servo[servoId].write(servoAngle);                           // servo takes position sent by servo tool
    servoAngleOld = servo[servoId].read();
  }
  if ((ackStraight == 1) && (ackThrown == 0)) {
    servoPos[servoId][0] = servoAngle;                          // angle accepted when ack received
  }
  if ((ackThrown == 1) && (ackStraight == 0)) {
    servoPos[servoId][1] = servoAngle;                          // angle accepted when ack received
  }
  if ((ackStraight == 1) && (ackThrown == 1)) {                 // cycle concluded when both acks are received
    if (debugFlag == true) {
      Serial.println();
      Serial.print("Servo Turnout address: ");
      Serial.print(addressTu[servoId]);
      Serial.print(" angle 1 - ");
      Serial.print(servoPos[servoId][0]);
      Serial.print(" angle 2 - ");
      Serial.print(servoPos[servoId][1]);
      Serial.println();
    }
    EEPROM.write ((servoId * 2), servoPos[servoId][0]);         // write values in EEPROM
    EEPROM.write ((servoId * 2 + 1), servoPos[servoId][1]);
    EEPROM.commit();
    delay(200);
    memset(msgOut, 0, sizeof(msgOut));
    msgOut[0] = addressTu[servoId];
    msgOut[4] = 1;
    sendMsg == true;
    configFlag = true;
    ReadEEPROM();
    if (debugFlag == true) {
      Serial.println();
      Serial.println("servo adjusted");
      Serial.println();
    }
    ackStraight = 0;
    ackThrown = 0;
  }
} //end of servoAdjust

/*
   WriteEEPROM

   function : write initial servo values to EEPROM

   called by: setup

*/
void WriteEEPROM() {
  byte val = 0;
  byte tlr = 0;
  for (byte index = 0; index < turnoutNr; index++) {
    for (byte ind = 0; ind < 2; ind++) {
      val = servoPos[index][ind];
      tlr = (index * 2 + ind);
      EEPROM.write (tlr, val);
    }
  }
  EEPROM.commit();
  delay(500);
} // end of WriteEEPROM

/*
   ReadEEPROM

   function : read servo values from EEPROM

   called by: setup, ServoAdjust

*/
void ReadEEPROM() {
  if (debugFlag == true) Serial.println();
  for (int index = 0; index < turnoutNr; index++) {
    if (debugFlag == true) {
      Serial.print("Servo Turnout address: ");
      Serial.print(addressTu[index]);
      Serial.println();
    }
    for (int ind = 0; ind < 2; ind++) {
      servoPos[index][ind] = EEPROM.read((2 * index) + ind);
      if (debugFlag == true) {
        Serial.print(" angle ");
        Serial.print(ind + 1);
        Serial.print(": ");
        Serial.print(servoPos[index][ind]);
        Serial.println();
      }
    }
    Serial.println();
  }
} // end of ReadEEPROM


/*
   txMsgMove

   function : signal start movement servo to Rocrail

   called by: Thrown, Straight

*/
byte TxMsgMove(byte turnoutId) {
  msgOut[11] = addressSr[turnoutId];
  msgOut[10] = 1;
  sendMsg = true;
} // end of txMsgMove

/*
   txMsgMoveStop

   function : signal stop movement servo to Rocrail

   called by: Thrown, Straight

*/
byte TxMsgMoveStop(byte turnoutId) {
  msgOut[11] = addressSr[turnoutId];
  msgOut[10] = 0;
  sendMsg = true;
} // end of txMsgMoveStop

/*

   Callback

   function : receive incoming message, test topic, test on recepient, select returning sensor messages, select servo configuration,
   select incoming turnout switch orders, select turnout number, store order into buffer.

*/
void Callback(const MQTT::Publish& pub) {
  if ((pub.topic()) == ("rocnet/sr")) {
    forMe = false;
    for (byte index = 0; index < turnoutNr; index++) {
      if (((byte)pub.payload()[11]) == (addressTu[index])) {          // check if address is contained in address array
        forMe = true;
      }
    }
    if (forMe == true) {
      if (debugFlag == true) {
        Serial.println();
        Serial.print("Msg received [");
        Serial.print(pub.topic());
        Serial.print(" - DEC, dotted] <== ");
        for (byte index = 0; index < (pub.payload_len()); index++) {
          Serial.print(((byte)pub.payload()[index]), DEC);
          if (index < (pub.payload_len()) - 1) Serial.print(F("."));
        }
        Serial.println();
      }
    }
  }

  if ((pub.topic()) == ("rocnet/cf")) {
    forMe = false;                                        // check if address is contained in address array
    servoId = 0;
    for (byte index = 0; index < turnoutNr; index++) {
      if (((byte)pub.payload()[0]) == (addressTu[index])) {
        forMe = true;
        servoId = index;
      }
    }
    if (forMe == true) {
      if (debugFlag == true) {
        Serial.println();
        Serial.print("Msg received [");
        Serial.print(pub.topic());
        Serial.print(" - DEC, dotted] <== ");
        for (byte index = 0; index < (pub.payload_len()); index++) {
          Serial.print(((byte)pub.payload()[index]), DEC);
          if (index < (pub.payload_len()) - 1) Serial.print(F("."));
        }
        Serial.println();
      }
      configFlag = false;
      servoAngle = ((byte)pub.payload()[1]);
      ackStraight = ((byte)pub.payload()[2]);
      ackThrown = ((byte)pub.payload()[3]);
    }
  }

  if ((pub.topic()) == ("rocnet/ot")) {
    forMe = false;
    if (((byte)pub.payload()[2]) == (turnoutBus)) {
      for (byte index = 0; index < turnoutNr; index++) {
        if (((byte)pub.payload()[11]) == (addressTu[index])) {          // check if address is contained in address array
          buf.targetId = index;
          forMe = true;
        }
      }
      if (forMe == true) {
        buf.orderOld = buf.order;
        buf.order = ((byte)pub.payload()[8]);
        buf.orderNew = true;
        if (debugFlag == true) {
          Serial.println();
          Serial.print("Msg received [");
          Serial.print(pub.topic());
          Serial.print(" - DEC, dotted] <== ");
          for (byte index = 0; index < (pub.payload_len()); index++) {
            Serial.print(((byte)pub.payload()[index]), DEC);
            if (index < (pub.payload_len()) - 1) Serial.print(F("."));
          }
          Serial.println();
          if ((buf.order) == 1) {
            turnoutOrder = "thrown";
          } else {
            turnoutOrder = "straight";
          }
          if (turnoutNr > 0) {
            Serial.print(F("Message in ["));
            Serial.print(pub.topic());
            Serial.print(F("] "));
            Serial.print(F("switch turnout - "));
            Serial.print(buf.targetId + 1);
            Serial.print(F(" address "));
            Serial.print(addressTu[buf.targetId]);
            Serial.print(F(" to "));
            Serial.print(turnoutOrder);
            Serial.println();
          }
        }
        /////////// debugging ///////
      }
    }
  }
} // end of Callback

/*
   re-establish connection with MQTT clientID.
   publish topic topicPub. subscribe to topic topicSub.
   when Mosquitto not available try again after 5 seconds

*/
void Reconnect() {
  while (!client.connected()) {
    Serial.print("Establishing connection with Mosquitto ...");
    // Attempt to connect
    if (client.connect(MQTTclientId)) {
      Serial.println("connected");
      client.subscribe(topicSub1);                              // and resubscribe to topic 1
      client.subscribe(topicSub2);                              // and resubscribe to topic 2
      client.subscribe(topicSub3);                              // and resubscribe to topic 3
      SoftwareVersion();
    } else {
      Serial.print("no Broker");
      Serial.println(" try again in second");
      delay(1000);                                             // Wait 1 second before retrying
    }
  }
}
// end of reconnect

/*
   StartPosition()

   function : move servo to middel position and back into straight position. switch relais off
   called by: Setup

*/
void StartPosition() {
  for (byte index = 0; index < turnoutNr ; index++) {
    servo[index].write(relaisSwitchPoint[index]);
    delay(500);
    servo[index].write(servoPos[index][0]);
    digitalWrite(relais[index], LOW);
  }
} // end of StartPosition
/*
   SetupWifi

   connect to network, install static IP address

*/
void SetupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(decoder, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("hostname: "));
  Serial.println(WiFi.hostname());
}
// end of SetupWifi

