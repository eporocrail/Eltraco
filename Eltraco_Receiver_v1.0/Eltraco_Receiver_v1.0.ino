/*


  changelog:

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
#include <SoftwareSerial.h>
extern "C" {
#include "user_interface.h"
}

WiFiClient espClient;

#define WIFI_TX_POWER 0 // TX power of ESP module (0 -> 0.25dBm) (0...85)

static char decoderId = 149;                                      // also used in IP address decoder (check if IP address is available)
static char wiFiHostname[] = "ELTRACO-Receiver";                  // Hostname displayed in OTA port
///////////////////////
/*
   Define which Wifi Network is to be used
   0 = Network 0 train room
   1 = Network 1 test room


*/
#define WIFI_NETWORK 1

//////////////////////////// wifi network selection //////////////
#if WIFI_NETWORK == 0
static const char *ssid = "SSID1";                                        // ssid WiFi network
static const char *password = "paswrd1";                                // password WiFi network
#endif

#if WIFI_NETWORK == 1
static const char *ssid = "SSID2";                                            // ssid WiFi network
static const char *password = "paswrd2";                                   // password WiFi network
#endif

static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                        // IP address Mosquitto
IPAddress decoder(192, 168, 2, decoderId);                                  // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                          // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet masker
PubSubClient client(espClient, mosquitto);
///////////////////////////////////////////////////////////////////////////////////////////
//static const String topicSub = "rocnet/#";
static const String topicSub = "/#";
////////////////////////////// RFID reader /////////////////////////////
///////////////////////////////////////////////////////////////set-up//////////////////////////////
void setup() {
  Serial.begin(9600);                                        // the Serial port of Wemos baud rate.

  WiFi.hostname(wiFiHostname);

  system_phy_set_max_tpw(WIFI_TX_POWER);                     //set TX power as low as possible

  setup_wifi();
  client.set_callback(callback);

}
///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  if (!client.connected()) {                                                   // maintain connection with Mosquitto
    reconnect();
  }
  client.loop();                                                               // content of client.loop can not be moved to function
}
///////////////////////////////////////////////////////////// end of program loop ///////////////////////

/*
   callback

   function : receive incoming message, test topic, test on recepient, select returning sensor messages, select servo configuration,
   select incoming turnout switch orders, select turnout number, store order into buffer.

*/
void callback(const MQTT::Publish& pub) {
  Serial.println();
  Serial.print("Msg received [");
  Serial.print(pub.topic());
  Serial.print(" - DEC, dotted] <== ");
  for (char index = 0; index < (pub.payload_len()); index++) {
    Serial.print(((char)pub.payload()[index]), DEC);
    if (index < (pub.payload_len()) - 1) Serial.print(F("."));
  }
  Serial.println();
} // end of callbac

/*
   re-establish connection with MQTT clientID.
   publish topic topicPub. subscribe to topic topicSub.
   when Mosquitto not available try again after 5 seconds

*/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Establishing connection with Mosquitto ...");
    // Attempt to connect
    if (client.connect(MQTTclientId)) {
      Serial.println("connected");
      client.subscribe(topicSub);                              // and resubscribe to topic 1
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

