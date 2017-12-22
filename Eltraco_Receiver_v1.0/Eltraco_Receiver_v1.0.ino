/*

  changelog:
  "2017-12-21" Software version display added
      
  dec 2017-2
  "char" for numbers is WRONG!
  change to byte

  dec 2017:
  software module added to retrieve MQTT broker IP-address and portnumber from Rocrail server.
  software developed by Liviu, slightly adapted

  dec 2017:
  new MQTT library. supports qos2. link for library: https://github.com/Imroy/pubsubclient
  Length of MQTT outgoing message equal to Rocnet message length.
  "yield()" in loop.
  display of published and received messages on serial monitor harmonised.
  change "byte" to "char"

  Eltraco_Receiver v1.0

  Receiver for MQTT messages

*/

#include <PubSubClient.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
extern "C" {
#include "user_interface.h"
}

WiFiClient espClient;
WiFiUDP espUdpClient;
IPAddress broadcastAddr(224, 0, 0, 1);
uint16_t udpPort = 8051;


#define WIFI_TX_POWER 0 // TX power of ESP module (0 -> 0.25dBm) (0...85)

static byte decoderId = 149;                                      // also used in IP address decoder (check if IP address is available)
static char wiFiHostname[] = "ELTRACO-Receiver";                  // Hostname displayed in OTA port
static const String topicSub = "rocnet/#";
//static const String topicSub = "/#";                              // select topic from which message are received
static String version = "2017-12-21";
static String decoderType = "MQTT receiver Broker retrieval";
///////////////////////
/*
   Define which Wifi Network is to be used
   0 = Network 0 train room
   1 = Network 1 test room


*/
#define WIFI_NETWORK 1

//////////////////////////// wifi network selection //////////////
#if WIFI_NETWORK == 0
static const char *ssid = "SSID1";                                         // ssid WiFi network
static const char *password = "PASWRD1";                                   // password WiFi network
#endif

#if WIFI_NETWORK == 1
static const char *ssid = "SSID2";                                         // ssid WiFi network
static const char *password = "PASWRD12";                                  // password WiFi network
#endif
///////////////////////////////////////////////////////////////////////////////////////////


static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                      // default IP address Mosquitto to be replaced with retrieved version
uint16_t mqttPort = 1883;                                                   // default mosquitto port to be replaced with retrieved version
IPAddress decoder(192, 168, 2, decoderId);                                  // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                          // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet masker
PubSubClient client(espClient, mosquitto);
void Callback(const MQTT::Publish& pub);
/////////////////////////////////////////// IP-address retrieval /////////////////////////
typedef struct {
  IPAddress ip;
  uint16_t port;
} mqttBrokerDet_t;

mqttBrokerDet_t myMqttBrokerDet;
mqttBrokerDet_t detectIpPort(void);
static byte retrievalCounter = 0;
static byte retrievalNumber = 5;                              // number of retrieval attempts
static boolean brokerDefault = false;
///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////set-up//////////////////////////////
void setup() {
  Serial.begin(9600);                                        // the Serial port of Wemos baud rate.

  WiFi.hostname(wiFiHostname);

  system_phy_set_max_tpw(WIFI_TX_POWER);                     //set TX power as low as possible

  SetupWifi();
  client.set_callback(Callback);

  //start the UDP client, to retrieve the broker address from Rocrail
  espUdpClient.begin(udpPort);

  //broker address request. BLOCKING FUNCTION, doesn't return if Rocrail doesn't send the IP (adapted)
  myMqttBrokerDet = detectIpPort();
  if (brokerDefault == false) {
    Serial.print(F("using retrieved data"));
    client.set_server(myMqttBrokerDet.ip, myMqttBrokerDet.port);
  } else {
    Serial.print(F("using default data"));
    client.set_server(mosquitto, mqttPort);
  }
  Serial.println();
  Serial.println();
}
///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  if (!client.connected()) {                                                   // maintain connection with Mosquitto
    Reconnect();
  }
  client.loop();                                                               // content of client.loop can not be moved to function
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
   callback

   function : receive incoming message

*/
void Callback(const MQTT::Publish& pub) {
  Serial.println();
  Serial.print("Msg received [");
  Serial.print(pub.topic());
  Serial.print(" - DEC, dotted] <== ");
  for (byte index = 0; index < (pub.payload_len()); index++) {
    Serial.print(((byte)pub.payload()[index]), DEC);
    if (index < (pub.payload_len()) - 1) Serial.print(F("."));
  }
  Serial.println();
} // end of callbac

/*
   re-establish connection with MQTT clientID.
   subscribe to topic topicSub.
   when Mosquitto not available try again after 5 seconds

*/
void Reconnect() {
  while (!client.connected()) {
    Serial.print("Establishing connection with Mosquitto ...");
    // Attempt to connect
    if (client.connect(MQTTclientId)) {
      Serial.println("connected");
      client.subscribe(topicSub);                              // and resubscribe to topic 1
      SoftwareVersion();
    } else {
      Serial.print("no Broker");
      Serial.println(" try again in 1 second");
      delay(1000);                                             // Wait 1 second before retrying
    }
  }
}
// end of reconnect

/*
   setup_wifi

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

  Serial.print(F("\nWiFi connected using:"));
  Serial.print(F("\nIP-address: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("hostname:   "));
  Serial.println(WiFi.hostname());
}
// end of setup_wifi

//-------------------------------------------------------------

mqttBrokerDet_t detectIpPort(void) {
  mqttBrokerDet_t mqttBr;
  bool bAddressFound = false;
  char packetBuffer[80];
  uint16_t brokerPort = 0;
  char ipAsCharBuf[25];
  bool ipOk = false;
  bool portOk = false;

  if (WiFi.status() == WL_CONNECTED) {
    if (!espUdpClient) {
      espUdpClient.begin(udpPort);
    }
  }

  do {
    espUdpClient.beginPacketMulticast(broadcastAddr, udpPort, WiFi.localIP());
    espUdpClient.write("BROKER-GET");
    espUdpClient.endPacket();

    int packetSize = espUdpClient.parsePacket();

    if (packetSize) {
      int len = espUdpClient.read(packetBuffer, 80);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      String udpMess = String(packetBuffer);
      uint8_t brokerIpStart = udpMess.indexOf("BROKER-HOST");
      uint8_t brokerPortStart = udpMess.indexOf("BROKER-PORT");

      ipOk = false;
      if (brokerIpStart != -1) {
        String fullBrokerIp = udpMess.substring(11, brokerPortStart - 1);
        String ip = fullBrokerIp.substring(fullBrokerIp.indexOf(":") + 1, brokerPortStart);
        ip.toCharArray(ipAsCharBuf, ip.length() + 1);
        ipOk = true;
      }

      portOk = false;
      if (brokerPortStart != -1) {
        String brokerPortMessage = udpMess.substring(brokerPortStart);
        String brokerPortAsString = brokerPortMessage.substring(brokerPortMessage.lastIndexOf(":") + 1);
        brokerPort = brokerPortAsString.toInt();
        portOk = true;
      }

      if (ipOk && portOk) {
        bAddressFound = true;
        mqttBr.ip.fromString(ipAsCharBuf);
        mqttBr.port = brokerPort;
        Serial.print(F("\nMQTT broker details retrieved from Rocrail:"));
        Serial.print(F("\nIP-address: "));
        mqttBr.ip.printTo(Serial);
        Serial.print(F("\nPORT:       "));
        Serial.println(brokerPort);
        Serial.println();
      } //if(ipOk && portOk)
    }

    if (!bAddressFound) {
      delay(1000);
      if (retrievalCounter == 0) {
        Serial.println();
        Serial.println("retrieving MQTT broker data from Rocrail");
      }
      retrievalCounter++;
      Serial.print("retrieval attempt: ");
      Serial.println(retrievalCounter);
      if (retrievalCounter > retrievalNumber) {
        brokerDefault = true;
        retrievalCounter = 0;
        Serial.print(F("\nNo data from Rocrail retrieved"));
        Serial.println();
        Serial.println();
        break;
      }
    }
  } while (!bAddressFound);

  return (mqttBr);
} // end of MqttBrokerDet_t detectIpPort(void)
