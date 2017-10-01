/*

  Eltraco_Switch v1.0
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
#include <stdlib.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

WiFiClient espClient;
PubSubClient client(espClient);

#define WIFI_TX_POWER 0 // TX power of ESP module (0 -> 0.25dBm) (0...85)

// do not use as ID: 1 and 9
static char decoderId = 31;                              // also used in IP address decoder (check if IP address is available)
static char wiFiHostname[] = "ELTRACO-SW-31";            // Hostname displayed in OTA port

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
static const char *ssid = "SSID0";                                           // ssid WiFi network
static const char *password = "PWD0";                                        // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                            // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                         // IP address Mosquitto
IPAddress decoder(192, 168, 2, decoderId);                                   // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                           // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                          // subnet mask

#endif

#if WIFI_NETWORK == 1
static const char *ssid = "SSID1";                                           // ssid WiFi network
static const char *password = "PWD1";                                        // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                            // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                         // IP address Mosquitto
IPAddress decoder(192, 168, 2, decoderId);                                   // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                           // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                          // subnet mask

#endif

#if WIFI_NETWORK == 2
static const char *ssid = "SSID2";                                           // ssid WiFi network
static const char *password = "PWD2";                                        // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                            // MQTT client Id
IPAddress mosquitto(192, 168, 1, 254);                                       // IP address Mosquitto
IPAddress decoder(192, 168, 1, decoderId);                                   // IP address decoder
IPAddress gateway(192, 168, 1, 251);                                         // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                          // subnet mask

#endif

//////////////////////////////////////// decoder function selection ///////////////////////////////////////////////////

static const char *topicSub = "rocnet/ot";                                  // rocnet/ot for turnout control

static const byte outputNr = 8;                                             // 8 switches differs per decoder
static const byte output[outputNr] = {D0, D1, D2, D3, D4, D5, D6, D7};      // output pins
static byte outputId = 0;                                                   // one out of eight outputes

static boolean debugFlag = true;                                            // display debug messages

static boolean buf = false;
static byte bufId = 0;
static boolean order = false;                                               // orders sent by rocrail
static boolean orderOld = false;                                            // old orders to detect changes
static byte orderId = 0;
static byte orderIdOld = 0;
static String(orderoutput) = "";                                            // used with debugging
///////////////////////////////////////////////////////////////set-up//////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println();

  system_phy_set_max_tpw(WIFI_TX_POWER); //set as lower TX power as possible

  WiFi.hostname(wiFiHostname);
  setup_wifi();
  client.setServer(mosquitto, 1883);
  client.setCallback(callback);

  for (byte index = 0; index < outputNr; index++) {                         // initialising output pins
    pinMode(output[index], OUTPUT);
    digitalWrite(output[index], HIGH);
  }

  delay(1000);
  for (byte index = 0; index < outputNr; index++) {                         // initialising output pins
    pinMode(output[index], OUTPUT);
    digitalWrite(output[index], LOW);
  }

  client.setServer(mosquitto, 1883);
  client.setCallback(callback);

  //// begin of OTA ////////
  ArduinoOTA.setPort(8266);                                                 // Port defaults to 8266
  ArduinoOTA.setHostname(wiFiHostname);                                     // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setPassword((const char *)"123");                            // No authentication by default
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
}
///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  ArduinoOTA.handle();                                                   // OTA handle must stay here in loop
    ProcessOrder();


  if (!client.connected()) {                                             // maintain connection with Mosquitto
    reconnect();
  }
  client.loop();                                                         // content of client.loop can not be moved to function
}
///////////////////////////////////////////////////////////// end of program loop ///////////////////////

/*
   ProcessOrder

   function : check orders if action required

   called by: loop
   calls    : Thrown, Straight

*/
void ProcessOrder() {
  orderOld = order;
  order = buf;
  orderIdOld = orderId;
  orderId = bufId - 1;
  if ((orderOld != order) | (orderIdOld != orderId)) {
    digitalWrite(output[orderId], order);
  }
} // end of ProcessOrder()

/*

   callback

   function : receive incoming message, test topic, test recipient

*/
void callback(char *topic, byte * payload, unsigned int length) {
  if ((strncmp("rocnet/ot", topic, 9) == 0)) {
    if (((byte)payload[2]) == (decoderId)) {
      buf = ((byte) payload[10]);                                                 // switching order is stored
      bufId = ((byte) payload[11]);                                               // pin to switch
      if (debugFlag == true) {
        Serial.print(F("Message in ["));
        Serial.print(topic);
        Serial.print(F("] "));
        for (byte index = 0; index < length; index++) {
          Serial.print(((char)payload[index]), DEC);
          if (index < length - 1) Serial.print(F("."));
        }
        Serial.println();
        if (buf == 1) {
          orderoutput = "ON";
        } else {
          orderoutput = "OFF";
        }
        Serial.print(F("Message in ["));
        Serial.print(topic);
        Serial.print(F("] "));
        Serial.print(F("switch decoder "));
        Serial.print(((byte) payload[2]));
        Serial.print(F(" output "));
        Serial.print(bufId);
        Serial.print(F(" to "));
        Serial.print(orderoutput);
        Serial.println();
      }
    }
  }
} // end of callback

/*
   re-establish connection with MQTT clientID.
   publish topic topicPub. subscribe to topic
   when Mosquitto not available try again after 5 seconds

*/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Establishing connection with Mosquitto ...");
    // Attempt to connect
    if (client.connect(MQTTclientId)) {
      Serial.println("connected");
      client.subscribe(topicSub);                              // subscribe to topic
    } else {
      Serial.print("no Broker");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);                                             // Wait 5 seconds before retrying
    }
  }
}
// end of reconnect

/*
   setup_wifi

   connect to network, install static IP address

*/
void setup_wifi() {
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
// end of setup_wifi

