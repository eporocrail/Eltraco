#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EmbAJAX.h>
#include <EEPROM.h>
#include <Servo.h>
#include <stdlib.h>
#include <PubSubClient.h>
#include <FS.h>
#include <ESP8266FtpServer.h>
#include <Wire.h>
#include <ArduinoOTA.h>

#include "Defaults.h"

WiFiClient espClient;
ESP8266WebServer server(80);
EmbAJAXOutputDriverESP8266 driver(&server);
Servo servoMotor[2];
PubSubClient client(espClient);
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial


/////////////////////////////////////////////// General /////////////////////////////////////////////
EmbAJAXTextInput<BUFLENTRIP> triplet0("triplet0"); EmbAJAXMutableSpan triplet0_d("triplet0_d"); char triplet0_buf[BUFLENTRIP];
EmbAJAXTextInput<BUFLENTRIP> triplet1("triplet1"); EmbAJAXMutableSpan triplet1_d("triplet1_d"); char triplet1_buf[BUFLENTRIP];
EmbAJAXTextInput<BUFLENTRIP> triplet2("triplet2"); EmbAJAXMutableSpan triplet2_d("triplet2_d"); char triplet2_buf[BUFLENTRIP];
EmbAJAXTextInput<BUFLENTRIP> triplet3("triplet3"); EmbAJAXMutableSpan triplet3_d("triplet3_d"); char triplet3_buf[BUFLENTRIP];
EmbAJAXTextInput<BUFLENTRIP> triplet4("triplet4"); EmbAJAXMutableSpan triplet4_d("triplet4_d"); char triplet4_buf[BUFLENTRIP];
EmbAJAXTextInput<BUFLENTRIP> triplet5("triplet5"); EmbAJAXMutableSpan triplet5_d("triplet5_d"); char triplet5_buf[BUFLENTRIP];

EmbAJAXTextInput<BUFLENSSID> ssid("ssid"); EmbAJAXMutableSpan ssid_d("ssid_d"); char ssid_buf[BUFLENSSID];
EmbAJAXTextInput<BUFLENPWD> pwd("pwd"); EmbAJAXMutableSpan pwd_d("pwd_d"); char pwd_buf[BUFLENPWD];

const char* type_opts[] = {"single turnout", "double turnout", "sensor", "switch"};

EmbAJAXOptionSelect<5> type("type", type_opts); EmbAJAXMutableSpan type_d("type_d");

const char* debug_opts[] = {"no", "yes"}; EmbAJAXOptionSelect<2> debug("debug", debug_opts); EmbAJAXMutableSpan debug_d("debug_d");
const char* wifiReset_opts[] = {"no", "yes"}; EmbAJAXOptionSelect<2> wifiReset("wifiReset", wifiReset_opts); EmbAJAXMutableSpan wifiReset_d("wifiReset_d");
const char* initial_opts[] = {"no", "yes"}; EmbAJAXOptionSelect<2> initial("initial", initial_opts); EmbAJAXMutableSpan initial_d("initial_d");

///////////////////////////////////////////// servo1 //////////////////////////////////////////
EmbAJAXSlider sliderAngleB1("sliderAngleB1", 0, 180, 90); EmbAJAXMutableSpan sliderAngleB1_d("sliderAngleB1_d");
char sliderAngleB1_d_buf[BUFLENSLIDER];
EmbAJAXSlider sliderAngleA1("sliderAngleA1", 0, 180, 90); EmbAJAXMutableSpan sliderAngleA1_d("sliderAngleA1_d");
char sliderAngleA1_d_buf[BUFLENSLIDER];

EmbAJAXCheckButton adjustServo1("adjustServo1", ""); EmbAJAXMutableSpan adjustServo1_d("adjustServo1_d");
EmbAJAXCheckButton ackAngleA1("AngleA1", ""); EmbAJAXMutableSpan ackAngleA1_d("ackAngleA1_d");
EmbAJAXCheckButton ackAngleB1("ackAngleB1", ""); EmbAJAXMutableSpan ackAngleB1_d("ackAngleB1_d");

/////////////////////////////////////////// fine adjust ///////////////////////////////////////////
int A1_count = 90;
void upA1Pressed(EmbAJAXPushButton*) {
  A1_count++;
}
EmbAJAXPushButton upA1("upA1", "up", upA1Pressed);
EmbAJAXMutableSpan upA1_d("upA1_d");
char upA1_d_buf[BUFLENSLIDER];
void downA1Pressed(EmbAJAXPushButton*) {
  A1_count--;
}
EmbAJAXPushButton downA1("downA1", "down", downA1Pressed);
EmbAJAXMutableSpan downA1_d("downA1_d");
char downA1_d_buf[BUFLENSLIDER];
EmbAJAXCheckButton takeA1("takeA1", ""); EmbAJAXMutableSpan takeA1_d("takeA1_d");

int B1_count = 90;
void upB1Pressed(EmbAJAXPushButton*) {
  B1_count++;
}
EmbAJAXPushButton upB1("upB1", "up", upB1Pressed);
EmbAJAXMutableSpan upB1_d("upB1_d");
char upB1_d_buf[BUFLENSLIDER];
void downB1Pressed(EmbAJAXPushButton*) {
  B1_count--;
}
EmbAJAXPushButton downB1("downB1", "down", downB1Pressed);
EmbAJAXMutableSpan downB1_d("downB1_d");
char downB1_d_buf[BUFLENSLIDER];
EmbAJAXCheckButton takeB1("takeB1", ""); EmbAJAXMutableSpan takeB1_d("takeB1_d");

int A2_count = 90;
void upA2Pressed(EmbAJAXPushButton*) {
  A2_count++;
}
EmbAJAXPushButton upA2("upA2", "up", upA2Pressed);
EmbAJAXMutableSpan upA2_d("upA2_d");
char upA2_d_buf[BUFLENSLIDER];
void downA2Pressed(EmbAJAXPushButton*) {
  A2_count--;
}
EmbAJAXPushButton downA2("downA2", "down", downA2Pressed);
EmbAJAXMutableSpan downA2_d("downA2_d");
char downA2_d_buf[BUFLENSLIDER];
EmbAJAXCheckButton takeA2("takeA2", ""); EmbAJAXMutableSpan takeA2_d("takeA2_d");

int B2_count = 90;
void upB2Pressed(EmbAJAXPushButton*) {
  B2_count++;
}
EmbAJAXPushButton upB2("upB2", "up", upB2Pressed);
EmbAJAXMutableSpan upB2_d("upB2_d");
char upB2_d_buf[BUFLENSLIDER];
void downB2Pressed(EmbAJAXPushButton*) {
  B2_count--;
}
EmbAJAXPushButton downB2("downB2", "down", downB2Pressed);
EmbAJAXMutableSpan downB2_d("downB2_d");
char downB2_d_buf[BUFLENSLIDER];
EmbAJAXCheckButton takeB2("takeB2", ""); EmbAJAXMutableSpan takeB2_d("takeB2_d");

///////////////////////////////////////////// servo moving speeg //////////////////////////////////////////
EmbAJAXTextInput<BUFLENSRVSPD> srvSpd0("srvSpd0"); EmbAJAXMutableSpan srvSpd0_d("srvSpd0_d"); char srvSpd0_buf[BUFLENSRVSPD];
EmbAJAXTextInput<BUFLENSRVSPD> srvSpd1("srvSpd1"); EmbAJAXMutableSpan srvSpd1_d("srvSpd1_d"); char srvSpd1_buf[BUFLENSRVSPD];

///////////////////////////////////////////// servo2 //////////////////////////////////////////
EmbAJAXSlider sliderAngleB2("sliderAngleB2", 0, 180, 90); EmbAJAXMutableSpan sliderAngleB2_d("sliderAngleB2_d");
char sliderAngleB2_d_buf[BUFLENSLIDER];
EmbAJAXSlider sliderAngleA2("sliderAngleA2", 0, 180, 90); EmbAJAXMutableSpan sliderAngleA2_d("sliderAngleA2_d");
char sliderAngleA2_d_buf[BUFLENSLIDER];

EmbAJAXCheckButton adjustServo2("adjustServo2", ""); EmbAJAXMutableSpan adjustServo2_d("adjustServo2_d");
EmbAJAXCheckButton ackAngleA2("AngleA2", ""); EmbAJAXMutableSpan ackAngleA2_d("ackAngleA2_d");
EmbAJAXCheckButton ackAngleB2("ackAngleB2", ""); EmbAJAXMutableSpan ackAngleB2_d("ackAngleB2_d");

//////////////////////////////////////////////// swap servo angles //////////////////////////////////////////
EmbAJAXCheckButton swpServo0("swpServo0", ""); EmbAJAXMutableSpan swpServo0_d("swpServo0_d");
EmbAJAXCheckButton swpServo1("swpServo1", ""); EmbAJAXMutableSpan swpServo1_d("swpServo1_d");

/////////////////////////////////////// save acknowledgement /////////////////////////////////
EmbAJAXCheckButton checkGeneral("checkGeneral", ""); EmbAJAXMutableSpan checkGeneral_d("checkGeneral_d");
EmbAJAXCheckButton checkSingle("checkSingle", ""); EmbAJAXMutableSpan checkSingle_d("checkSingle_d");
EmbAJAXCheckButton checkDouble("checkDouble", ""); EmbAJAXMutableSpan checkDouble_d("checkDouble_d");
EmbAJAXCheckButton checkSensor("checkSensor", ""); EmbAJAXMutableSpan checkSensor_d("checkSensor_d");
EmbAJAXCheckButton checkOutput("checkOutput", ""); EmbAJAXMutableSpan checkOutput_d("checkOutput_d");

//////////////////////////////////////////// output /////////////////////////////////////////////
const char* output_opts[] = {"no", "yes"};
EmbAJAXOptionSelect<2> output0("output0", output_opts); EmbAJAXMutableSpan output0_d("output0_d");
EmbAJAXOptionSelect<2> output1("output1", output_opts); EmbAJAXMutableSpan output1_d("output1_d");
EmbAJAXOptionSelect<2> output2("output2", output_opts); EmbAJAXMutableSpan output2_d("output2_d");
EmbAJAXOptionSelect<2> output3("output3", output_opts); EmbAJAXMutableSpan output3_d("output3_d");
EmbAJAXOptionSelect<2> output4("output4", output_opts); EmbAJAXMutableSpan output4_d("output4_d");
EmbAJAXOptionSelect<2> output5("output5", output_opts); EmbAJAXMutableSpan output5_d("output5_d");
EmbAJAXOptionSelect<2> output6("output6", output_opts); EmbAJAXMutableSpan output6_d("output6_d");
EmbAJAXOptionSelect<2> output7("output7", output_opts); EmbAJAXMutableSpan output7_d("output7_d");

EmbAJAXTextInput<BUFLENPULS> pulsLength0("pulsLength0"); EmbAJAXMutableSpan pulsLength0_d("pulsLength0_d"); char pulsLength0_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength1("pulsLength1"); EmbAJAXMutableSpan pulsLength1_d("pulsLength1_d"); char pulsLength1_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength2("pulsLength2"); EmbAJAXMutableSpan pulsLength2_d("pulsLength2_d"); char pulsLength2_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength3("pulsLength3"); EmbAJAXMutableSpan pulsLength3_d("pulsLength3_d"); char pulsLength3_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength4("pulsLength4"); EmbAJAXMutableSpan pulsLength4_d("pulsLength4_d"); char pulsLength4_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength5("pulsLength5"); EmbAJAXMutableSpan pulsLength5_d("pulsLength5_d"); char pulsLength5_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength6("pulsLength6"); EmbAJAXMutableSpan pulsLength6_d("pulsLength6_d"); char pulsLength6_buf[BUFLENPULS];
EmbAJAXTextInput<BUFLENPULS> pulsLength7("pulsLength7"); EmbAJAXMutableSpan pulsLength7_d("pulsLength7_d"); char pulsLength7_buf[BUFLENPULS];

//////////////////////////////////////////// sensor /////////////////////////////////////////////
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce0("debounce0"); EmbAJAXMutableSpan debounce0_d("debounce0_d"); char debounce0_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce1("debounce1"); EmbAJAXMutableSpan debounce1_d("debounce1_d"); char debounce1_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce2("debounce2"); EmbAJAXMutableSpan debounce2_d("debounce2_d"); char debounce2_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce3("debounce3"); EmbAJAXMutableSpan debounce3_d("debounce3_d"); char debounce3_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce4("debounce4"); EmbAJAXMutableSpan debounce4_d("debounce4_d"); char debounce4_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce5("debounce5"); EmbAJAXMutableSpan debounce5_d("debounce5_d"); char debounce5_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce6("debounce6"); EmbAJAXMutableSpan debounce6_d("debounce6_d"); char debounce6_buf[BUFLENDEBOUNCE];
EmbAJAXTextInput<BUFLENDEBOUNCE> debounce7("debounce7"); EmbAJAXMutableSpan debounce7_d("debounce7_d"); char debounce7_buf[BUFLENDEBOUNCE];

EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh0("sensorHigh0"); EmbAJAXMutableSpan sensorHigh0_d("sensorHigh0_d"); char sensorHigh0_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh1("sensorHigh1"); EmbAJAXMutableSpan sensorHigh1_d("sensorHigh1_d"); char sensorHigh1_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh2("sensorHigh2"); EmbAJAXMutableSpan sensorHigh2_d("sensorHigh2_d"); char sensorHigh2_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh3("sensorHigh3"); EmbAJAXMutableSpan sensorHigh3_d("sensorHigh3_d"); char sensorHigh3_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh4("sensorHigh4"); EmbAJAXMutableSpan sensorHigh4_d("sensorHigh4_d"); char sensorHigh4_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh5("sensorHigh5"); EmbAJAXMutableSpan sensorHigh5_d("sensorHigh5_d"); char sensorHigh5_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh6("sensorHigh6"); EmbAJAXMutableSpan sensorHigh6_d("sensorHigh6_d"); char sensorHigh6_buf[BUFLENSENSORHIGH];
EmbAJAXTextInput<BUFLENSENSORHIGH> sensorHigh7("sensorHigh7"); EmbAJAXMutableSpan sensorHigh7_d("sensorHigh7_d"); char sensorHigh7_buf[BUFLENSENSORHIGH];

EmbAJAXCheckButton outputSensor0("outputSensor0", ""); EmbAJAXMutableSpan outputSensor0_d("outputSensor0_d");
EmbAJAXCheckButton outputSensor1("outputSensor1", ""); EmbAJAXMutableSpan outputSensor1_d("outputSensor1_d");
EmbAJAXCheckButton outputSensor2("outputSensor2", ""); EmbAJAXMutableSpan outputSensor2_d("outputSensor2_d");
EmbAJAXCheckButton outputSensor3("outputSensor3", ""); EmbAJAXMutableSpan outputSensor3_d("outputSensor3_d");
EmbAJAXCheckButton outputSensor4("outputSensor4", ""); EmbAJAXMutableSpan outputSensor4_d("outputSensor4_d");
EmbAJAXCheckButton outputSensor5("outputSensor5", ""); EmbAJAXMutableSpan outputSensor5_d("outputSensor5_d");
EmbAJAXCheckButton outputSensor6("outputSensor6", ""); EmbAJAXMutableSpan outputSensor6_d("outputSensor6_d");
EmbAJAXCheckButton outputSensor7("outputSensor7", ""); EmbAJAXMutableSpan outputSensor7_d("outputSensor7_d");

////////////////////////////////////////// static elements ////////////////////////////////////////////////
EmbAJAXStatic statics[] = {
  EmbAJAXStatic("<hr>"),
  EmbAJAXStatic("<space330><a href =\"/page2\">Single turnout</a></space330>"),
  EmbAJAXStatic("<space330><a href =\"/page3\">Double turnout</a></space330>"),
  EmbAJAXStatic("<space330><a href =\"/page4\">Switch</a></space330>"),
};
///////////////////////////////////////// table cell placing definition ////////////////////////////////////////////////
EmbAJAXStatic nextCell("</td><td>&nbsp;</td><td><b>");
EmbAJAXStatic nextRow("</b></td></tr><tr><td>");

/////////////////////////////////////////// hideable ///////////////////////////////////////////////////////
EmbAJAXBase* container1_contents[] = {&statics[1]};
EmbAJAXHideableContainer<1> container1("link1", container1_contents);

EmbAJAXBase* container2_contents[] = {&statics[2]};
EmbAJAXHideableContainer<1> container2("link2", container2_contents);

EmbAJAXBase* container3_contents[] = {&statics[3]};
EmbAJAXHideableContainer<1> container3("link3", container3_contents);

////////////////////////////////////////// construction of General page ////////////////////////////////////////
MAKE_EmbAJAXPage(page1, "General", "",
                 new EmbAJAXStatic("<head><style> space5 {margin-left: 5;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space10 {margin-left: 10;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space15 {margin-left: 15;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space36 {margin-left: 36;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space40 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space50 {margin-left: 50;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space55 {margin-left: 55;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space60 {margin-left: 60;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space75 {margin-left: 75;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space77 {margin-left: 77;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space90 {margin-left: 90;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space150 {margin-left: 150px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space290 {margin-left: 290px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space273 {margin-left: 273;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space286 {margin-left: 286;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space330 {margin-left: 330px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space380 {margin-left: 380px;}</style></head>"),

                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid lightblue;border-radius: 24px;font-size: 24px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {background-color: #00878F;color: white;font-family: 'Arial', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {box-shadow: 1px 1px 5px #555555;padding: 2px;width: 425pxpx;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Arial', sans-serif;font-size: 18px;text-align: center;color: black;background-color:#99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {border: 2px solid #99ffcc;border-radius: 24px;padding: 2px;width: 300px;margin-left: 95px;}</style></head>"),

                 new EmbAJAXStatic("<h1>ELTRACO</h1>"),
                 new EmbAJAXStatic("<h2>General configuration</h2>"),

                 new EmbAJAXStatic("<table cellpadding=\"3\"><tr><td>"),
                 new EmbAJAXStatic("Link status:"), &nextCell,
                 new EmbAJAXConnectionIndicator(), &nextCell, &checkGeneral, &checkGeneral_d,
                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Wifi network</ins></b></space40>"),
                 new EmbAJAXStatic("<br><br><space10>SSID</space10>"),
                 new EmbAJAXStatic("<space36>"), &ssid, &ssid_d,
                 new EmbAJAXStatic("</space36>"),
                 new EmbAJAXStatic("<br><space10>Password</space10>"),
                 new EmbAJAXStatic("<space10>"), &pwd, &pwd_d,
                 new EmbAJAXStatic("</space10>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>IP-addres</ins></b> &nbsp&nbsp&nbsp each field value < 256 </space40>"),
                 new EmbAJAXStatic("<br><br><space10>router</space10>"),
                 new EmbAJAXStatic("<space15>"), &triplet0,
                 new EmbAJAXStatic("</space15>"),
                 new EmbAJAXStatic("<space15>"), &triplet1,
                 new EmbAJAXStatic("</space15>"),
                 new EmbAJAXStatic("<space15>"), &triplet2,
                 new EmbAJAXStatic("</space15>"),
                 new EmbAJAXStatic("<space15>"), &triplet3, &triplet3_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<br><space75>"), &triplet0_d,
                 new EmbAJAXStatic("</space75>"),
                 new EmbAJAXStatic("<space75>"), &triplet1_d,
                 new EmbAJAXStatic("</space75>"),
                 new EmbAJAXStatic("<space75>"), &triplet2_d,
                 new EmbAJAXStatic("</space75>"),
                 new EmbAJAXStatic("<br><space10>mosquitto</space10>"),
                 new EmbAJAXStatic("<space273>"), &triplet4, &triplet4_d,
                 new EmbAJAXStatic("</space273>"),
                 new EmbAJAXStatic("<br><space10>decoder</space10>"),
                 new EmbAJAXStatic("<space286>"), &triplet5, &triplet5_d,
                 new EmbAJAXStatic("</space286>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Function</b></ins></space40>"),
                 new EmbAJAXStatic("<br><br><space10>decoder type</space10><space36>"), &type, &type_d,
                 new EmbAJAXStatic("</space36>"),
                 new EmbAJAXStatic("<br><br><space10>debug</space10><space77>"), &debug, &debug_d,
                 new EmbAJAXStatic("</space77>"),
                 new EmbAJAXStatic("<p style=\"font-size:75%;\"><space330>Reset ALL values</p></space330>"),
                 new EmbAJAXStatic("<space10>wifiReset</space10><space55>"), &wifiReset, &wifiReset_d,
                 new EmbAJAXStatic("</space55>"),
                 new EmbAJAXStatic("<space90>initial settings</space90><space10>"), &initial, &initial_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<p style=\"font-size:75%;\"><space330>Reset ALL values</p></space330>"),

                 new EmbAJAXStatic(""), &statics[0],

                 new EmbAJAXStatic(""), &container1,
                 new EmbAJAXStatic("<br>"), &container2,
                 new EmbAJAXStatic("<br>"), &container3,
                )
///////////////////////////////////////////////////////// construction single turnout page //////////////////////////////////////
MAKE_EmbAJAXPage(page2, "Single", "",
                 new EmbAJAXStatic("<head><style> space5 {margin-left: 5;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space10 {margin-left: 10;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space20 {margin-left: 20;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space40 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space50 {margin-left: 50;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space78 {margin-left: 78;}</style></head>"),

                 new EmbAJAXStatic("<head><style> space90 {margin-left: 90;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space103 {margin-left: 103;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space185 {margin-left: 185;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space150 {margin-left: 150px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space330 {margin-left: 330px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space380 {margin-left: 380px;}</style></head>"),

                 new EmbAJAXStatic("<head><style> div {margin-left: 50px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 35;}</style></head>"),

                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid lightblue;border-radius: 24px;font-size: 24px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {background-color: #00878F;color: white;font-family: 'Arial', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {box-shadow: 1px 1px 5px #555555;padding: 2px;width: 425pxpx;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Arial', sans-serif;font-size: 18px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {border: 2px solid #99ffcc;border-radius: 24px;padding: 2px;width: 190px;margin-left: 145px;}</style></head>"),

                 new EmbAJAXStatic("<h1>ELTRACO</h1>"),
                 new EmbAJAXStatic("<h2>single turnout</h2>"),

                 new EmbAJAXStatic("<table cellpadding=\"3\"><tr><td>"),
                 new EmbAJAXStatic("Link status:"), &nextCell,
                 new EmbAJAXConnectionIndicator(), &nextCell,  &checkSingle, &checkSingle_d,
                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 ///////////////////////////////////////// first turnout ///////////////////////////////////////
                 new EmbAJAXStatic("<space40><b><ins>Adjust servo</ins></b><br><br></space40>"),
                 new EmbAJAXStatic("<space50>"), &adjustServo1,
                 new EmbAJAXStatic("</space50>"),
                 new EmbAJAXStatic("<space5>"), &adjustServo1_d,
                 new EmbAJAXStatic("</space5>"),

                 new EmbAJAXStatic("<br><br><space90>"), &sliderAngleA1,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleA1_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeA1, &takeA1_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downA1,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upA1,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleA1,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleA1_d,
                 new EmbAJAXStatic("</space10>"),

                 new EmbAJAXStatic("<br><space90>"), &sliderAngleB1,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleB1_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeB1, &takeB1_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downB1,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upB1,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleB1,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleB1_d,

                 new EmbAJAXStatic("</space10>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Servo speed</ins></b> &nbsp&nbsp&nbsp higher number - slower speed (default: 40)</space40>"),

                 new EmbAJAXStatic("<br><br><space40>""Servo1"),
                 new EmbAJAXStatic("</space40>"),
                 new EmbAJAXStatic("<space20>"), &srvSpd0, &srvSpd0_d,

                 new EmbAJAXStatic(""), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Swap angles</ins></b></space40>"),
                 new EmbAJAXStatic("<br><br><space40>""Servo1"),
                 new EmbAJAXStatic("</space40>"),
                 new EmbAJAXStatic("<space20>"), &swpServo0, &swpServo0_d,
                 new EmbAJAXStatic("</space20>"), &statics[0],

                 new EmbAJAXStatic("<space380><a href=\"/\">General</a></space380>"),
                )

///////////////////////////////////////////////////////// construction double turnout page //////////////////////////////////////
MAKE_EmbAJAXPage(page3, "Double", "",
                 new EmbAJAXStatic("<head><style> space5 {margin-left: 5;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space10 {margin-left: 10;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space20 {margin-left: 20;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space40 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space50 {margin-left: 50;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space78 {margin-left: 78;}</style></head>"),

                 new EmbAJAXStatic("<head><style> space90 {margin-left: 90;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space103 {margin-left: 103;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space185 {margin-left: 185px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space330 {margin-left: 330px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space380 {margin-left: 380px;}</style></head>"),

                 new EmbAJAXStatic("<head><style> div {margin-left: 50px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 35;}</style></head>"),

                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid lightblue;border-radius: 24px;font-size: 24px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {background-color: #00878F;color: white;font-family: 'Arial', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {box-shadow: 1px 1px 5px #555555;padding: 2px;width: 425pxpx;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Arial', sans-serif;font-size: 18px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {border: 2px solid #99ffcc;border-radius: 24px;padding: 2px;width: 200px;margin-left: 140px;}</style></head>"),

                 new EmbAJAXStatic("<h1>ELTRACO</h1>"),
                 new EmbAJAXStatic("<h2>double turnout</h2>"),

                 new EmbAJAXStatic("<table cellpadding=\"3\"><tr><td>"),
                 new EmbAJAXStatic("Link status:"), &nextCell,
                 new EmbAJAXConnectionIndicator(), &nextCell,  &checkDouble, &checkDouble_d,
                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 ///////////////////////////////////////// first turnout ///////////////////////////////////////
                 new EmbAJAXStatic("<space40><b><ins>Adjust servo</ins></b></space40><br><br>"),
                 new EmbAJAXStatic("<space50>"), &adjustServo1,
                 new EmbAJAXStatic("</space50>"),
                 new EmbAJAXStatic("<space5>"), &adjustServo1_d,
                 new EmbAJAXStatic("</space5>"),

                 new EmbAJAXStatic("<br><br><space90>"), &sliderAngleA1,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleA1_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeA1, &takeA1_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downA1,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upA1,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleA1,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleA1_d,
                 new EmbAJAXStatic("</space10>"),

                 new EmbAJAXStatic("<br><space90>"), &sliderAngleB1,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleB1_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeB1, &takeB1_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downB1,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upB1,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleB1,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleB1_d,

                 new EmbAJAXStatic("</space10>"), &statics[0],
                 ///////////////////////////////////////// second turnout ///////////////////////////////////////
                 new EmbAJAXStatic("<space50>"), &adjustServo2,
                 new EmbAJAXStatic("</space50>"),
                 new EmbAJAXStatic("<space5>"), &adjustServo2_d,
                 new EmbAJAXStatic("</space5>"),

                 new EmbAJAXStatic("<br><br><space90>"), &sliderAngleA2,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleA2_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeA2, &takeA2_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downA2,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upA2,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleA2,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleA2_d,
                 new EmbAJAXStatic("</space10>"),

                 new EmbAJAXStatic("<br><space90>"), &sliderAngleB2,
                 new EmbAJAXStatic("</space90>"),
                 new EmbAJAXStatic("<space10>"), &sliderAngleB2_d,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space20>"), &takeB2, &takeB2_d,
                 new EmbAJAXStatic("</space20>"),
                 new EmbAJAXStatic("<br><space103>"), &downB2,
                 new EmbAJAXStatic("</space103>"),
                 new EmbAJAXStatic("<space10>"), &upB2,
                 new EmbAJAXStatic("</space10>"),
                 new EmbAJAXStatic("<space78>"), &ackAngleB2,
                 new EmbAJAXStatic("</space78>"),
                 new EmbAJAXStatic("<space10>"), &ackAngleB2_d,

                 new EmbAJAXStatic("</space10>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Servo speed</ins></b> &nbsp&nbsp&nbsp higher number - slower speed (default: 40)</space40>"),

                 new EmbAJAXStatic("<br><br><space40>""Servo1</space40>"),
                 new EmbAJAXStatic("<space20>"), &srvSpd0, &srvSpd0_d,
                 new EmbAJAXStatic("</space20><br><space40>""Servo2</space40>"),
                 new EmbAJAXStatic("<space20>"), &srvSpd1, &srvSpd1_d,
                 new EmbAJAXStatic("</space20>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Swap angles</ins></b></space40>"),
                 new EmbAJAXStatic("<br><br><space40>""Servo1</space40>"),
                 new EmbAJAXStatic("<space20>"), &swpServo0, &swpServo0_d,
                 new EmbAJAXStatic("</space20><br><space40>""Servo2</space40>"),
                 new EmbAJAXStatic("<space20>"), &swpServo1, &swpServo1_d,
                 new EmbAJAXStatic("</space20>"), &statics[0],

                 new EmbAJAXStatic("<space380><a href=\"/\">General</a></space380>"),
                )


///////////////////////////////////////////////////////// output page  //////////////////////////////////////
MAKE_EmbAJAXPage(page4, "Output", "",
                 new EmbAJAXStatic("<head><style> space20 {margin-left: 20;}* {box-sizing: border-box;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space40 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space150 {margin-left: 150px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space330 {margin-left: 330px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> space380 {margin-left: 380px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 35;}</style></head>"),

                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid lightblue;border-radius: 24px;font-size: 24px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {background-color: #00878F;color: white;font-family: 'Arial', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {box-shadow: 1px 1px 5px #555555;padding: 2px;width: 425pxpx;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Arial', sans-serif;font-size: 18px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {border: 2px solid #99ffcc;border-radius: 24px;padding: 2px;width: 140px;margin-left: 170px;}</style></head>"),

                 new EmbAJAXStatic("<h1>ELTRACO</h1>"),
                 new EmbAJAXStatic("<h2>Output</h2>"),

                 new EmbAJAXStatic("<table cellpadding=\"3\"><tr><td>"),
                 new EmbAJAXStatic("Link status:"), &nextCell,
                 new EmbAJAXConnectionIndicator(), &nextCell, &checkOutput, &checkOutput_d,
                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Output pulse</b></ins></space40><br><br>"),
                 new EmbAJAXStatic("<table cellpadding=\"11\"><tr><td>"),
                 new EmbAJAXStatic(""), &nextCell,
                 new EmbAJAXStatic("<b>output 1</b>"), &nextCell,
                 new EmbAJAXStatic("output 2"), &nextCell,
                 new EmbAJAXStatic("output 3"), &nextCell,
                 new EmbAJAXStatic("output 4"),
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &output0, &nextCell, &output1, &nextCell, &output2, &nextCell, &output3,
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &output0_d, &nextCell, &output1_d, &nextCell, &output2_d, &nextCell, &output3_d, &nextRow, &nextCell,

                 new EmbAJAXStatic("output 5"), &nextCell,
                 new EmbAJAXStatic("output 6"), &nextCell,
                 new EmbAJAXStatic("output 7"), &nextCell,
                 new EmbAJAXStatic("output 8"),
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &output4, &nextCell, &output5, &nextCell, &output6, &nextCell, &output7,
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &output4_d, &nextCell, &output5_d, &nextCell, &output6_d, &nextCell, &output7_d,

                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Pulse width</b></ins></space40> &nbsp&nbsp&nbsp (default: 200)<br><br>"),
                 new EmbAJAXStatic("<table cellpadding=\"6\"><tr><td>"),
                 new EmbAJAXStatic(""), &nextCell,
                 new EmbAJAXStatic("<b>output 1</b>"), &nextCell,
                 new EmbAJAXStatic("output 2"), &nextCell,
                 new EmbAJAXStatic("output 3"), &nextCell,
                 new EmbAJAXStatic("output 4"),
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &pulsLength0, &nextCell, &pulsLength1, &nextCell, &pulsLength2, &nextCell, &pulsLength3,
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &pulsLength0_d, &nextCell, &pulsLength1_d, &nextCell, &pulsLength2_d, &nextCell, &pulsLength3_d, &nextRow, &nextCell,

                 new EmbAJAXStatic("output 5"), &nextCell,
                 new EmbAJAXStatic("output 6"), &nextCell,
                 new EmbAJAXStatic("output 7"), &nextCell,
                 new EmbAJAXStatic("output 8"),
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &pulsLength4, &nextCell, &pulsLength5, &nextCell, &pulsLength6, &nextCell, &pulsLength7,
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &pulsLength4_d, &nextCell, &pulsLength5_d, &nextCell, &pulsLength6_d, &nextCell, &pulsLength7_d,

                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 new EmbAJAXStatic("<space40><b><ins>Sensor message</b></ins></space40><br><br>"),
                 new EmbAJAXStatic("<table cellpadding=\"11\"><tr><td>"),
                 new EmbAJAXStatic(""), &nextCell,
                 new EmbAJAXStatic("<b>output 1</b>"), &nextCell,
                 new EmbAJAXStatic("output 2"), &nextCell,
                 new EmbAJAXStatic("output 3"), &nextCell,
                 new EmbAJAXStatic("output 4"),
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &outputSensor0, &nextCell, &outputSensor1, &nextCell, &outputSensor2, &nextCell, &outputSensor3,
                 new EmbAJAXStatic(""), &nextRow, &nextCell, &outputSensor0_d, &nextCell, &outputSensor1_d, &nextCell, &outputSensor2_d, &nextCell, &outputSensor3_d, &nextRow, &nextCell,

                 new EmbAJAXStatic("output 5"), &nextCell,
                 new EmbAJAXStatic("output 6"), &nextCell,
                 new EmbAJAXStatic("output 7"), &nextCell,
                 new EmbAJAXStatic("output 8"),
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &outputSensor4, &nextCell, &outputSensor5, &nextCell, &outputSensor6, &nextCell, &outputSensor7,
                 new EmbAJAXStatic(""),  &nextRow, &nextCell, &outputSensor4_d, &nextCell, &outputSensor5_d, &nextCell, &outputSensor6_d, &nextCell, &outputSensor7_d,

                 new EmbAJAXStatic("</b></td></tr></table>"), &statics[0],

                 new EmbAJAXStatic("<space380><a href=\"/\">General</a></space380>"),
                )

// One page handler for each page.
void handlePage1() {
  if (server.method() == HTTP_POST) { // AJAX request
    page1.handleRequest(UpdateUI);
  } else {  // Page load
    page1.print();
  }
}

void handlePage2() {
  if (server.method() == HTTP_POST) { // AJAX request
    page2.handleRequest(UpdateUI);
  } else {  // Page load
    page2.print();
  }
}

void handlePage3() {
  if (server.method() == HTTP_POST) { // AJAX request
    page3.handleRequest(UpdateUI);
  } else {  // Page load
    page3.print();
  }
}

void handlePage4() {
  if (server.method() == HTTP_POST) { // AJAX request
    page4.handleRequest(UpdateUI);
  } else {  // Page load
    page4.print();
  }
}

//////////////////////////////////////////////////// setup /////////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.print("\n\n\n\n");
  PrintHeader("Starting up");
  StartEEPROM();                                    // initialise EEPROM
  StartSPIFFS();                                    // initialise SPIFFS
  InitialSettings();                                // initial settings in EEPROM
  system_phy_set_max_tpw(txPower);                        // TX power of ESP module (0 -> 0.25dBm) (0...85)
  PageHandler();                                    // controls the configuration webpages
  LoadGeneral();                                    // load General decoder settings from EEPROM
  LoadServo();                                      // load servo settings from EEPROM
  LoadOutput();                                     // load output settings from EEPROM
  StartAP();                                        // start WiFi Access Point and
  StartFtp();                                     // start FTP server
  SelectConfig();                                   // configure actual decoder type
  if (FLwifiReset == false) {
    CreateName();                                   // generate decoder name
    StartWifi();                                    // start WiFi ST mode
    if (FLwifiEstablished == true) {
      StartMQTT();                                    // start Mosquitto Client
      Reconnect();                                    // connect to Mosquitto
      StartOTA();
      StartupScan();
      SoftwareVersion();                              // display actual decoder type and software version
    } else {
      PrintWarning("No WiFi connection. Change parameters via configuration");
    }
  } else {
    PrintWarning("WIFI      N O T     operational. Change parameters via configuration");
  }
  startUpReady = true;
  PrintHeader("Start up finished");
} // end of setup
//////////////////////////////////////////////////// end of setup /////////////////////////
///////////////////////////////////////////////////// loop ///////////////////////////////
void loop() {
  yield();                                            // to prevent awakening the watch dog
  server.handleClient();                              // configuration website server
  ftpSrv.handleFTP();                                 // handle for FTP server
  ArduinoOTA.handle();                                // Over The Air interrupt
  UpdateGeneral();
  UpdateServo();
  UpdateOutput();

  if (FLwifiReset == false) {

    if (WiFi.status() != WL_CONNECTED) {            // maintain WiFi connection
      StartWifi();
    }
    if (!client.connected()) {                      // maintain connection with Mosquitto
      Reconnect();
    }

    MQTTHartBeat();                                 // to prevent MQT connection being interrupted
    client.loop();                                  // message reception interrupt

    switch (flow) {
      case 0:
        Controller();                              // General flow control
        break;
      case 1:
        ScanInterrupt();                           // scanning sensor interrupts
        CheckTurnoutOrder();                           // check turnout order buffer
        break;
      case 2:
        PolD0();                                   // poll D0 sensor, which is not interrupt capable
        ScanInterrupt();                           // scanning sensor interrupts
        break;
      case 3:
        CheckOutputOrder();                        // check output order
        break;
      case 11:                                     // free running
        yield();
        break;
      default:
        FLonce = false;
        PrintDefault("Should not happen - Loop");
        break;
    }
  }
}
////////////////////////////////////////////////// end of loop ////////////////////////////


/*

   Controller

   function : control program flow
   called by: loop

*/
void Controller() {
  switch (decoderType) {
    case Single:
      flow = 1;
      break;
    case Double:
      flow = 1;
      break;
    case Sensor:
      flow = 2;
      break;
    case Switch:
      flow = 3;
      break;
    case None:
      flow = 11;
      break;
    default:
      FLonce = false;
      PrintDefault("Should not happen - Controller decoderType");
      break;
  }

  switch (reboot) {
    case 0:
      break;
    case WifiIp:
      PrintHeader("changing to different WiFi network");
      FLreboot = true;
      break;
    case DecoderIp:
      PrintHeader("after R E B O O T new IP-address in use");
      PrintHeader("Login on same Configuration network");
      FLreboot = true;
      break;
    case DecoderType:
      PrintHeader("changing decoder Type");
      FLreboot = true;
      break;
    case NoWifi:
      PrintWarning("NO WIFI connection. Change parameters via configuration");
      FLreboot = true;
      break;
    case NoMqtt:
      PrintWarning("NO MQTT Connection. Change IP triplet via configuration");
      FLreboot = true;
      break;
    default:
      FLonce = false;
      PrintDefault("Should not happen - Controller reboot");
      break;
  }
  if (FLreboot == true) {
    PrintHeader("R E B O O T");
    waitTimer = millis();
    Wait(3000);
    ESP.restart();
  }
} // end of Controller

/*

   CheckTurnoutOrder

   function : converts buffer into turnout order, execute order
   called by: loop
   calls .  : Straight, Thrown

*/
void CheckTurnoutOrder() {
  if (toOrder.executed == true) {
    if (toBuffer.orderNew == true) {
      toBuffer.orderNew = false;
      toOrder.in = toBuffer.order;
      toOrder.targetId = toBuffer.targetId;
      toOrder.executed = false;
      toOrder.msgMove = false;
      toOrder.msgStop = false;
    }
  }
  if (toOrder.executed == false) {
    if (FLdetachServo == true) servoMotor[toOrder.targetId - 1].attach (servo[toOrder.targetId - 1].Pin);
    if (servo[toOrder.targetId - 1].Swap == false) {
      if ((toOrder.in == 1)) {
        Thrown(toOrder.targetId - 1);
      }
      if ((toOrder.in == 0)) {
        Straight(toOrder.targetId - 1);
      }
    } else {
      if ((toOrder.in == 0)) {
        Thrown(toOrder.targetId - 1);
      }
      if ((toOrder.in == 1)) {
        Straight(toOrder.targetId - 1);
      }
    }
  }
} //end of CheckTurnoutOrder

/*
  Thrown

  function : move servo into thrown position
             compare current servo position with ordered position, signal start servo movement,
             output relais in middle of servo movement, signal end of movement, set flag order executed
  called by: CheckTurnoutOrder
  calls    : TMSM, TSSM

*/
byte Thrown(byte toNmbr) {
  byte angle = servo[toNmbr].AngleB;
  if (toNmbr != 255) {
    currentPosition[toNmbr] = servoMotor[toNmbr].read();
    if ((currentPosition[toNmbr]) != (angle) && millis() >= servo[toNmbr].MoveTime) {
      if (servo[toNmbr].Invert == true) {  /////////////////////
        servoMotor[toNmbr].write((currentPosition[toNmbr]) - 1);
      } else servoMotor[toNmbr].write((currentPosition[toNmbr]) + 1);
      servo[toNmbr].MoveTime = millis() + servo[toNmbr].Spd;
      if (toOrder.msgMove == false) {
        toOrder.msgMove = true;
        TMSM(sensor[toNmbr].Address);
      }
      if ((currentPosition[toNmbr]) == (relais[toNmbr].SwitchPoint)) digitalWrite(relais[toNmbr].Pin, HIGH);
    }
    if ((currentPosition[toNmbr]) == (angle)) {
      toOrder.executed = true;
      if (toOrder.msgStop == false) {
        toOrder.msgStop = true;
        if (FLdetachServo == true) servoMotor[toNmbr].detach();
        TSSM(sensor[toNmbr].Address);
      }
    }
  }
} // end of Thrown

/*
   Straight

   function : move servo into straight position
              compare current servo position with ordered position, signal start servo movement
              output relais in middle of servo movement, signal end of movement, set flag order executed
   called by: CheckTurnoutOrder
   calls    : TMSM, TSSM

*/
byte Straight(byte toNmbr) {
  byte angle = servo[toNmbr].AngleA;
  if (toNmbr != 255) {
    currentPosition[toNmbr] = servoMotor[toNmbr].read();
    if ((currentPosition[toNmbr]) != (angle) && millis() >= servo[toNmbr].MoveTime) {
      if (servo[toNmbr].Invert == true) {
        servoMotor[toNmbr].write((currentPosition[toNmbr]) + 1);
      } else servoMotor[toNmbr].write((currentPosition[toNmbr]) - 1);
      servo[toNmbr].MoveTime = millis() + servo[toNmbr].Spd;
      if (toOrder.msgMove == false) {
        toOrder.msgMove = true;
        TMSM(sensor[toNmbr].Address);
      }
      if ((currentPosition[toNmbr]) == (relais[toNmbr].SwitchPoint)) digitalWrite(relais[toNmbr].Pin, LOW);
    }
    if ((currentPosition[toNmbr]) == (angle)) {
      toOrder.executed = true;
      if (toOrder.msgStop == false) {
        toOrder.msgStop = true;
        if (FLdetachServo == true) servoMotor[toNmbr].detach();
        TSSM(sensor[toNmbr].Address);
      }
    }
  }
} // end of Straight


/*
   CheckOutputOrder

   function : scan order state, output HIGH and if pulse port output LOW after pulse duration
   output immediately LOW, set order state executed
   after high state duration send state LOW
   called by: loop
   calls    : SM

*/
void CheckOutputOrder() {

  for (byte index = 0; index < outputNr; index++) {

    if ((output[index].State == HIGH) && (output[index].Pulse == true) && (output[index].Executed == false)) {

      if (millis() > (output[index].Timer + output[index].PulseDuration)) {
        digitalWrite((output[index].Pin ) , LOW);
        output[index].State = LOW;
        output[index].Executed = true;
        if (output[index].SensorOn == false) SM(index + 1, LOW);
      }
    }

    if (output[index].Order != output[index].State) {
      if ((output[index].Order == HIGH) && (output[index].Executed == false)) {
        digitalWrite((output[index].Pin ) , HIGH);
        if (output[index].Pulse == false)  output[index].Executed = true;
        output[index].State = HIGH;
        output[index].Timer = millis();
        if (output[index].SensorOn == false) SM(index + 1, HIGH);
      }

      if ((output[index].Order == LOW) && (output[index].Pulse == false) && (output[index].Executed == false)) {
        digitalWrite((output[index].Pin ) , LOW);
        output[index].Executed = true;
        output[index].State = LOW;
        if (output[index].SensorOn == false) SM(index + 1, LOW);
      }

    }
  }
} // end of CheckOutputOrder

/*

   PolD0

   function : read status of D0
   called by: loop

*/
void PolD0() {   // only for sensor decoder
  D0StateOld = D0State;
  D0State = !digitalRead(sensor[0].Pin);
  if ((D0StateOld != D0State)) {
    sensor[0].Itr = true;
    sensor[0].ItrTime = millis();
  }
} // end of PolD0

/*

   ISR0

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR0() {
  sensor[0].Itr = true;
  sensor[0].ItrTime = millis();
} // end of ISR0

/*

   ISR1

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR1() {
  sensor[1].Itr = true;
  sensor[1].ItrTime = millis();
} // end of ISR1

/*

   ISR2

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR2() {

  sensor[2].Itr = true;
  sensor[2].ItrTime = millis();
} // end of ISR2

/*

   ISR3

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR3() {
  sensor[3].Itr = true;
  sensor[3].ItrTime = millis();
} // end of ISR3

/*

   ISR4

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR4() {

  sensor[4].Itr = true;
  sensor[4].ItrTime = millis();
} // end of ISR4

/*

   ISR5

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR5() {
  sensor[5].Itr = true;
  sensor[5].ItrTime = millis();
} // end of ISR5

/*

   ISR6

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR6() {
  sensor[6].Itr = true;
  sensor[6].ItrTime = millis();
} // end of ISR6

/*

   ISR7

   function : adjust sensor state to interrupt, record change time
   called by: interrupt

*/
void ISR7() {
  sensor[7].Itr = true;
  sensor[7].ItrTime = millis();
} // end of ISR7

/*

   ScanInterrupt

   function : scan interrupt state, after debounce time send sensor state HIGH,
   after high state duration send state LOW
   called by: loop
   calls    : SM

*/
void ScanInterrupt() {
  boolean scan = false;
  int counter;
  for (byte index = 0; index < sensorNr; index++) {
    counter = 0;
    if (sensor[index].Itr == true) {
      if ( millis() > sensor[index].ItrTime + sensor[index].Debounce) {
        sensor[index].StateOld = sensor[index].State;
        scan = !digitalRead(sensor[index].Pin);
        if (scan == false) {
          sensor[index].CountOff++;
        }
        if (scan == true) {
          sensor[index].CountOff = 0;
          sensor[index].State = true;
        }
        if (sensor[index].CountOff > sensor[index].ScanNegativeNr) {
          sensor[index].State = false;
        }
        if (sensor[index].StateOld != sensor[index].State) {
          if (sensor[index].State == false) {
            do {
              counter++;
              if (counter % 150 == 0) yield();
            } while (millis() < (sensor[index].ItrTime + sensorPositiveTime));
          }
          SM(index + 1, (sensor[index].State));
        }
      }

    }
  }
} // end of ScanInterrupt

/*
  MQTTHartBeat

  function : send msg to Mosquitto ever 10 sec to keep the connection alive
  called by: loop
  calls    : HBM

*/
void MQTTHartBeat() {
  if ((millis() - HBSendTime) > HBPeriod) {
    HBSendTime = millis();
    HBM();
  }
} // end of MQTTHartBeat

/*
   SM

   function : transmit sensor signal
   called by: ScanSensor
   calls    : PrintMsgOutContent, SendMessage

*/
void SM(byte pin, byte state) {                                   // Sensor Message
  FixedFieldsSr();
  msgOut[10] = state;
  msgOut[11] = pin;
  PrintMsgOutContent("SM");
  SendMessage();
} // end of SM

/*
  HBM

  function : send destination track number, loc detection and rank of detection
  called by: MQTTHartBeat
  calls    : PrintMsgOutContent, SendMessage, FixedFieldsSr

*/
void HBM() {                                  //  Hart Beat Message
  topicPub = "eltraco/hb";
  msgOut[0] = decoderId;                      // sendId
  msgOut[1] = 0;                              // "0" for HB, "1" for all, "decoderId" for single
  msgOut[2] = 111;                            // HBM
  msgOut[3] = 0;                              // number following data byte
  msgLengthOut = 4 + msgOut[3];
  PrintMsgOutContent("HBM");
  SendMessage();
  FixedFieldsSr();
} // end of HBM

/*
   TMSM Turnout Move Sensor Message
   function : signal turnout is moving
   called by: Thrown, Straight

*/
byte TMSM(byte msNmbr) {
  msgOut[11] = msNmbr;
  msgOut[10] = 1;
  PrintMsgOutContent("TMSM");
  SendMessage();
} // end of TMSM

/*
   TSSM Turnout Stop Sensor Message

   function : signal turnout stops moving
   called by: Thrown, Straight

*/
byte TSSM(byte msNmbr) {
  msgOut[11] = msNmbr;
  msgOut[10] = 0;
  PrintMsgOutContent("TSSM");
  SendMessage();
} // end of TSSM

/*
  FixedFieldsSr()

  function : fill fixed values outgoing sensor message
  called by: messages

*/
void FixedFieldsSr() {
  topicPub = "rocnet/sr";
  msgOut[0] = 0;            // group
  msgOut[1] = 0;            // receiveId H
  msgOut[2] = 1;            // receiveId L
  msgOut[3] = 0;            // sendId H
  msgOut[4] = decoderId;    // sendIdL
  msgOut[5] = 8;            // group
  msgOut[6] = 1;            // code
  msgOut[7] = 4;            // number following data bytes
  msgOut[8] = 0;
  msgOut[9] = 0;
  msgLengthOut = 8 + msgOut[7];
} // end of FixedFields()


/*
   PrintMsgOutContent

   function : display message to be sent
   called by: messages

*/
void PrintMsgOutContent(String text) {
  if (FLdebug == true) {
    boolean FLprint = false;
    if (topicPub != "eltraco/hb") FLprint = true;
    if (FLshowHB == true) FLprint = true;
    if (FLprint == true) {
      Serial.print("\n");
      Serial.print(F("Publish msg  ["));
      Serial.print(topicPub);
      Serial.print(F(" - DEC, dotted] "));
      Serial.print(text);
      Serial.print(F(" ==> "));
      for (int index = 0 ; index < msgLengthOut ; index++) {
        Serial.print((msgOut[index]), DEC);
        if (index < msgLengthOut - 1) Serial.print(F("."));
      }
      Serial.print("\n");
    }
  }
} // end of PrintMsgOutContent

/*
   SendMessage

   function : send structured message
   called by: all structured messages

*/
void SendMessage() {
  if (client.publish(MQTT::Publish(topicPub, msgOut, msgLengthOut).set_qos(2)) != true) {     // message is published with qos2, highest
    Serial.print(F("\nfault publishing"));                                                 // guarantee for delivery of message to Mosquitto
  }
} // end of SendMessage

/*
  UpdateUI

  Function : update return value of variables on screen, set control flags
  Called by: UpdateGeneral, LoadGeneral, UpdateServo, LoadServo, UpdateOutput, LoadOutput
  handlePage1, handlePage2, handlePage3, handlePage4

*/
void UpdateUI() {
  container1.setVisible((type.selectedOption() == 0));
  container2.setVisible((type.selectedOption() == 1));
  container3.setVisible((type.selectedOption() == 3));

  triplet0_d.setValue(strncpy(triplet0_buf, triplet0.value(), BUFLENTRIP));
  triplet1_d.setValue(strncpy(triplet1_buf, triplet1.value(), BUFLENTRIP));
  triplet2_d.setValue(strncpy(triplet2_buf, triplet2.value(), BUFLENTRIP));
  triplet3_d.setValue(strncpy(triplet3_buf, triplet3.value(), BUFLENTRIP));
  triplet4_d.setValue(strncpy(triplet4_buf, triplet4.value(), BUFLENTRIP));
  triplet5_d.setValue(strncpy(triplet5_buf, triplet5.value(), BUFLENTRIP));

  ssid_d.setValue(strncpy(ssid_buf, ssid.value(), BUFLENSSID));
  pwd_d.setValue(strncpy(pwd_buf, pwd.value(), BUFLENPWD));

  type_d.setValue(type_opts[type.selectedOption()]);
  debug_d.setValue(debug_opts[debug.selectedOption()]);
  wifiReset_d.setValue(wifiReset_opts[wifiReset.selectedOption()]);
  initial_d.setValue(initial_opts[initial.selectedOption()]);

  checkGeneral_d.setValue(checkGeneral.isChecked() ? "saved" : "NOT saved, check box");

  if (checkGeneral.isChecked() == 1) FLupdateGeneral = true; else FLupdateGeneral = false;

  /////////////////////////// first turnout //////////////////
  sliderAngleA1_d.setValue(itoa(sliderAngleA1.intValue(), sliderAngleA1_d_buf, 10));
  sliderAngleB1_d.setValue(itoa(sliderAngleB1.intValue(), sliderAngleB1_d_buf, 10));

  adjustServo1_d.setValue(adjustServo1.isChecked() ? "Adjusting" : "Start adjusting Servo 1");
  ackAngleA1_d.setValue(ackAngleA1.isChecked() ? "confirmed" : "confirm");
  ackAngleB1_d.setValue(ackAngleB1.isChecked() ? "confirmed" : "confirm");

  if (adjustServo1.isChecked() == 1) FLadjustServo1 = true; else FLadjustServo1 = false;
  if (ackAngleA1.isChecked() == 1) FLackAngleA1 = true; else FLackAngleA1 = false;
  if (ackAngleB1.isChecked() == 1) FLackAngleB1 = true; else FLackAngleB1 = false;

  //////////////////////////// fine tuning ////////////////////
  takeA1_d.setValue(takeA1.isChecked() ? "tuning" : "fine tune angle - A");
  if (takeA1.isChecked() == true) {
    if (FLtakeA1 == false) {
      FLtakeA1 = true;
      A1_count = sliderAngleA1.intValue();
    }
    upA1_d.setValue(itoa(sliderAngleA1.intValue(), upA1_d_buf, 10));
    downA1_d.setValue(itoa(sliderAngleA1.intValue(), upA1_d_buf, 10));
    sliderAngleA1.setValue(A1_count);
    sliderAngleA1_d.setValue(itoa(sliderAngleA1.intValue(), sliderAngleA1_d_buf, 10));
  }
  if (takeA1.isChecked() == false) {
    FLtakeA1 = false;
  }

  takeB1_d.setValue(takeB1.isChecked() ? "tuning" : "fine tune angle - B");
  if (takeB1.isChecked() == true) {
    if (FLtakeB1 == false) {
      FLtakeB1 = true;
      B1_count = sliderAngleB1.intValue();
    }
    upB1_d.setValue(itoa(sliderAngleB1.intValue(), upB1_d_buf, 10));
    downB1_d.setValue(itoa(sliderAngleB1.intValue(), upB1_d_buf, 10));
    sliderAngleB1.setValue(B1_count);
    sliderAngleB1_d.setValue(itoa(sliderAngleB1.intValue(), sliderAngleB1_d_buf, 10));
  }
  if (takeB1.isChecked() == false) {
    FLtakeB1 = false;
  }

  /////////////////////////// second turnout /////////////////
  sliderAngleB2_d.setValue(itoa(sliderAngleB2.intValue(), sliderAngleB2_d_buf, 10));
  sliderAngleA2_d.setValue(itoa(sliderAngleA2.intValue(), sliderAngleA2_d_buf, 10));

  adjustServo2_d.setValue(adjustServo2.isChecked() ? "Adjusting" : "Start adjusting Servo 2");
  ackAngleB2_d.setValue(ackAngleB2.isChecked() ? "confirmed" : "confirm");
  ackAngleA2_d.setValue(ackAngleA2.isChecked() ? "confirmed" : "confirm");

  //////////////////////////// fine tuning ////////////////////
  takeA2_d.setValue(takeA2.isChecked() ? "tuning" : "fine tune angle - A");
  if (takeA2.isChecked() == true) {
    if (FLtakeA2 == false) {
      FLtakeA2 = true;
      A2_count = sliderAngleA2.intValue();
    }
    upA2_d.setValue(itoa(sliderAngleA2.intValue(), upA2_d_buf, 10));
    downA2_d.setValue(itoa(sliderAngleA2.intValue(), upA2_d_buf, 10));
    sliderAngleA2.setValue(A2_count);
    sliderAngleA2_d.setValue(itoa(sliderAngleA2.intValue(), sliderAngleA2_d_buf, 10));
  }
  if (takeA2.isChecked() == false) {
    FLtakeA2 = false;
  }

  takeB2_d.setValue(takeB2.isChecked() ? "tuning" : "fine tune angle - B");
  if (takeB2.isChecked() == true) {
    if (FLtakeB2 == false) {
      FLtakeB2 = true;
      B2_count = sliderAngleB2.intValue();
    }
    upB2_d.setValue(itoa(sliderAngleB2.intValue(), upB2_d_buf, 10));
    downB2_d.setValue(itoa(sliderAngleB2.intValue(), upB2_d_buf, 10));
    sliderAngleB2.setValue(B2_count);
    sliderAngleB2_d.setValue(itoa(sliderAngleB2.intValue(), sliderAngleB2_d_buf, 10));
  }
  if (takeB2.isChecked() == false) {
    FLtakeB2 = false;
  }

  srvSpd0_d.setValue(strncpy(srvSpd0_buf, srvSpd0.value(), BUFLENSRVSPD));
  srvSpd1_d.setValue(strncpy(srvSpd1_buf, srvSpd1.value(), BUFLENSRVSPD));

  /////////////////////////////////////////////////////////////////////////////////////////
  if (adjustServo2.isChecked() == 1) FLadjustServo2 = true; else FLadjustServo2 = false;
  if (ackAngleB2.isChecked() == 1) FLackAngleB2 = true; else FLackAngleB2 = false;
  if (ackAngleA2.isChecked() == 1) FLackAngleA2 = true; else FLackAngleA2 = false;

  swpServo0_d.setValue(swpServo0.isChecked() ? "swapped" : "NOT swapped");
  swpServo1_d.setValue(swpServo1.isChecked() ? "swapped" : "NOT swapped");

  checkSingle_d.setValue(checkSingle.isChecked() ? "saved" : "NOT saved, check box");
  checkDouble_d.setValue(checkDouble.isChecked() ? "saved" : "NOT saved, check box");
  if (checkSingle.isChecked() == 1) FLupdateSingle = true; else FLupdateSingle = false;
  if (checkDouble.isChecked() == 1) FLupdateDouble = true; else FLupdateDouble = false;

  //////////////////////////////// output /////////////////////////
  checkOutput_d.setValue(checkOutput.isChecked() ? "saved" : "NOT saved, check box");
  if (checkOutput.isChecked() == 1) FLupdateOutput = true; else FLupdateOutput = false;
  output0_d.setValue(output_opts[output0.selectedOption()]);
  output1_d.setValue(output_opts[output1.selectedOption()]);
  output2_d.setValue(output_opts[output2.selectedOption()]);
  output3_d.setValue(output_opts[output3.selectedOption()]);
  output4_d.setValue(output_opts[output4.selectedOption()]);
  output5_d.setValue(output_opts[output5.selectedOption()]);
  output6_d.setValue(output_opts[output6.selectedOption()]);
  output7_d.setValue(output_opts[output7.selectedOption()]);

  pulsLength0_d.setValue(strncpy(pulsLength0_buf, pulsLength0.value(), BUFLENPULS));
  pulsLength1_d.setValue(strncpy(pulsLength1_buf, pulsLength1.value(), BUFLENPULS));
  pulsLength2_d.setValue(strncpy(pulsLength2_buf, pulsLength2.value(), BUFLENPULS));
  pulsLength3_d.setValue(strncpy(pulsLength3_buf, pulsLength3.value(), BUFLENPULS));
  pulsLength4_d.setValue(strncpy(pulsLength4_buf, pulsLength4.value(), BUFLENPULS));
  pulsLength5_d.setValue(strncpy(pulsLength5_buf, pulsLength5.value(), BUFLENPULS));
  pulsLength6_d.setValue(strncpy(pulsLength6_buf, pulsLength6.value(), BUFLENPULS));
  pulsLength7_d.setValue(strncpy(pulsLength7_buf, pulsLength7.value(), BUFLENPULS));

  outputSensor0_d.setValue(outputSensor0.isChecked() ? "no" : "yes");
  outputSensor1_d.setValue(outputSensor1.isChecked() ? "no" : "yes");
  outputSensor2_d.setValue(outputSensor2.isChecked() ? "no" : "yes");
  outputSensor3_d.setValue(outputSensor3.isChecked() ? "no" : "yes");
  outputSensor4_d.setValue(outputSensor4.isChecked() ? "no" : "yes");
  outputSensor5_d.setValue(outputSensor5.isChecked() ? "no" : "yes");
  outputSensor6_d.setValue(outputSensor6.isChecked() ? "no" : "yes");
  outputSensor7_d.setValue(outputSensor7.isChecked() ? "no" : "yes");

} // end of UpdateUI

/*
  UpdateGeneral

  Function : transfer web page values to variables, store variables to EEPROM, read variables from EEPROM
  print variables to serial monitor
  Called by: Loop
  Calls    : WriteEEPROMGeneral, ReadEEPROMGeneral, PrintGeneral, UpdateUI, WriteWifiReset, ReadWifiReset

*/
void UpdateGeneral() {
  if (FLupdateGeneral == true) {
    FLupdateGeneral = false;
    byte a[3];
    boolean b[3];
    memset(IP, 0, sizeof(IP));
    memset(netName, 0, sizeof(netName));
    memset(passWord, 0, sizeof(passWord));
    memset(a, 0, sizeof(a));
    memset(b, false, sizeof(b));

    strncpy(IP[0], triplet0_buf, strlen(triplet0_buf));
    strncpy(IP[1], triplet1_buf, strlen(triplet1_buf));
    strncpy(IP[2], triplet2_buf, strlen(triplet2_buf));
    strncpy(IP[3], triplet3_buf, strlen(triplet3_buf));
    strncpy(IP[4], triplet4_buf, strlen(triplet4_buf));
    strncpy(IP[5], triplet5_buf, strlen(triplet5_buf));

    for (int index1 = 0 ; index1 < 6; index1++) {
      for (int index = 0 ; index < 3; index++) {
        if ((IP[index1][index] > 47) && (IP[index1][index] < 59)) {
          a[index] = byte(IP[index1][index] - 48);
          b[index] = true;
        }
      }
      if ((b[0] == 1) && (b[1] == 0) && (b[2] == 0)) {
        IPTriplet[index1] = a[0];
      }
      if ((b[0] == 1) && (b[1] == 1) && (b[2] == 0)) {
        IPTriplet[index1] = (a[0] * 10) + a[1];
      }
      if ((b[0] == 1) && (b[1] == 1) && (b[2] == 1)) {
        IPTriplet[index1] =  (a[0] * 100) + (a[1] * 10) + a[2];
      }
      memset(a, 0, sizeof(a));
      memset(b, false, sizeof(b));
    }

    strncpy(netName, ssid_buf, strlen(ssid_buf));
    strncpy(passWord, pwd_buf, strlen(pwd_buf));

    FLdebug = byte((debug.selectedOption()));
    decoderTypeOld = decoderType;
    decoderType = byte((type.selectedOption()));
    FLwifiResetOld = FLwifiReset;
    FLwifiReset = byte((wifiReset.selectedOption()));
    FLinitial = byte((initial.selectedOption()));

    WriteWifiReset();
    ReadWifiReset();
    WriteEEPROMGeneral();
    ReadEEPROMGeneral();
    WriteInitial();
    ReadInitial();

    PrintGeneral();

    checkGeneral.setChecked(false);
    UpdateUI();

    if ((FLwifiResetOld == 0) && (FLwifiReset == 1))  {
      PrintHeader("change to different WiFi network");
      PrintHeader("R E B O O T");
      waitTimer = millis();
      Wait(3000);
      ESP.restart();
    }
    if (decoderTypeOld != decoderType) {
      PrintHeader("changing decoder Type");
      PrintHeader("R E B O O T");
      waitTimer = millis();
      Wait(3000);
      ESP.restart();
    }
    if (decoderId != IPTriplet[5]) {
      PrintHeader("after R E B O O T new IP-address in use");
      PrintHeader("Login on N E W Cofiguration network");
      waitTimer = millis();
      Wait(3000);
      ESP.restart();
    }
    PrintGeneral();
  }
} // end of UpdateGeneral

/*
  LoadGeneral

  function : load initial values from EEPROM into web page
  called by: setup
  calls    : ReadEEPROMGeneral, ReadWifiReset, UpdateUI, PrintGeneral, PrintDefault

*/
void LoadGeneral() {
  char tri[4];
  byte a = 0;
  byte b = 0;
  byte c = 0;

  ReadEEPROMGeneral();
  ReadWifiReset();

  for (int index = 0 ; index < 6; index++) {
    a = (IPTriplet[index]) / 100;
    b = (IPTriplet[index] % 100) / 10;
    c = (IPTriplet[index] % 10);
    tri[0] = a + 48;
    tri[1] = b + 48;
    tri[2] = c + 48;
    tri[3] = 0;

    switch (index) {
      case 0:
        triplet0.setValue(tri);
        break;
      case 1:
        triplet1.setValue(tri);
        break;
      case 2:
        triplet2.setValue(tri);
        break;
      case 3:
        triplet3.setValue(tri);
        break;
      case 4:
        triplet4.setValue(tri);
        break;
      case 5:
        triplet5.setValue(tri);
        break;
      default:
        FLonce = false;
        PrintDefault("Should not happen - LoadGeneral");
        break;
    }
  }
  decoderId = IPTriplet[5];
  debug.selectOption(FLdebug);
  type.selectOption(decoderType);
  wifiReset.selectOption(FLwifiReset);

  ssid.setValue(netName);
  pwd.setValue(passWord);

  UpdateUI();
  PrintGeneral();

} // end of LoadGeneral

/*
  PrintGeneral

  function : print values loaded from EEPROM to serial monitor
  called by: UpdateGeneral, LoadGeneral
  calls    : PrintText, PrintHeader, PrintDefault

*/
void PrintGeneral() {
  PrintHeader("WiFi ST mode parameter");
  PrintText("SSID:       " + String(netName), false );
  PrintText("password:   " + String(passWord), false);
  PrintText("IP addresses", false);
  PrintText("router:     " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
            String(IPTriplet[2]) + "." + String(IPTriplet[3]), false);
  PrintText("mosquitto:  " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
            String(IPTriplet[2]) + "." + String(IPTriplet[4]), false);
  PrintText("decoder:    " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
            String(IPTriplet[2]) + "." + String(IPTriplet[5]), false);
  PrintText("netmask:    " + String("255") + "." + String("255") + "." + String("255") + \
            "." + String("0"), false);
  PrintHeader("General parameter");
  switch (decoderType) {
    case Single:
      PrintText("type:       single turnout", false);
      break;
    case Double:
      PrintText("type:       double turnout", false);
      break;
    case Sensor:
      PrintText("type:       sensor", false);
      break;
    case Switch:
      PrintText("type:       output", false);
      break;
    default:
      FLonce = false;
      PrintDefault("Should not happen - PrintGeneral");
      break;
  }
  if (FLdebug == true) PrintText("debug:      ON", false);
  if (FLdebug == false) PrintText("debug:      OFF", false);
  if (FLwifiReset == true) PrintText("wifiReset:  ON", false);
  if (FLwifiReset == false) PrintText("wifiReset:  OFF", false);
  if (FLinitial == true) PrintText("initial:    ON", false);
  if (FLinitial == false) PrintText("initial:    OFF", false);
} // end of PrintGeneral

/*
   WriteEEPROMGeneral

   function : write General decoder values to EEPROM
   called by: UpdateGeneral

*/
void WriteEEPROMGeneral() {
  EEPROM.write(8, decoderType);
  EEPROM.write(9, FLdebug);

  for (int index = 0 ; index < 26; index++) {
    EEPROM.write(_GENERAL + index, netName[index]);
  }

  for (int index = 0 ; index < 9; index++) {
    EEPROM.write(_GENERAL + _SSID + index, passWord[index]);
  }

  for (int index = 0 ; index < 6; index++) {
    EEPROM.write(_GENERAL + _SSID + _PWD + index, IPTriplet[index]);
  }

  EEPROM.commit();
} // end of WriteEEPROMGeneral

/*
   ReadEEPROMGeneral

   function : read General decoder values from EEPROM
   called by: UpdateGeneral, LoadGeneral

*/
void ReadEEPROMGeneral() {
  decoderType = EEPROM.read(8);
  FLdebug = EEPROM.read(9);

  for (int index = 0 ; index < 26; index++) {
    netName[index] = EEPROM.read(_GENERAL + index);
  }

  for (int index = 0 ; index < 9; index++) {
    passWord[index] = EEPROM.read(_GENERAL + _SSID + index);
  }

  for (int index = 0 ; index < 6; index++) {
    IPTriplet[index] = EEPROM.read(_GENERAL + _SSID + _PWD + index);
  }

} // end of ReadEEPROMGeneral

/*
  UpdateServo

  Function : controls servo, transfer web page values to variables, store variables to EEPROM, read variables from EEPROM
  print variables to serial monitor, set control flags
  Called by: loop
  Calls    : ServoMove, WriteEEPROMServo1, WriteEEPROMServo2, ReadEEPROMServo1, ReadEEPROMServo1
             UpdateUI, PrintServo

*/
void UpdateServo() {
  boolean flag = false;
  ///////////// first turnout //////////////////
  if ((FLadjustServo1 == true) && (FLadjustServo2 == false)) {
    if ((FLackAngleA1 == false) && (FLackAngleB1 == false)) {
      servo[0].AngleA = byte(sliderAngleA1.intValue());
      ServoMove(0, servo[0].AngleA);
    }

    if ((FLackAngleA1 == true) && (FLackAngleB1 == false)) {
      servo[0].AngleB = byte(sliderAngleB1.intValue());
      ServoMove(0, servo[0].AngleB);
    }

    if ((FLackAngleA1 == true) && (FLackAngleB1 == true)) {  // cycle concluded when both acks are received
      WriteEEPROMServo1();
      FLadjustServo1 = false;
      FLackAngleA1 = false;
      FLackAngleB1 = false;
      ackAngleA1.setChecked(false);
      ackAngleB1.setChecked(false);
      adjustServo1.setChecked(false);
      takeA1.setChecked(false);
      takeB1.setChecked(false);
      flag = true;
    }

  }

  ///////////// second turnout //////////////////
  if ((FLadjustServo2 == true) && (decoderType = 1)) {
    if ((FLackAngleA2 == false) && (FLackAngleB2 == false)) {
      servo[1].AngleA = byte(sliderAngleA2.intValue());
      ServoMove(1, servo[1].AngleA);
    }

    if ((FLackAngleA2 == true) && (FLackAngleB2 == false)) {
      servo[1].AngleB = byte(sliderAngleB2.intValue());
      ServoMove(1, servo[1].AngleB);
    }

    if ((FLackAngleA2 == true) && (FLackAngleB2 == true)) {  // cycle concluded when both acks are received
      WriteEEPROMServo2();
      FLadjustServo2 = false;
      FLackAngleA2 = false;
      FLackAngleB2 = false;
      ackAngleA2.setChecked(false);
      ackAngleB2.setChecked(false);
      adjustServo2.setChecked(false);
      takeA2.setChecked(false);
      takeB2.setChecked(false);
      flag = true;
    }
  }

  if ((FLupdateSingle == true) || (FLupdateDouble == true)) {
    if ((swpServo0.isChecked()) == 0) {
      servo[0].Swap = false;
    } else servo[0].Swap = true;
    servo[0].Spd = atoi(srvSpd0_buf);
    checkSingle.setChecked(false);
    WriteEEPROMServo1();
    flag = true;
  }

  if (FLupdateDouble == true) {
    if ((swpServo1.isChecked()) == 0) {
      servo[1].Swap = false;
    } else servo[1].Swap = true;
    servo[1].Spd = atoi(srvSpd1_buf);
    checkDouble.setChecked(false);
    WriteEEPROMServo2();
    flag = true;
  }

  if (flag == true) {
    ReadEEPROMServo1();
    ReadEEPROMServo2();
    UpdateUI();
    PrintServo();
  }

} // end of UpdateServo

/*
  ServoMove

  function : move servo
  called by: UpdateServo
  calls    : PrintWarning

*/
void ServoMove(byte id, byte angle) {
  byte servoAngleOld = 0;
  if ((angle) != (servoAngleOld)) {
    servoMotor[id].write(angle);                                // servo takes position
    servoAngleOld = servoMotor[id].read();
  } else {
    servoMoveCount ++;
    if (servoMoveCount == 5) {
      if (FLservoMovePrint == false) {
        FLservoMovePrint = true;
        PrintWarning("No servo movement");
      }
    }
  }

} //end of ServoMove

/*
  LoadServo

  function : load initial values from EEPROM into web page
  Called by: UpdateServo, LoadServo
  Calls    : ReadEEPROMServo1, ReadEEPROMServo2, UpdateUI, PrintServo

*/
void LoadServo() {

  ReadEEPROMServo1();
  ReadEEPROMServo2();

  sliderAngleA1.setValue(servo[0].AngleA);
  sliderAngleB1.setValue(servo[0].AngleB);
  ByteToAsc(servo[0].Spd);
  srvSpd0.setValue(BTA);

  sliderAngleA2.setValue(servo[1].AngleA);
  sliderAngleB2.setValue(servo[1].AngleB);
  ByteToAsc(servo[1].Spd);
  srvSpd1.setValue(BTA);

  swpServo0.setChecked(servo[0].Swap);
  swpServo1.setChecked(servo[1].Swap);

  UpdateUI();
  PrintServo();
} // end of LoadServo

/*
  PrintServo

  function : print values from EEPROM to serial monitor
  called by: UpdateServo, LoadServo
  Calls    : PrintText, PrintHeader

*/
void PrintServo() {
  if ((decoderType == 0) || (decoderType == 1) || (FLshowBootAll == true)) {
    PrintHeader("Servo parameter");
    PrintText("servo 1:    angle A - " + String(servo[0].AngleA), false);
    PrintText("            angle B - " + String(servo[0].AngleB), false);
    PrintText("            speed -   " + String(servo[0].Spd), false);
    if (servo[0].Swap == false) {
      PrintText("            angle NOT swapped", false);
    } else {
      PrintText("            angle swapped", false);
    }
  }
  if ((decoderType == 1) || (FLshowBootAll == true)) {
    Serial.print("\n");
    PrintText("servo 2:    angle A - " + String(servo[1].AngleA), false);
    PrintText("            angle B - " + String(servo[1].AngleB), false);
    PrintText("            speed -   " + String(servo[1].Spd), false);
    if (servo[1].Swap == false) {
      PrintText("            angle NOT swapped", false);
    } else {
      PrintText("            angle swapped", false);
    }
  }
} // end of PrintServo

/*
   WriteEEPROMServo1

   function : write servo angles to EEPROM
   called by: UpdateServo

*/
void WriteEEPROMServo1() {
  EEPROM.write(0, servo[0].AngleA);
  EEPROM.write(1, servo[0].AngleB);
  EEPROM.write(2, servo[0].Spd);
  EEPROM.write(3, servo[0].Swap);
  EEPROM.commit();
} // end of WriteEEPROMServo1

/*
   WriteEEPROMServo2

   function : write servo angles to EEPROM
   called by: UpdateServo

*/
void WriteEEPROMServo2() {
  EEPROM.write(4, servo[1].AngleA);
  EEPROM.write(5, servo[1].AngleB);
  EEPROM.write(6, servo[1].Spd);
  EEPROM.write(7, servo[1].Swap);
  EEPROM.commit();
} // end of WriteEEPROMServo2

/*
   ReadEEPROMServo1

   function : read servo angles from EEPROM
   called by: UpdateServo, LoadServo

*/
void ReadEEPROMServo1() {
  servo[0].AngleA = EEPROM.read(0);
  servo[0].AngleB = EEPROM.read(1);
  servo[0].Spd = EEPROM.read(2);
  servo[0].Swap = EEPROM.read(3);
} // end of ReadEEPROMServo1

/*
   ReadEEPROMServo2

   function : read servo angles from EEPROM
   called by: UpdateServo, LoadServo

*/
void ReadEEPROMServo2() {
  servo[1].AngleA = EEPROM.read(4);
  servo[1].AngleB = EEPROM.read(5);
  servo[1].Spd = EEPROM.read(6);
  servo[1].Swap = EEPROM.read(7);
} // end of ReadEEPROMServo2

/*
  UpdateOutput

  Function : transfer web page values to variables, store variables to EEPROM, read variables from EEPROM
             print variables to serial monitor
  called by: loop
  Calls    : WriteEEPROMOutput, ReadEEPROMOutput, PrintOutput, UpdateUI

*/
void UpdateOutput() {
  if (FLupdateOutput == true) {
    char c[4];
    FLupdateOutput = false;
    output[0].Pulse = byte((output0.selectedOption()));
    output[1].Pulse = byte((output1.selectedOption()));
    output[2].Pulse = byte((output2.selectedOption()));
    output[3].Pulse = byte((output3.selectedOption()));
    output[4].Pulse = byte((output4.selectedOption()));
    output[5].Pulse = byte((output5.selectedOption()));
    output[6].Pulse = byte((output6.selectedOption()));
    output[7].Pulse = byte((output7.selectedOption()));

    strncpy(c, pulsLength0.value(), BUFLENPULS);
    output[0].PulseDuration = atoi(c);
    strncpy(c, pulsLength1.value(), BUFLENPULS);
    output[1].PulseDuration = atoi(c);
    strncpy(c, pulsLength2.value(), BUFLENPULS);
    output[2].PulseDuration = atoi(c);
    strncpy(c, pulsLength3.value(), BUFLENPULS);
    output[3].PulseDuration = atoi(c);
    strncpy(c, pulsLength4.value(), BUFLENPULS);
    output[4].PulseDuration = atoi(c);
    strncpy(c, pulsLength5.value(), BUFLENPULS);
    output[5].PulseDuration = atoi(c);
    strncpy(c, pulsLength6.value(), BUFLENPULS);
    output[6].PulseDuration = atoi(c);
    strncpy(c, pulsLength7.value(), BUFLENPULS);
    output[7].PulseDuration = atoi(c);

    output[0].SensorOn = (outputSensor0.isChecked());
    output[1].SensorOn = (outputSensor1.isChecked());
    output[2].SensorOn = (outputSensor2.isChecked());
    output[3].SensorOn = (outputSensor3.isChecked());
    output[4].SensorOn = (outputSensor4.isChecked());
    output[5].SensorOn = (outputSensor5.isChecked());
    output[6].SensorOn = (outputSensor6.isChecked());
    output[7].SensorOn = (outputSensor7.isChecked());

    WriteEEPROMOutput();
    ReadEEPROMOutput();

    checkOutput.setChecked(false);
    UpdateUI();
    PrintOutput();
  }
} // end of UpdateOutput

/*
  LoadOutput

  function : load initial values from EEPROM into web page
  called by: setup
  calls    : ReadEEPROMOutput, UpdateUI, PrintOutput

*/
void LoadOutput() {
  ReadEEPROMOutput();

  output0.selectOption(output[0].Pulse);
  output1.selectOption(output[1].Pulse);
  output2.selectOption(output[2].Pulse);
  output3.selectOption(output[3].Pulse);
  output4.selectOption(output[4].Pulse);
  output5.selectOption(output[5].Pulse);
  output6.selectOption(output[6].Pulse);
  output7.selectOption(output[7].Pulse);

  IntToAsc(output[0].PulseDuration);
  pulsLength0.setValue(ITA);
  IntToAsc(output[1].PulseDuration);
  pulsLength1.setValue(ITA);
  IntToAsc(output[2].PulseDuration);
  pulsLength2.setValue(ITA);
  IntToAsc(output[3].PulseDuration);
  pulsLength3.setValue(ITA);
  IntToAsc(output[4].PulseDuration);
  pulsLength4.setValue(ITA);
  IntToAsc(output[5].PulseDuration);
  pulsLength5.setValue(ITA);
  IntToAsc(output[6].PulseDuration);
  pulsLength6.setValue(ITA);
  IntToAsc(output[7].PulseDuration);
  pulsLength7.setValue(ITA);

  outputSensor0.setChecked(output[0].SensorOn);
  outputSensor1.setChecked(output[1].SensorOn);
  outputSensor2.setChecked(output[2].SensorOn);
  outputSensor3.setChecked(output[3].SensorOn);
  outputSensor4.setChecked(output[4].SensorOn);
  outputSensor5.setChecked(output[5].SensorOn);
  outputSensor6.setChecked(output[6].SensorOn);
  outputSensor7.setChecked(output[7].SensorOn);

  UpdateUI();
  PrintOutput();
} // end of LoadOutput

/*
  PrintOutput

  function : print values from EEPROM to serial monitor
  called by: UpdateOutput, LoadOutput
  calls    : PrintHeader, PrintText

*/
void PrintOutput() {
  String text = "";
  if ((decoderType == 3) || (FLshowBootAll == true)) {
    PrintHeader("Switch parameter");
    PrintText("switch pulse", true);
    for (int index = 0 ; index < 8; index++) {
      text = "switch: " + String((index + 1)) + " - ";
      if ((output[index].Pulse) == 0) text = text + "No";
      if ((output[index].Pulse) == 1) text = text + "Yes";
      PrintText(text, false);
    }
    Serial.print("\n");
    PrintText("switch pulse length", true);
    for (int index = 0 ; index < 8; index++) {
      text = "switch: " + String((index + 1)) + " - " + String((output[index].PulseDuration));
      PrintText(text, false);
    }
    Serial.print("\n");
    PrintText("switch sensor message", true);
    for (int index = 0 ; index < 8; index++) {
      text = "switch: " + String((index + 1)) + " - ";
      if ((output[index].SensorOn) == 1) text = text + "No";
      if ((output[index].SensorOn) == 0) text = text + "Yes";
      PrintText(text, false);
    }
  }
} // end of PrintOutput

/*
  WriteEEPROMOutput

  function : write track table to EEPROM
  called by: UpdateOutput

*/
void WriteEEPROMOutput() {
  total = _GENERAL + _SSID + _PWD + _IP + _OUTPUT + _PULSLENGTH;
  for (int index = 0 ; index < 8; index++) {
    EEPROM.write(_GENERAL + _SSID + _PWD + _IP + index, output[index].Pulse);
  }
  for (int index = 0 ; index < 8; index++) {
    EEPROM.write(_GENERAL + _SSID + _PWD + _IP + _OUTPUT + (2 * index), (output[index].PulseDuration / 255));
    EEPROM.write(_GENERAL + _SSID + _PWD + _IP + _OUTPUT + (2 * index + 1), (output[index].PulseDuration % 255));
  }
  for (int index = 0 ; index < 8; index++) {
    EEPROM.write(total + index, output[index].SensorOn);
  }
  EEPROM.commit();
} // end of WriteEEPROMOutput

/*
   ReadEEPROMOutput

   function : read servo angles from EEPROM
   called by: UpdateOutput, LoadOutput

*/
void ReadEEPROMOutput() {
  total = _GENERAL + _SSID + _PWD + _IP + _OUTPUT + _PULSLENGTH;
  byte nr;
  byte rest;
  for (int index = 0 ; index < 8; index++) {
    output[index].Pulse = EEPROM.read(_GENERAL + _SSID + _PWD + _IP + index);
  }
  for (int index = 0 ; index < 8; index++) {
    nr = EEPROM.read(_GENERAL + _SSID + _PWD + _IP + _OUTPUT + (2 * index));
    rest = EEPROM.read(_GENERAL + _SSID + _PWD + _IP + _OUTPUT + (2 * index + 1));
    output[index].PulseDuration = nr * 255 + rest;
  }
  for (int index = 0 ; index < 8; index++) {
    output[index].SensorOn = EEPROM.read(total + index);
  }
} // end of ReadEEPROMOutput

/*
   InitialSettings

   function : store initial settings in EEPROM
   called by: setup
   calls    : PrintHeader, WriteEEPROMGeneral, WriteEEPROMServo1, WriteEEPROMServo1, WriteEEPROMServo2,
              WriteEEPROMOutput, WriteWifiReset, WriteInitial, PrintHeader

*/
void InitialSettings() {
  SPIFFS.begin();
  boolean readInitialExist = false;
  readInitialExist = ReadInitial();
  if ((FLinitial == true) || (readInitialExist == false)) {
    PrintHeader("storing INITIAL SETTINGS");
    PrintHeader("cleaning EEPROM");
    for (int index = 0; index < 512; index++) {
      EEPROM.write(index, 0);
    }
    EEPROM.commit();
    waitTimer = millis();
    Wait(2000);
    SPIFFS.begin();
    char net[10] = "TEST";
    char pw[10] = "12345678";
    sprintf(netName, "%s", net);
    sprintf(passWord, "%s", pw);
    FLwifiReset == true;
    FLwifiOK = false;
    FLdebug = true;
    FLinitial = false;
    FLswapAngleA = false;
    FLswapAngleB = false;
    servo[0].Swap = false;
    servo[1].Swap = false;
    servo[0].AngleB = 90;
    servo[0].Delay = 5;
    servo[1].AngleA = 90;
    servo[1].AngleB = 90;
    servo[1].Delay = 5;
    decoderType = 1;
    IPTriplet[0] = 192;
    IPTriplet[1] = 168;
    IPTriplet[2] = 1;
    IPTriplet[3] = 251;
    IPTriplet[4] = 254;
    IPTriplet[5] = decoderId;

    for (byte index = 0; index < 8; index++) {
      output[index].Pulse = false;
    }
    for (byte index = 0; index < 8; index++) {
      output[index].PulseDuration = 200;
    }
    WriteEEPROMGeneral();
    WriteEEPROMServo1();
    WriteEEPROMServo2();
    WriteEEPROMOutput();
    WriteWifiReset();
    WriteInitial();
    PrintHeader("INITIAL SETTINGS stored");
  }
} // end of InitialSettings

/*
   SelectConfig

   function : establish configuration of actual decoder based on decoder type
   called by: loop
   calls    : StartPosition, PrintDefault

*/
void SelectConfig() {
  FixedFieldsSr();
  memset(msgIn, 0, sizeof(msgIn));
  memset(msgOut, 0, sizeof(msgOut));
  switch (decoderType) {
    case Single:                         // single turnout decoder
      sensorNr = 5;                 // number of sensors for the PCB
      turnoutNr = 1;                // number of turnouts for the PCB

      sensor[0].Pin = D1;
      sensor[1].Pin = D2;
      sensor[2].Pin = D3;
      sensor[3].Pin = D4;
      sensor[4].Pin = D5;

      sensor[0].Address = 1;
      sensor[1].Address = 2;
      sensor[2].Address = 3;
      sensor[3].Address = 4;
      sensor[4].Address = 5;

      servo[0].Pin = D6;
      servo[0].AngleA = 100;

      relais[0].Pin = D0;

      pinMode(relais[0].Pin, OUTPUT);
      digitalWrite(relais[0].Pin , LOW);

      if ((servo[0].AngleA) > (servo[0].AngleB)) servo[0].Invert = true;
      relais[0].SwitchPoint = ((servo[0].AngleA + servo[0].AngleB) / 2);
      servoMotor[0].attach (servo[0].Pin);

      for (byte index = 0; index < sensorNr; index++) {        // initialising sensor pins
        pinMode(sensor[index].Pin, INPUT_PULLUP);
      }
      if (FLpolling == false) {
        attachInterrupt(digitalPinToInterrupt(sensor[0].Pin), ISR0, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[1].Pin), ISR1, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[2].Pin), ISR2, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[3].Pin), ISR3, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[4].Pin), ISR4, FALLING);
      }

      toBuffer.orderNew = true;
      toOrder.executed = true;             // do NOT remove!!!!!!!

      StartPosition();
      break;

    case Double:                                // double turnout decoder
      sensorNr = 2;                        // number of sensors for the PCB
      turnoutNr = 2;                       // number of turnouts for the PCB

      sensor[0].Pin = D1;
      sensor[1].Pin = D2;
      sensor[0].Address = 1;
      sensor[1].Address = 2;

      servo[0].Pin = D6;
      servo[1].Pin = D7;

      relais[0].Pin = D0;
      relais[1].Pin = D5;

      for (byte index = 0; index < turnoutNr; index++) {                     // initialising output pins
        pinMode(relais[index].Pin, OUTPUT);
        digitalWrite(relais[index].Pin , LOW);
      }

      for (byte index = 0; index < sensorNr; index++) {          // initialising sensor pins
        pinMode(sensor[index].Pin, INPUT_PULLUP);
      }
      if (FLpolling == false) {
        attachInterrupt(digitalPinToInterrupt(sensor[0].Pin), ISR0, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[1].Pin), ISR1, FALLING);
      }

      for (byte index = 0; index < turnoutNr ; index++) {        // initialising relais output points
        if ((servo[index].AngleA) > (servo[index].AngleB)) servo[index].Invert = true;
        relais[index].SwitchPoint = ((servo[index].AngleA + servo[index].AngleB) / 2);
      }

      servoMotor[0].attach (servo[0].Pin);
      servoMotor[1].attach (servo[1].Pin);

      toBuffer.orderNew = true;
      toOrder.executed = true;         // do NOT remove!!!!!!!

      StartPosition();
      break;

    case Sensor:                           // sensor decoder
      sensorNr = 8;                   // number of sensors

      sensor[0].Pin = D0;
      sensor[1].Pin = D1;
      sensor[2].Pin = D2;
      sensor[3].Pin = D3;
      sensor[4].Pin = D4;
      sensor[5].Pin = D5;
      sensor[6].Pin = D6;
      sensor[7].Pin = D7;

      sensor[0].Address = 0;
      sensor[1].Address = 1;
      sensor[2].Address = 2;
      sensor[3].Address = 3;
      sensor[4].Address = 4;
      sensor[5].Address = 5;
      sensor[6].Address = 6;
      sensor[7].Address = 7;

      for (byte index = 0; index < sensorNr; index++) {                   // initialising sensor pins
        pinMode(sensor[index].Pin, INPUT_PULLUP);
      }

      if (FLpolling == false) {
        attachInterrupt(digitalPinToInterrupt(sensor[1].Pin), ISR1, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[2].Pin), ISR2, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[3].Pin), ISR3, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[4].Pin), ISR4, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[5].Pin), ISR5, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[6].Pin), ISR6, FALLING);
        attachInterrupt(digitalPinToInterrupt(sensor[7].Pin), ISR7, FALLING);
      }
      break;

    case Switch:                             // output decoder
      outputNr = 8;                     // number of outputes

      output[0].Pin = D0;               // output pins
      output[1].Pin = D1;
      output[2].Pin = D2;
      output[3].Pin = D3;
      output[4].Pin = D4;
      output[5].Pin = D5;
      output[6].Pin = D6;
      output[7].Pin = D7;

      for (byte index = 0; index < outputNr; index++) {                     // initialising output pins
        pinMode(output[index].Pin, OUTPUT);
        digitalWrite(output[index].Pin , LOW);
      }

      break;
    default:
      FLonce = false;
      PrintDefault("Should not happen - SelectConfig");
      break;
  }
} //end of SelectConfig

/*
   StartAP

   function : setup WiFi Access Point
   called by: setup()

*/
void StartAP() {
  char myPW[8] = "eltraco";
  static char APPassWord[20];
  char myName[17] = "config-";
  char st[4] = "ST-";
  char dt[4] = "DT-";
  char sw[4] = "SW-";
  char sr[4] = "SR-";
  char ck[4] = "CK-";
  switch (decoderType) {
    case Single:
      sprintf(APName, "%s%s%i", myName, st, decoderId);
      break;
    case Double:
      sprintf(APName, "%s%s%i", myName, dt, decoderId);
      break;
    case Sensor:
      sprintf(APName, "%s%s%i", myName, sr, decoderId);
      break;
    case Switch:
      sprintf(APName, "%s%s%i", myName, sw, decoderId);
      break;
  }
  IPAddress gateway(0, 0, 0, 0);
  IPAddress decoder(192, 168, 4, decoderId);
  IPAddress subnet(255, 255, 255, 0);
  sprintf(APPassWord, "%s%i", myPW, decoderId);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig (decoder, gateway, subnet);
  WiFi.softAP(APName, APPassWord, 11);
  PrintHeader("WiFi AP mode started");
  PrintText("SSID:       " + String(APName), false);
  PrintText("password:   " + String(APPassWord), false);
  Serial.print("          IP address: ");
  Serial.print(WiFi.softAPIP());
  Serial.print("\n\n");
} // end of StartAP

/*
  StartWifi

  function : connect to network, install static IP address

  calls    : WriteWifiReset

*/
void StartWifi() {
  byte count = 0;
  ReadWifiOk();
  IPAddress gateway(IPTriplet[0], IPTriplet[1], IPTriplet[2], IPTriplet[3]);
  IPAddress decoder(IPTriplet[0], IPTriplet[1], IPTriplet[2], IPTriplet[5]);
  IPAddress subnet(255, 255, 255, 0);
  waitTimer = millis();
  Wait(10);
  PrintHeader("Connecting to SSID " + String(netName));
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(netName, passWord);
  WiFi.config(decoder, gateway, subnet);

  Serial.print("          ");
  while (WiFi.status() != WL_CONNECTED) {
    FLwifiEstablished = false;
    waitTimer = millis();
    Wait(500);
    count++;
    Serial.print("* ");
    if (count == 30 && FLwifiOK == false) {
      PrintWarning("NO WIFI connection. Change parameters via configuration");
      FLwifiReset = true;
      WriteWifiReset();
      waitTimer = millis();
      Wait(2000);
      ESP.restart();
    }
    if (count == 30 && FLwifiOK == true) {
      PrintWarning("NO WIFI connection. Switch router ON");
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n");
    PrintHeader("WiFi connected");
    PrintText("decoder:    " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
              String(IPTriplet[2]) + "." + String(IPTriplet[5]) + "\n", false);
    FLwifiEstablished = true;
  }
} // end of StartWifi

/*
  StartMQTT

  function : start Mosquitto, set callback function
  called by: setup
  calls    : Callback, PrintHeader, PrintText

*/
void StartMQTT() {
  IPAddress mosquitto(IPTriplet[0], IPTriplet[1], IPTriplet[2], IPTriplet[4]);
  client.set_server(mosquitto, 1883);
  static const char *MQTTclientId = ClientName;
  client.set_callback(Callback);                 // set up callback for Mosquitto
  PrintHeader("Mosquitto client started");
  PrintText(String(ClientName) + " connecting to broker at " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
            String(IPTriplet[2]) + "." + String(IPTriplet[4]), false);
  Serial.print("\n");
} // end of StartMQTT

/*
  Reconnect

  function : re-establish connection with MQTT clientID. subscribe to topic.
             when Mosquitto not available try again. after 10 attemts ==> reboot
  called by: loop
  calls    : PrintHeader, PrintText, PrintWarning, WriteWifiReset

*/
void Reconnect() {
  Serial.print("          ");
  int retries = 0;
  FLonce = false;
  while (!client.connected()) {
    FLmqttEstablished = false;
    if (client.connect(ClientName)) {
      client.subscribe(topicSub1);
      client.subscribe(topicSub2);
      client.subscribe(topicSub3);
      client.subscribe(topicSub4);
      PrintHeader("Subscribed to topics");
      PrintText(String(topicSub1), false);
      PrintText(String(topicSub2), false);
      PrintText(String(topicSub3), false);
      PrintText(String(topicSub4), false);
    } else {
      if (FLonce == false) {
        PrintHeader("connecting to mosquitto");
        FLonce = true;
      }
      retries++;
      Serial.print("* ");
      if (retries % 30 == 0) {
        Serial.print("\n");
      }
      waitTimer = millis();
      Wait(500);
      if (retries % 30 == 0) {
        PrintWarning("Connection with Mosquitto failed");
        waitTimer = millis();
        Wait(1000);
        ESP.restart();
        break;
      }
    }
  }
  if (client.connected()) FLmqttEstablished = true;
}
// end of reconnect()

/*
  StartFtp

  function : start FTP server
  called by: setup
  calls    : PrintHeader, PrintText

*/
void StartFtp() {
  char myName[10] = "eltraco";
  char myPW[10] = "eltraco";
  static char FTPName[15];
  static char FTPPassWord[15];
  sprintf(FTPName, "%s%i", myName, decoderId);
  sprintf(FTPPassWord, "%s%i", myPW, decoderId);
  ftpSrv.begin(FTPName, FTPPassWord);   //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
  PrintHeader("FTP server started");
  PrintText("login on AP network to use FTP", true);
  PrintText("IPaddress:  " + String(IPTriplet[0]) + "." + String(IPTriplet[1]) + "." + \
            "4" + "." + String(IPTriplet[5]), false);
  PrintText("username:   " + String(FTPName), false);
  PrintText("password:   " + String(FTPPassWord), false);
  PrintText("portnumber: 21", false);
} // end of StartFtp

/*
   CreateName

   function : create decoder name
   called by: StartMQTT

*/
void CreateName() {
  char myName[17] = "ELTRACO-";
  char st[4] = "ST-";
  char dt[4] = "DT-";
  char sw[4] = "SW-";
  char sr[4] = "SR-";
  char ck[4] = "CK-";
  switch (decoderType) {
    case Single:
      sprintf(ClientName, "%s%s%i", myName, st, decoderId);
      break;
    case Double:
      sprintf(ClientName, "%s%s%i", myName, dt, decoderId);
      break;
    case Sensor:
      sprintf(ClientName, "%s%s%i", myName, sr, decoderId);
      break;
    case Switch:
      sprintf(ClientName, "%s%s%i", myName, sw, decoderId);
      break;
  }
  PrintHeader("clientname: " + String(ClientName));
} // end of CreateName

/*
   StartOTA

   function : start Over The Air function
   called by: setup

*/
void StartOTA() {
  ArduinoOTA.setPort(8266);                         // Port defaults to 8266
  ArduinoOTA.setHostname(ClientName);               // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setPassword((const char *)"123");    // No authentication by default
  ArduinoOTA.onStart([]() {
    PrintHeader("OTA started");
  });
  ArduinoOTA.onEnd([]() {
    Serial.print("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.print("\nAuth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.print("\nBegin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.print("\nConnect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.print("\nReceive Failed");
    else if (error == OTA_END_ERROR) Serial.print("\nEnd Failed");
  });
  ArduinoOTA.begin();
} // end of StartOTA

/*

   StartupScan

   function : report initial sensor status
   called by: setup

*/
void StartupScan() {
  if (decoderType != Switch) {
    PrintHeader("Initial sensor scan");
    waitTimer = millis();
    Wait(2000);
    for (byte index = 0; index < sensorNr; index++) {
      sensor[index].State = !digitalRead(sensor[index].Pin);
      SM(index + 1, (sensor[index].State));
      sensor[0].Itr = true;
      sensor[0].ItrTime = millis();
      waitTimer = millis();
      Wait(100);
    }
  }
} // end of StartupScan

/*
  SoftwareVersion

  function : display decoder type and sofware version on serial monitor
  called by: setup
  calls    : PrintText

*/
void SoftwareVersion() {
  static String text = "";
  switch (decoderType) {
    case Single:
      text = "single turnout";
      break;
    case Double:
      text = "double turnout";
      break;
    case Sensor:
      text = "sensor";
      break;
    case Switch:
      text = "switch";
      break;
  }
  Serial.print("\n\n == == == == == == == == == == == == == == == == == == == == == == == ==\n");
  Serial.print("\n          EEEEE  LL   TTTTTT  RRR        A        CCC     OO");
  Serial.print("\n          EE     LL     TT    RR RR    AA AA     CC     OO  OO");
  Serial.print("\n          EEEE   LL     TT    RRR     AAAAAAA   CC     OO    OO");
  Serial.print("\n          EE     LL     TT    RR RR   AA   AA    CC     OO  OO");
  Serial.print("\n          EEEEE  LLLLL  TT    RR  RR  AA   AA     CCC     OO");
  Serial.print("\n\n == == == == == == == == == == == == == == == == == == == == == == == ==\n");
  Serial.print("\n\n");
  PrintText("Decoder: " + text, true);
  PrintText("Software date: " + version, true);
  PrintText("INITIAL configuration via 192.168.4.1", true);
  PrintText("following settings via 192.168.4." + String(IPTriplet[5]), true);
  PrintText("FTP: login to " + String(APName) + " and use 192.168.4." + String(IPTriplet[5]) + " port 21", true);
} // end of SoftwareVersion()

/*
   PageHandler

   function : controls the pages to be displayed on the configuration website
   called by: setup()

*/
void PageHandler() {
  server.on("/", handlePage1);
  server.on("/page1", handlePage1);
  server.on("/page2", handlePage2);
  server.on("/page3", handlePage3);
  server.on("/page4", handlePage4);
  server.begin();
} // end of PageHandler

/*
   StartEEPROM

   function : prepare EEPROM for first use
   called by: setup()

*/
void StartEEPROM() {
  EEPROM.begin(512);
  if ((EEPROM.read(512) == 0xFF) && (EEPROM.read(513) == 0xFF)) {                    //eeprom empty, first run
    PrintHeader("EEPROM EMPTY....Setting Default values");
    for (int index = 0; index < 512; index++) {
      EEPROM.write(index, 0);
    }
    EEPROM.commit();
    waitTimer = millis();
    Wait(2000);
  }
} // end of StartEEPROM

/*
  Callback

  function : receive incoming message, test topic, test on recepient, display message, store data
  called by: StartMQTT
  calls    : PrintMsgIn,

*/
void Callback(const MQTT::Publish & pub) {
  if (startUpReady == true) {

    if ((pub.topic() == "rocnet/sr")  || (pub.topic() == "rocnet/ot")) {
      msgIn[2] = ((byte)pub.payload()[2]);             // receiveId L
      msgIn[4] = ((byte)pub.payload()[4]);             // sendIdL
      msgIn[7] = ((byte)pub.payload()[7]);             // number following data bytes
      msgIn[8] = ((byte)pub.payload()[8]);             // data1
      msgIn[9] = ((byte)pub.payload()[9]);             // data2
      msgIn[10] = ((byte)pub.payload()[10]);           // data3
      msgIn[11] = ((byte)pub.payload()[11]);           // data4
      FLprint = false;  // xxxxxxx
      if (msgIn[2] == decoderId || msgIn[4] == 41 || msgIn[4] == 42 || msgIn[4] == 43 || msgIn[4] == 44) {
        if ((msgIn[4] == decoderId) && (FLshowOwnMessage == true)) FLprint = true;
        if (msgIn[2] == decoderId) FLprint = true;
        if (msgIn[2] == 1) FLprint = true;
        if (FLprint == true && FLdebug == true) {
          topicIn = pub.topic();
          msgLengthIn = 12;
          if (pub.topic() == "rocnet/sr") {
            PrintMsgInContent("SM");
          }
          if (pub.topic() == "rocnet/ot") {
            if (decoderType != 2)  PrintMsgInContent("TOM");
          }
        }
      }
    }

    if ((pub.topic()) == ("rocnet/hb")) {
      msgIn[0] = ((byte)pub.payload()[0]);             // sendId
      msgIn[1] = ((byte)pub.payload()[1]);             // "0" for HB, "1" for all, "decoderId" for single
      msgIn[2] = ((byte)pub.payload()[2]);             // HBM
      msgIn[3] = ((byte)pub.payload()[3]);             // length
      if ((msgIn[0] == decoderId) || (msgIn[1] == 1) || (msgIn[1] == decoderId)) {
        if (FLdebug == true && FLshowHB == true) {
          msgLengthIn = 4;
          topicIn = pub.topic();
          PrintMsgInContent("HBM");
        }
      }
    }

    if ((pub.topic()) == ("rocnet/ck")) {
      msgIn[12] = ((byte)pub.payload()[12]);           // hour
      msgIn[13] = ((byte)pub.payload()[13]);           // minute
      msgIn[14] = ((byte)pub.payload()[14]);           // second
      msgIn[15] = ((byte)pub.payload()[15]);           // clock divider
      if (msgIn[15] == 0) msgIn[15] = 1;
      topicIn = pub.topic();
      msgLengthIn = 16;
      if (FLshowClockMessage == true) PrintMsgInContent("CKM");
    }


    if (topicIn == "rocnet/ot")  {
      if ( msgIn[4] == 1) {                                        // sender is server
        if ((decoderType == Double || decoderType == Single) && msgIn[2] == decoderId) {        // single or double turnout
          TOMReaction();                                           // reaction on Turnout Order Message
        }
      }

      if (msgIn[4] == 1) {                                        // sender is server
        if (decoderType == Switch && msgIn[2] == decoderId) {     // switch
          SOMReaction();                                          // reaction on Switch Order Message
        }
      }

    }
    if (topicIn == "rocnet/sr" && msgIn[4] != 1 && ((msgIn[2] == decoderId) || (msgIn[4] == 41) || (msgIn[4] == 42) || (msgIn[4] == 43))) {
      if (decoderType == Switch) {                                // switch
        SMReaction();                                             // reaction on Sensor Message
      }
      if (decoderType == Single ) {                              // switch
        SMTOReaction();                                             // reaction on Sensor Message
      }
    }

    if ((pub.topic()) == ("rocnet/ck")) {
      hours = msgIn[12];
      minutes = msgIn[13];
      seconds = msgIn[14];
      keepAliveCounter = 0;
      CLOCK_SPEED = 1000 / msgIn[15];
    }

  }
} // end of Callback

void SMTOReaction() {
  byte port = 0;
  byte pos = 0;
  boolean sw = false;
  boolean to = false;
  String outputOrder = "";
  switch (msgIn[4]) {
    case 43:

      if ((msgIn[11] == 3) && (msgIn[10] = 1)) {
        msgIn[8] = 0;
      }
      if ((msgIn[11] == 6) && (msgIn[10] = 1)) {
        msgIn[8] = 1;
      }
      msgIn[11] = 1;
      TOMReaction();
      break;
  }
}

void SMReaction() {
  byte port = 0;
  byte pos = 0;
  boolean sw = false;
  boolean to = false;
  String outputOrder = "";
  switch (msgIn[4]) {
    case 1:
      /*
        port = msgIn[11];
        output[port].Order = msgIn[10];
        output[port].Executed = false;
      */
      break;

    case 41:
      if (decoderId == 44) {
        switch (msgIn[11]) {
          case 1:
            break;
          case 3:
            port = 3;
            sw = true;
            break;
          case 2:
            port = 4;
            sw = true;
            break;
          default:
            FLonce = false;
            PrintDefault("Should not happen - SMReaction - 41-" + String(msgIn[11]));
            break;
        }
      }
      break;

    case 42:
      if (decoderId == 44) {
        switch (msgIn[11]) {
          case 0:
            port = 5;
            sw = true;
            break;
          default:
            FLonce = false;
            PrintDefault("Should not happen - SMReaction -42 -" + String(msgIn[11]));
            break;
        }
      }
      break;

    case 43:
      if (decoderId == 44) {
        switch (msgIn[11]) {
          case 4:
            port = 6;
            output[port].Order = msgIn[10];
            output[port].Executed = false;
            port = 7;
            sw = true;
            break;
          case 2:
            port = 2;
            sw = true;
            break;
          case 3:
            port = 0;
            sw = true;
            break;
          case 6:
            port = 1;
            sw = true;
            break;
          default:
            FLonce = false;
            PrintDefault("Should not happen - SMReaction - 43 -" + String(msgIn[11]));
            break;
        }
      }
      if (decoderId == 41) {
        switch (msgIn[11]) {
          case 2:
            break;
          case 3:
            break;
          case 4:
            break;
          case 6:
            port = 1;
            to = true;
            break;
          default:
            FLonce = false;
            PrintDefault("Should not happen - SMReaction - 41 sub -" + String(msgIn[11]));
            break;
        }
      }
      break;

    default:
      FLonce = false;
      PrintDefault("Should not happen - SMReaction");
      break;
  }

  if (sw == true) {
    sw = false;
    output[port].Order = msgIn[10];
    output[port].Executed = false;
  }
  if (to == true) {
    to = false;
    toBuffer.targetId = port;
    toBuffer.orderOld = toBuffer.order;
    toBuffer.order = msgIn[10];
    toBuffer.orderNew = true;
    if (FLfirstOrder == true) {
      toOrder.executed = true;
      FLfirstOrder = false;
    }
    if (msgIn[10] == 1) {
      outputOrder = "thrown";
    } else {
      outputOrder = "straight";
    }
    PrintMsgInCat("direct order received - move turnout to " + String(outputOrder));
  }

} // end of SMReaction

void SOMReaction() {
  byte port = 0;
  String outputOrder = "";
  port = msgIn[11] - 1;
  output[port].OrderOld = output[port].Order;
  output[port].Order = msgIn[10];
  if (output[port].OrderOld != output[port].Order) {
    output[port].Executed = false;
    if (msgIn[10] == 1) {
      outputOrder = "ON";
    } else {
      outputOrder = "OFF";
    }
    PrintMsgInCat("SOM received - switch port " + String(port) + " to " + outputOrder);
  }
} // end of SOMReaction

void TOMReaction() {
  String outputOrder = "";
  toBuffer.targetId = msgIn[11];
  toBuffer.orderOld = toBuffer.order;
  toBuffer.order = msgIn[8];
  toBuffer.orderNew = true;
  if (FLfirstOrder == true) {
    toOrder.executed = true;
    FLfirstOrder = false;
  }
  if (msgIn[8] == 1) {
    outputOrder = "thrown";
  } else {
    outputOrder = "straight";
  }
  PrintMsgInCat("TOM received - move turnout " + String(msgIn[11]) + " to " + outputOrder);
} // end of TOMReaction

void PrintMsgInContent(String text) {
  Serial.print("\n");
  Serial.print("Msg received  [");
  Serial.print(topicPub);
  Serial.print(" - DEC, dotted] ");
  Serial.print(text + " <== ");
  for (int index = 0 ; index < msgLengthIn ; index++) {
    Serial.print((msgIn[index]), DEC);
    if (index < msgLengthIn - 1) Serial.print(F("."));
  }
  Serial.print("\n");
} // end of PrintRMsg

/*
  PrintMsgInCat

  function : display message received
  called by: Callback
  calls    : PrintHeader

*/
void PrintMsgInCat(String text) {
  if (FLprint == true) {
    PrintHeader(text);
  }
} // end of PrintMsgInCat

/*
  ReadInitial

  function : read "FLinitial" from SPIFFS file "initial.txt", load initial values
  called by: UpdateGeneral

*/
boolean ReadInitial() {
  File f = SPIFFS.open("/initial.txt", "r");
  byte cntr = 0;
  if (!f) {
    PrintHeader("File doesn't exist.");
    return (false);
    // open the file in write mode
    File f = SPIFFS.open("/initial.txt", "w");
    if (!f) {
      PrintHeader("NOT able to create file.");
    } else {
      f.println("1");
      PrintHeader("File created.");
    }
  } else {
    String in = f.readStringUntil('n');
    FLinitial = (in.toInt());
    if (FLinitial == false) {
      PrintHeader("Initial values available");
    } else PrintHeader("Generation initial values required");
    return (true);
  }
  f.close();
} // end of ReadInitial

/*
  WriteInitial()

  function : write "FLinitial" to file "initial.txt" in SPIFFS
  called by: UpdateGeneral, InitialSettings

*/
void WriteInitial() {
  File f = SPIFFS.open("/initial.txt", "w+");
  f.println(FLinitial);
  f.close();
} // end of WriteInitial

/*
  WriteInitial()

  function : write "FLinitial" to file "initial.txt" in SPIFFS
  called by: UpdateGeneral, InitialSettings

*/
void WriteWifiOk() {
  File f = SPIFFS.open("/wifiok.txt", "w+");
  f.println(FLwifiOK);
  f.close();
} // end of WriteWifiOk

/*
  ReadWifiOk

  function : read "FLinitial" from SPIFFS file "initial.txt", load initial values
  called by: UpdateGeneral

*/
boolean ReadWifiOk() {
  File f = SPIFFS.open("/wifiok.txt", "r");
  byte cntr = 0;
  if (!f) {
    PrintHeader("writing file");
    return (false);
    // open the file in write mode
    File f = SPIFFS.open("/wifiok.txt", "w");
    if (!f) {
      PrintHeader("NOT able to create file.");
    } else {
      f.println("0");
      PrintHeader("file created.");
    }
  } else {
    String in = f.readStringUntil('n');
    FLwifiOK = (in.toInt());
    if (FLwifiOK == false) PrintHeader("wifiOK:     NO");
    if (FLwifiOK == true) PrintHeader("wifiOK:     YES");
    return (true);
  }
  f.close();
} // end of ReadWifiOk

/*
  StartPosition

  function : output relais on, move servo to middle position and back into straight position. output relais off
  called by: SelectConfig

*/
void StartPosition() {
  PrintHeader("Initial servo move");
  for (byte index = 0; index < turnoutNr ; index++) {
    digitalWrite(relais[index].Pin, HIGH);
    servoMotor[index].write((servo[index].AngleA + servo[index].AngleB) / 2);
    waitTimer = millis();
    Wait(500);
    servoMotor[index].write(servo[index].AngleA);
    digitalWrite(relais[index].Pin, LOW);
  }
} // end of StartPosition

/*
  PrintDefault

  function : print formatted text and variable to serial monitor
  called by: output statements

*/
void PrintDefault(String text) {
  if (FLonce == false) {
    FLonce = true;
    Serial.print("\n -------- ");
    Serial.print(text);
    Serial.print(" ----------\n");
  }
} // end of PrintDefault
/*
  PrintHeader

  function : print formatted header to serial monitor

*/
void PrintHeader(String text) {
  Serial.print("\n -------- ");
  Serial.print(text);
  Serial.print(" -------- \n\n");
} // end of PrintHeader

/*
  PrintText

  function : print formatted text to serial monitor

*/
void PrintText(String text, boolean b) {
  Serial.print("          ");
  Serial.print(text);
  Serial.print(" \n");
  if (b == true) Serial.print(" \n");
} // end of PrintText

/*
  PrintWarning

  function : print formatted text to serial monitor

*/
void PrintWarning(String text) {
  Serial.print("\n\n\n");
  for (byte index = 0; index < text.length() + 16 ; index++) {
    Serial.print("X");
  }
  Serial.print("\nX X X   ");
  Serial.print(text);
  Serial.print("   X X X\n");
  for (byte index = 0; index < text.length() + 16; index++) {
    Serial.print("X");
  }
  Serial.print("\n\n\n");
} // end of PrintWarning

/*
  ReadWifiReset

  function : read "resetWifi" from SPIFFS file "wifi.txt"
  called by: UpdateGeneral, LoadGeneral
  calls    : PrintHeader

*/
void ReadWifiReset() {
  File f = SPIFFS.open("/wifi.txt", "r");
  byte cntr = 0;
  if (!f) {
    PrintHeader("File doesn't exist.");
    // open the file in write mode
    File f = SPIFFS.open("/wifi.txt", "w");
    if (!f) {
      PrintHeader("NOT able to create file.");
    } else {
      f.println("1");
      PrintHeader("File created.");
    }
  } else {
    String in = f.readStringUntil('n');
    FLwifiReset = (in.toInt());
    if (FLwifiReset == 0)  {
      PrintHeader("Wifi configuration OK");
    } else PrintHeader("WiFi needs to be configured");
  }
  f.close();
} // end of ReadWifiReset

/*
  WriteWifiReset()

  function : write "resetWifi" to file "wifi.txt" in SPIFFS
  called by: UpdateGeneral, InitialSettings, StartWifi, Reconnect

*/
void WriteWifiReset() {
  File f = SPIFFS.open("/wifi.txt", "w+");
  f.println(FLwifiReset);
  f.close();
} // end of WriteWifiReset

/*
  StartSPIFFS

  function : Start the SPIFFS and list all contents
  called by: setup
  calls    : PrintHeader

*/
void StartSPIFFS() {
  SPIFFS.begin();
  PrintHeader("SPIFFS started");
} // end of StartSPIFFS

/*
  Wait

  function : generate wait period without delay()
  called by: InitialSettings

*/
void Wait(int period) {
  do {
    if (((millis() - waitTimer) % 100) == 0) {
      yield();
    }
  } while (millis() < waitTimer + period);
} // end of WaitPeriode

/*
  ByteToAsc

  function : convert byte value into ASCII value
  called by: several

*/
void ByteToAsc(int nr) {
  memset(BTA, 0, sizeof(BTA));
  byte a = 0;
  byte b = 0;
  byte c = 0;
  if (nr > 255) {
    PrintText("number > 255", false);
  } else {
    a = (nr) / 100;
    b = (nr % 100) / 10;
    c = (nr % 10);
    BTA[0] = a + 48;
    BTA[1] = b + 48;
    BTA[2] = c + 48;
    BTA[3] = 0;
  }
} // end of ByteToAsc

/*
  IntToAsc

  function : convert int value into ASCII value
  called by: several

*/
void IntToAsc(int nr) {
  memset(ITA, 0, sizeof(ITA));
  byte a = 0;
  byte b = 0;
  byte c = 0;
  byte d = 0;
  byte e = 0;
  byte f = 0;
  byte g = 0;
  byte h = 0;

  if (nr > 99999999) {
    PrintText("number to large", false);
  } else {

    a = (nr % 10);
    b = (nr % 100) / 10;
    c = (nr % 1000) / 100;
    d = (nr % 10000) / 1000;
    e = (nr % 100000) / 10000;
    f = (nr % 1000000) / 100000;
    g = (nr % 10000000) / 1000000;
    h = (nr % 100000000) / 10000000;

    ITA[8] = 0; ITA[7] = a + 48; ITA[6] = b + 48; ITA[5] = c + 48; ITA[4] = d + 48; ITA[3] = e + 48; ITA[2] = f + 48; ITA[1] = g + 48; ITA[0] = h + 48;

    if (h == 0) {
      ITA[7] = 0; ITA[6] = a + 48; ITA[5] = b + 48; ITA[4] = c + 48; ITA[3] = d + 48; ITA[2] = e + 48; ITA[1] = f + 48; ITA[0] = g + 48;
    }
    if ((h == 0) && (g == 0)) {
      ITA[6] = 0; ITA[5] = a + 48; ITA[4] = b + 48; ITA[3] = c + 48; ITA[2] = d + 48; ITA[1] = e + 48; ITA[0] = f + 48;
    }
    if ((h == 0) && (g == 0) && (f == 0)) {
      ITA[5] = 0; ITA[4] = a + 48; ITA[3] = b + 48; ITA[2] = c + 48; ITA[1] = d + 48; ITA[0] = e + 48;
    }
    if ((h == 0) && (g == 0) && (f == 0) && (e == 0)) {
      ITA[4] = 0; ITA[3] = a + 48; ITA[2] = b + 48; ITA[1] = c + 48; ITA[0] = d + 48;
    }
    if ((h == 0) && (g == 0) && (f == 0) && (e == 0) && (d == 0)) {
      ITA[3] = 0; ITA[2] = a + 48; ITA[1] = b + 48; ITA[0] = c + 48;
    }
    if ((h == 0) && (g == 0) && (f == 0) && (e == 0) && (d == 0) && (c == 0)) {
      ITA[2] = 0; ITA[1] = a + 48; ITA[0] = b + 48;
    }
    if ((h == 0) && (g == 0) && (f == 0) && (e == 0) && (d == 0) && (c == 0) && (b == 0)) {
      ITA[1] = 0; ITA[0] = a + 48;
    }

  }
} // end of IntToAsc



