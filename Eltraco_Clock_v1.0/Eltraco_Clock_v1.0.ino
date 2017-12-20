/*

  changelog:
      
  dec 2017-2
  "char" for numbers is WRONG!
  change to byte
  
  dec 2017:
  new MQTT library. supports qos2. link for library: https://github.com/Imroy/pubsubclient
  Length of MQTT outgoing message equal to Rocnet message length.
  "yield()" in loop.
  display of published and received messages on serial monitor harmonised.
  change "byte" to "char"
  

   version 0.2 Keepalive minute counter added

   ELTRACO Clock version 0.1

*/
#include <PubSubClient.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

extern "C" {
#include "user_interface.h"
}
#include <Wire.h>  
#include <SFE_MicroOLED.h>                           // Include the SFE_MicroOLED library


#define PIN_RESET 255 
#define DC_JUMPER 0                                  // I2C Addres: 0 - 0x3C, 1 - 0x3D

MicroOLED oled(PIN_RESET, DC_JUMPER);                // I2C Example

WiFiClient espClient;


#define WIFI_TX_POWER 0 // TX power of ESP module (0 -> 0.25dBm) (0...85)

// do not use as ID: 1 and 9
static byte decoderId = 248;                         // also used in IP address decoder (check if IP address is available)
static char wiFiHostname[] = "ELTRACO-Clock";        // Hostname displayed in OTA port

///////////////////////
/*
   Define which Wifi Network is to be used
   0 = Network 0 train room
   1 = Network 1 test room
   2 = Mobile demo layout

*/
#define WIFI_NETWORK 1
//////////////////////////// wifi network selection //////////////
#if WIFI_NETWORK == 0
static const char *ssid = "ssid1";                                         // ssid WiFi network
static const char *password = "password1";                                 // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                            // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                         // IP address Mosquitto
IPAddress decoder(192, 168, 2, decoderId);                                   // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                           // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                          // subnet masker

#endif

#if WIFI_NETWORK == 1
static const char *ssid = "ELP";                                             // ssid WiFi network
static const char *password = "password2";                                    // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                            // MQTT client Id
IPAddress mosquitto(192, 168, 2, 1);                                         // IP address Mosquitto
IPAddress decoder(192, 168, 2, decoderId);                                   // IP address decoder
IPAddress gateway(192, 168, 2, 1);                                           // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                          // subnet masker

#endif

#if WIFI_NETWORK == 2
static const char *ssid = "ssid1";                                      // ssid WiFi network
static const char *password = "password3";                                  // password WiFi network
static const char *MQTTclientId = (wiFiHostname);                           // MQTT client Id
IPAddress mosquitto(192, 168, 1, 254);                                      // IP address Mosquitto
IPAddress decoder(192, 168, 1, decoderId);                                  // IP address decoder
IPAddress gateway(192, 168, 1, 251);                                        // IP address gateway
IPAddress subnet(255, 255, 255, 0);                                         // subnet masker

#endif

//////////////////////////////////////// decoder function selection ///////////////////////////////////////////////////
static const String topicSub1 = "rocnet/ck";

static boolean debugFlag = true;                                            // display debug messages
/////////////////////////////////////////// clock //////////////////////////

// Use these variables to set the initial time
static byte hours = 8;
static byte minutes = 5;
static byte seconds = 30;

// How fast do you want the clock to spin? Set this to 1 for fun.
// Set this to 1000 to get _about_ 1 second timing.

static int CLOCK_SPEED = 1000;

// Global variables to help draw the clock face:
static const int MIDDLE_Y = oled.getLCDHeight() / 2;
static const int MIDDLE_X = oled.getLCDWidth() / 2;

static int CLOCK_RADIUS;
static int POS_12_X, POS_12_Y;
static int POS_3_X, POS_3_Y;
static int POS_6_X, POS_6_Y;
static int POS_9_X, POS_9_Y;
static int S_LENGTH;
static int M_LENGTH;
static int H_LENGTH;

static unsigned long lastDraw = 0;

static byte keepAliveCounter = 0;
PubSubClient client(espClient, mosquitto);
///////////////////////////////////////////////////////////////set-up//////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println();

  system_phy_set_max_tpw(WIFI_TX_POWER);               //set TX power as low as possible

  WiFi.hostname(wiFiHostname);
  setup_wifi();
  client.set_callback(callback);

  /////////////////////////////////////////// clock /////////////////////////
  oled.begin();                                        // Initialize the OLED
  oled.clear(PAGE);                                    // Clear the display's internal memory
  oled.clear(ALL);                                     // Clear the library's display buffer
  oled.display();                                      // Display what's in the buffer (splashscreen)

  initClockVariables();

  oled.clear(ALL);
  drawFace();
  drawArms(hours, minutes, seconds);
  oled.display();                                      // display the memory buffer drawn

}
///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  ArduinoOTA.handle();                                 // OTA handle must stay here in loop

  Clock();

  if (!client.connected()) {                           // maintain connection with Mosquitto
    reconnect();
  }
  client.loop();                                       // content of client.loop can not be moved to function
}
///////////////////////////////////////////////////////////// end of program loop ///////////////////////
void initClockVariables() {
  // Calculate constants for clock face component positions:
  oled.setFontType(0);
  //  CLOCK_RADIUS = min(MIDDLE_X, MIDDLE_Y) - 1;
  CLOCK_RADIUS = MIDDLE_Y - 1;
  POS_12_X = MIDDLE_X - oled.getFontWidth();
  POS_12_Y = MIDDLE_Y - CLOCK_RADIUS + 2;
  POS_3_X  = MIDDLE_X + CLOCK_RADIUS - oled.getFontWidth() - 1;
  POS_3_Y  = MIDDLE_Y - oled.getFontHeight() / 2;
  POS_6_X  = MIDDLE_X - oled.getFontWidth() / 2;
  POS_6_Y  = MIDDLE_Y + CLOCK_RADIUS - oled.getFontHeight() - 1;
  POS_9_X  = MIDDLE_X - CLOCK_RADIUS + oled.getFontWidth() - 2;
  POS_9_Y  = MIDDLE_Y - oled.getFontHeight() / 2;

  // Calculate clock arm lengths
  S_LENGTH = CLOCK_RADIUS - 2;
  M_LENGTH = S_LENGTH * 0.7;
  H_LENGTH = S_LENGTH * 0.5;
}

void Clock() {
  // Check if we need to update seconds, minutes, hours:
  if (lastDraw + CLOCK_SPEED < millis()) {
    lastDraw = millis();
    // Add a second, update minutes/hours if necessary:
    updateTime();

    // Draw the clock:
    if (keepAliveCounter < 75) {
      oled.clear(PAGE);                               // Clear the buffer
      drawFace();                                     // Draw the face to the buffer
      drawArms(hours, minutes, seconds);              // Draw arms to the buffer
      oled.display();                                 // Draw the memory buffer
    } else {
      oled.clear(PAGE);                               // Clear the buffer
      drawFace2();                                    // Draw the face to the buffer
      oled.display();                                 // Draw the memory buffer
    }
  }
}


void updateTime() {
  if (keepAliveCounter < 75) {
    keepAliveCounter++;
    seconds++;                                        // Increment seconds
    if (seconds >= 60) {                              // If seconds overflows (>=60)
      seconds = 0;                                    // Set seconds back to 0
    }
  } else {
    seconds = 0;

  }
}

// Draw the clock's three arms: seconds, minutes, hours.
void drawArms(int h, int m, int s) {
  double midHours;                                     // this will be used to slightly adjust the hour hand
  static int hx, hy, mx, my, sx, sy;

  // Adjust time to shift display 90 degrees ccw
  // this will turn the clock the same direction as text:
  h -= 3;
  m -= 15;
  s -= 15;
  if (h <= 0)
    h += 12;
  if (m < 0)
    m += 60;
  if (s < 0)
    s += 60;

  // Calculate and draw new lines:
  s = map(s, 0, 60, 0, 360);                           // map the 0-60, to "360 degrees"
  sx = S_LENGTH * cos(PI * ((float)s) / 180);
  sy = S_LENGTH * sin(PI * ((float)s) / 180);
  // draw the second hand:
  oled.line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + sx, MIDDLE_Y + sy);

  m = map(m, 0, 60, 0, 360);                          // map the 0-60, to "360 degrees"
  mx = M_LENGTH * cos(PI * ((float)m) / 180);  
  my = M_LENGTH * sin(PI * ((float)m) / 180); 
  // draw the minute hand
  oled.line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + mx, MIDDLE_Y + my);

  midHours = minutes / 12;                           // midHours is used to set the hours hand to middling levels between whole hours
  h *= 5;                                            // Get hours and midhours to the same scale
  h += midHours;                                     // add hours and midhours
  h = map(h, 0, 60, 0, 360);                         // map the 0-60, to "360 degrees"
  hx = H_LENGTH * cos(PI * ((float)h) / 180);  
  hy = H_LENGTH * sin(PI * ((float)h) / 180);  
  oled.line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + hx, MIDDLE_Y + hy);  // draw the hour hand
}

// Draw an analog clock face
void drawFace() {
  // Draw the clock border
  oled.circle(MIDDLE_X, MIDDLE_Y, CLOCK_RADIUS);

  // Draw the clock numbers
  oled.setFontType(0);                                // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(POS_12_X, POS_12_Y);                 // points cursor to x=27 y=0
  oled.print(12);
  oled.setCursor(POS_6_X, POS_6_Y);
  oled.print(6);
  oled.setCursor(POS_9_X, POS_9_Y);
  oled.print(9);
  oled.setCursor(POS_3_X, POS_3_Y);
  oled.print(3);
}

// Draw an analog clock face
void drawFace2() {
  oled.setFontType(0);                               // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(POS_9_X, POS_9_Y);
  oled.print("no synch");
}

/*

   callback

   function : receive incoming message, test topic, test recipient


*/
void callback(const MQTT::Publish& pub) {
  if ((pub.topic()) == ("rocnet/ck")) {
    hours = ((byte)pub.payload()[12]);
    minutes = ((byte)pub.payload()[13]);
    keepAliveCounter = 0;
    CLOCK_SPEED = 1000 / ((byte)pub.payload()[15]);;
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
} // end of callback

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
      client.subscribe(topicSub1);                              // subscribe to topic 1
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

