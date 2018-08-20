/*

   Aim of this software is to demonstrate the use of the EmbAJAX library.

   This library provides the ability to create interactive webpages using an ESP8266 module as server.
   Data exchange beteen server and client is done in the background.

   The library is created by Thomas Friedrichsmeier.

   This software is made to provide a GUI for my model railroad automatic shunting project.

   The way to use of the web elements can be deducted from the examples provided by Thomas.

   His examples however do not provide any guidance on how to model the presentation of data.
   Modelling the presentation of the data in the GUI is done with CSS commands.
   However not all CSS commands available can be used with the library.

   Being a more complex application of the library it took quite some time to
   discover the posibilities of the library and the usage of CSS type of commands.

   This software should run stand alone without any further integration with ELTRACO decoder software.

   The software presents three pages.

   Page 1 is foreseen to be used most often. Its purpose is to determine which railroad car has to
   go to which railroad track. This selection is done by means of the "radiogroup" element of the
   library.

   Page 2 is used to insert the identification of the wagons with the RFID tag code associated with each wagon.
   Mainly "textinput" elements are used.
   The RFID reader software does not provide the separate bytes of the RFID code of the tag but a sum contained
   in an INT value. The library exchanges CHAR values between server and client. Therefore in the software there
   are two software modules, one to convert the sum of the RFID code into a number of CHAR elements and another
   one for the conversion the other way around.

   The identification data of the wagons is used on the first page to select a destination track for each of them.
   The identification data can be inserted and changed on page 2 and are presented on page 1.

   Page 3 provides the possibility to adjust the two angles of the servo which is the hart of the decoupling device.
   The "slider" element is used to adjust the angles.
   For my shunting project the ELTRACO single turnout PCB is used to drive the servo for the decoupler.

   On the left side at the bottom of each page the "Link Status" is displayed. This is done with the
   "connectionindicator" element. Being "OK" indicates a correct behaviour of the data exchange. Sometimes
   it takes some moments to change from "FAIL" into "OK". Thomas addresses this in his documentation of the library.

   On the right side at the bottom two "links" are provided to switch to one of the other pages.

   Furthermore "checkbutton" elements are used to evoke actions in the software to process data.

   The data inserted in the GUI are stored into EEPROM.
   During the boot phase these data are read from EEPROM and displayed in the GUI.


   This software is provided as is in the hope to save other people time to figure out how to use the very attractive
   EmbAJAX library.


   Ellard Postma

*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EmbAJAX.h>
#include <FS.h>
#include <EEPROM.h>
#include <Servo.h>

#define DEBUG

ESP8266WebServer server(80);
EmbAJAXOutputDriverESP8266 driver(&server);
Servo servo[1];
#define BUFLENTAG 6
#define BUFLENCAR 23
#define BUFLENSLIDERUP 5
#define BUFLENSLIDERDOWN 5

//////////////////////////////////////////// decoupler ////////////////////
static const int tagAmount = 10;
static byte servoPin[2];                      // servo pin number
static byte servoPos[2][2] = {                // initial value of two pair of servo angles
  {80, 100},
  {70, 110}
};
static byte servoId = 0;                      // id servo
static byte servoAngle = 0;                   // angle servo
static byte servoAngleOld = 0;
static boolean inverted[2];                   // if angleservoPos 0 > servoPos 1 than inverted = true
static byte relais[2];                        // relais pin number
static boolean ackStraight = false;           // ack straight for servo adjust
static boolean ackThrown = false;             // ack thrown for servo adjust
static byte turnoutId = 0;                    // id turnout
static byte turnoutNr;
static unsigned int relaisSwitchPoint[8];     // relais switch start time

////////////////////////////////////////// shunting GUI /////////////////////////////////////////

static const char* destination_opts[13] = {"...Stop", "...Tr-1", "...Tr-2", "...Tr-3", "...Tr-4", \
                                           "...Tr-5", "...Tr-6", "...Tr-7", "...Tr-8", "...Tr-9", \
                                           "...Tr-10", "...Tr-11"
                                          };

static boolean updateCarTagFlag = false;
static boolean updateTrackFlag = false;
static boolean startupFlag = false;
static boolean adjustServoFlag = false;

static const byte tagLen = BUFLENTAG - 1;
static const int carAmount = 10;
static unsigned char printBuffer[30];
static byte selected = 13;

static char car[carAmount][BUFLENCAR - 1];
static byte tag[tagAmount][2];
static byte track[11];
static char tagChar[tagLen];

static int trackAmount = 0;
static int RFIDsum = 0;
static const byte EEPROMOffset = 20;

//construction of the tracks to be selected

EmbAJAXRadioGroup<12> destination0("destination0", destination_opts, selected);
EmbAJAXMutableSpan destination0_d("destination0_d");
EmbAJAXRadioGroup<12> destination1("destination1", destination_opts, selected);
EmbAJAXMutableSpan destination1_d("destination1_d");
EmbAJAXRadioGroup<12> destination2("destination2", destination_opts, selected);
EmbAJAXMutableSpan destination2_d("destination2_d");
EmbAJAXRadioGroup<12> destination3("destination3", destination_opts, selected);
EmbAJAXMutableSpan destination3_d("destination3_d");
EmbAJAXRadioGroup<12> destination4("destination4", destination_opts, selected);
EmbAJAXMutableSpan destination4_d("destination4_d");
EmbAJAXRadioGroup<12> destination5("destination5", destination_opts, selected);
EmbAJAXMutableSpan destination5_d("destination5_d");
EmbAJAXRadioGroup<12> destination6("destination6", destination_opts, selected);
EmbAJAXMutableSpan destination6_d("destination6_d");
EmbAJAXRadioGroup<12> destination7("destination7", destination_opts, selected);
EmbAJAXMutableSpan destination7_d("destination7_d");
EmbAJAXRadioGroup<12> destination8("destination8", destination_opts, selected);
EmbAJAXMutableSpan destination8_d("destination8_d");
EmbAJAXRadioGroup<12> destination9("destination9", destination_opts, selected);
EmbAJAXMutableSpan destination9_d("destination9_d");
EmbAJAXRadioGroup<12> destination10("destination10", destination_opts, selected);
EmbAJAXMutableSpan destination10_d("destination10_d");
EmbAJAXRadioGroup<12> destinationStop("destinationStop", destination_opts, selected);
EmbAJAXMutableSpan destinationStop_d("destinationStop_d");

// construction of the RFID tags to be inserted

EmbAJAXTextInput<BUFLENTAG> tag0("tag0"); EmbAJAXMutableSpan tag0_d("tag0_d"); char tag0_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag1("tag1"); EmbAJAXMutableSpan tag1_d("tag1_d"); char tag1_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag2("tag2"); EmbAJAXMutableSpan tag2_d("tag2_d"); char tag2_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag3("tag3"); EmbAJAXMutableSpan tag3_d("tag3_d"); char tag3_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag4("tag4"); EmbAJAXMutableSpan tag4_d("tag4_d"); char tag4_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag5("tag5"); EmbAJAXMutableSpan tag5_d("tag5_d"); char tag5_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag6("tag6"); EmbAJAXMutableSpan tag6_d("tag6_d"); char tag6_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag7("tag7"); EmbAJAXMutableSpan tag7_d("tag7_d"); char tag7_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag8("tag8"); EmbAJAXMutableSpan tag8_d("tag8_d"); char tag8_buf[BUFLENTAG];
EmbAJAXTextInput<BUFLENTAG> tag9("tag9"); EmbAJAXMutableSpan tag9_d("tag9_d"); char tag9_buf[BUFLENTAG];

// construction of the car names to be inserted

EmbAJAXTextInput<BUFLENCAR> car0("car0"); EmbAJAXMutableSpan car0_d("car0_d"); char car0_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car1("car1"); EmbAJAXMutableSpan car1_d("car1_d"); char car1_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car2("car2"); EmbAJAXMutableSpan car2_d("car2_d"); char car2_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car3("car3"); EmbAJAXMutableSpan car3_d("car3_d"); char car3_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car4("car4"); EmbAJAXMutableSpan car4_d("car4_d"); char car4_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car5("car5"); EmbAJAXMutableSpan car5_d("car5_d"); char car5_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car6("car6"); EmbAJAXMutableSpan car6_d("car6_d"); char car6_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car7("car7"); EmbAJAXMutableSpan car7_d("car7_d"); char car7_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car8("car8"); EmbAJAXMutableSpan car8_d("car8_d"); char car8_buf[BUFLENCAR];
EmbAJAXTextInput<BUFLENCAR> car9("car9"); EmbAJAXMutableSpan car9_d("car9_d"); char car9_buf[BUFLENCAR];

/////////////////// servo slider /////////

EmbAJAXSlider sliderUp("sliderUp", 0, 180, 90); EmbAJAXMutableSpan sliderUp_d("sliderUp_d");
char sliderUp_d_buf[BUFLENSLIDERUP];

EmbAJAXSlider sliderDown("sliderDown", 0, 180, 90); EmbAJAXMutableSpan sliderDown_d("sliderDown_d");
char sliderDown_d_buf[BUFLENSLIDERDOWN];

const char* decoupler_opts[] = {"Up", "Down"};
EmbAJAXRadioGroup<2> decoupler("decoupler", decoupler_opts);
EmbAJAXMutableSpan decoupler_d("decoupler_d");

EmbAJAXCheckButton ackUp("ackUp", "");
EmbAJAXMutableSpan ackUp_d("ackUp_d");

EmbAJAXCheckButton ackDown("ackDown", "");
EmbAJAXMutableSpan ackDown_d("ackDown_d");

EmbAJAXCheckButton AdjustServo("AdjustServo", "");
EmbAJAXMutableSpan AdjustServo_d("AdjustServo_d");
///////////////// save acknowledgement ////////

EmbAJAXCheckButton checkCarTag("checkCarTag", ""); EmbAJAXMutableSpan checkCarTag_d("checkCarTag_d");
EmbAJAXCheckButton checkTrack("checkTrack", ""); EmbAJAXMutableSpan checkTrack_d("checkTrack_d");

// construction of the table - cel/row behaviour

EmbAJAXStatic nextCell("</td><td>&nbsp;</td><td><b>"); EmbAJAXStatic nextRow("</b></td></tr><tr><td>");

////////////////////////////////////////// construction of the Track Selection page ////////////////////////////////////////

MAKE_EmbAJAXPage(page1, "TrackSelection", "",

                 new EmbAJAXStatic("<style>table, th, td {border: 1px solid black;border-collapse: collapse;margin-left: 25px;}</style>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 60;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div1 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> p1 {margin-left: 100;}</style></head>"),
                 new EmbAJAXStatic("<head><style> r1 {margin-left: 800px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {font-family: 'Roboto', sans-serif;font-size: 24px;text-align: center;color: black;background-color:#99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid #99ffcc;border-radius: 8px;padding:8px;width: 150px;margin-left: 470px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Roboto', sans-serif;text-align: center;color: black;background-color:#99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h3 {font-family: 'Roboto', sans-serif;text-align: center;color: black;background-color:#99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {border: 2px solid lightblue;border-radius: 8px;font-size: 32px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {background-color: #00878F;color: white;font-family: 'Roboto', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {box-shadow: 1px 1px 5px #555555;padding:24px;width: 920px;}</style></head>"),

                 new EmbAJAXStatic("<h4>""ELTRACO</h4>"),
                 new EmbAJAXStatic("<div1>"), &checkTrack, &checkTrack_d,
                 new EmbAJAXStatic("</div1>"),
                 new EmbAJAXStatic("<h1>""SHUNTING</h1>"),

                 new EmbAJAXStatic("<table>"), &nextRow,
                 new EmbAJAXStatic("<h3>nr.<br></h3>"), &nextCell,
                 new EmbAJAXStatic("<h3>car</h3>"), &nextCell,
                 new EmbAJAXStatic("<h2>destination track</h2>"), &nextRow,
                 new EmbAJAXStatic(" - 1 - "), &nextCell, &car0, &nextCell, &destination0, &nextRow,
                 new EmbAJAXStatic(" - 2 - "), &nextCell, &car1, &nextCell, &destination1, &nextRow,
                 new EmbAJAXStatic(" - 3 - "), &nextCell, &car2, &nextCell, &destination2, &nextRow,
                 new EmbAJAXStatic(" - 4 - "), &nextCell, &car3, &nextCell, &destination3, &nextRow,
                 new EmbAJAXStatic(" - 5 - "), &nextCell, &car4, &nextCell, &destination4, &nextRow,
                 new EmbAJAXStatic(" - 6 - "), &nextCell, &car5, &nextCell, &destination5, &nextRow,
                 new EmbAJAXStatic(" - 7 - "), &nextCell, &car6, &nextCell, &destination6, &nextRow,
                 new EmbAJAXStatic(" - 8 - "), &nextCell, &car7, &nextCell, &destination7, &nextRow,
                 new EmbAJAXStatic(" - 9 - "), &nextCell, &car8, &nextCell, &destination8, &nextRow,
                 new EmbAJAXStatic(" - 10 - "), &nextCell, &car9, &nextCell, &destination9,
                 new EmbAJAXStatic("</table>"),

                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<p1>Link status</p1>"),
                 new EmbAJAXStatic("<div>"),
                 new EmbAJAXConnectionIndicator(),
                 new EmbAJAXStatic("</div>"),
                 new EmbAJAXStatic("<r1><a href =\"/page2\">CAR and RFID tag</a></r1>"),
                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<r1><a href =\"/page3\">adjust SERVO</a></r1>"),

                )
///////////////////////////////////////////////////// construction of the car with RFID tag page //////////////////////////////////////
MAKE_EmbAJAXPage(page2, "CarTag", "",
                 new EmbAJAXStatic("<style>table, th, td {border: 1px solid black;border-collapse: collapse;margin-left: 10px;}</style>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 60;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 60;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div1 {margin-left: 40;}</style></head>"),
                 new EmbAJAXStatic("<head><style> p1 {margin-left: 100;}</style></head>"),
                 new EmbAJAXStatic("<head><style> r1 {margin-left: 370px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {font-family: 'Roboto', sans-serif;font-size: 24px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid #99ffcc;border-radius: 8px;padding: 8px;width: 230px;margin-left: 125px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h2 {font-family: 'Roboto', sans-serif;text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h3 {font-family: 'Roboto', sans-serif;text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {border: 2px solid lightblue;border-radius: 8px;font-size: 32px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {background-color: #00878F;color: white;font-family: 'Roboto', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {box-shadow: 1px 1px 5px #555555;padding: 24px;width: 435px;}</style></head>"),

                 new EmbAJAXStatic("<h4>""ELTRACO</h4>"),
                 new EmbAJAXStatic("<div1>"), &checkCarTag, &checkCarTag_d,
                 new EmbAJAXStatic("</div1>"),
                 new EmbAJAXStatic("<h1>""CAR and RFID tag</h1>"),

                 new EmbAJAXStatic("<table>"), &nextRow,
                 new EmbAJAXStatic("<h3>nr.</h3>"), &nextCell,
                 new EmbAJAXStatic("<h3>car</h3>"), &nextCell,
                 new EmbAJAXStatic("<h3>designator</h3>"), &nextCell,
                 new EmbAJAXStatic("<h3>rfid</h3>"), &nextCell,
                 new EmbAJAXStatic("<h3>code</h3>"), &nextRow,
                 new EmbAJAXStatic("- 1 -"), &nextCell, &car0, &nextCell, &car0_d, &nextCell, &tag0, &nextCell, &tag0_d, &nextRow,
                 new EmbAJAXStatic("- 2 -"), &nextCell, &car1, &nextCell, &car1_d, &nextCell, &tag1, &nextCell, &tag1_d, &nextRow,
                 new EmbAJAXStatic("- 3 -"), &nextCell, &car2, &nextCell, &car2_d, &nextCell, &tag2, &nextCell, &tag2_d, &nextRow,
                 new EmbAJAXStatic("- 4 -"), &nextCell, &car3, &nextCell, &car3_d, &nextCell, &tag3, &nextCell, &tag3_d, &nextRow,
                 new EmbAJAXStatic("- 5 -"), &nextCell, &car4, &nextCell, &car4_d, &nextCell, &tag4, &nextCell, &tag4_d, &nextRow,
                 new EmbAJAXStatic("- 6 -"), &nextCell, &car5, &nextCell, &car5_d, &nextCell, &tag5, &nextCell, &tag5_d, &nextRow,
                 new EmbAJAXStatic("- 7 -"), &nextCell, &car6, &nextCell, &car6_d, &nextCell, &tag6, &nextCell, &tag6_d, &nextRow,
                 new EmbAJAXStatic("- 8 -"), &nextCell, &car7, &nextCell, &car7_d, &nextCell, &tag7, &nextCell, &tag7_d, &nextRow,
                 new EmbAJAXStatic("- 9 -"), &nextCell, &car8, &nextCell, &car8_d, &nextCell, &tag8, &nextCell, &tag8_d, &nextRow,
                 new EmbAJAXStatic("- 10 -"), &nextCell, &car9, &nextCell, &car9_d, &nextCell, &tag9, &nextCell, &tag9_d,
                 new EmbAJAXStatic("</table>"),

                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<p1>Link status</p1>"),
                 new EmbAJAXStatic("<div>"),
                 new EmbAJAXConnectionIndicator(),
                 new EmbAJAXStatic("</div>"),
                 new EmbAJAXStatic("<r1><a href=\"/\">SHUNTING</a></r1>"),
                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<r1><a href =\"/page3\">adjust SERVO</a></r1>"),

                )
///////////////////////////////////////////////////////// construction of servo adjust page //////////////////////////////////////
MAKE_EmbAJAXPage(page3, "ServoAdjust", "",
                 new EmbAJAXStatic("<head><style> r1 {margin-left: 330px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> p1 {margin-left: 50;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div {margin-left: 35;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div2 {margin-left: 90;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div3 {margin-left: 10;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div4 {margin-left: 20;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div5 {margin-left: 50;}</style></head>"),
                 new EmbAJAXStatic("<head><style> div6 {margin-left: 5;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {font-family: 'Roboto', sans-serif;font-size: 24px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {text-align: center;color: black;background-color: #99ffcc;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h1 {border: 2px solid #99ffcc;border-radius: 8px;padding: 8px;width: 150px;margin-left: 160px;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {border: 2px solid lightblue;border-radius: 8px;font-size: 32px;text-align: center;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {background-color: #00878F;color: white;font-family: 'Roboto', sans-serif;}</style></head>"),
                 new EmbAJAXStatic("<head><style> h4 {box-shadow: 1px 1px 5px #555555;padding: 24px;width: 435px;}</style></head>"),

                 new EmbAJAXStatic("<h4>""ELTRACO</h4>"),
                 new EmbAJAXStatic("<h1>""Adjust servo</h1>"),

                 new EmbAJAXStatic("<br><br><br><div5>"), &AdjustServo,
                 new EmbAJAXStatic("</div5>"),
                 new EmbAJAXStatic("<div6>"), &AdjustServo_d,
                 new EmbAJAXStatic("</div6>"),

                 new EmbAJAXStatic("<br><br><br><br>"),
                 new EmbAJAXStatic("<div2>"), &sliderUp,
                 new EmbAJAXStatic("</div2>"),
                 new EmbAJAXStatic("<div3>"), &sliderUp_d,
                 new EmbAJAXStatic("</div3>"),

                 new EmbAJAXStatic("<div4>"), &ackUp,
                 new EmbAJAXStatic("</div4>"),
                 new EmbAJAXStatic("<div3>"), &ackUp_d,
                 new EmbAJAXStatic("</div3>"),

                 new EmbAJAXStatic("<br><br><br>"),
                 new EmbAJAXStatic("<div2>"), &sliderDown,
                 new EmbAJAXStatic("</div2>"),
                 new EmbAJAXStatic("<div3>"), &sliderDown_d,
                 new EmbAJAXStatic("</div3>"),

                 new EmbAJAXStatic("<div4>"), &ackDown,
                 new EmbAJAXStatic("</div4>"),
                 new EmbAJAXStatic("<div3>"), &ackDown_d,
                 new EmbAJAXStatic("</div3>"),

                 new EmbAJAXStatic("<br><br><br><br>"),
                 new EmbAJAXStatic("<p1>Link status</p1>"),
                 new EmbAJAXStatic("<div>"),
                 new EmbAJAXConnectionIndicator(),
                 new EmbAJAXStatic("</div>"),

                 new EmbAJAXStatic("<r1><a href=\"/\">SHUNTING</a></r1>"),
                 new EmbAJAXStatic("<br>"),
                 new EmbAJAXStatic("<r1><a href =\"/page2\">CAR and RFID tag</a></r1>"),

                )

// One page handler for each page. Note that these are essentially identical, except for the page object
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
//////////////////////////////////////////////////// setup /////////////////
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting up");
  Serial.println();
  memset(car, 0, sizeof(car));
  memset(tag, 0, sizeof(car));
  memset(track, 0, sizeof(car));
  StartAP();
  StartEEPROM();
  Config();
  UpdateUI();
  StartShunting();
  Serial.println("Start up finished");
} // end of setup
//////////////////////////////////////////////////// end of setup /////////////////
//////////////////////////////////////////////////////////// loop /////////////////////////
void loop() {
  yield();
  server.handleClient();
  UpdateTrack();
  UpdateCarTag();
  UpdateServoAngle();
}
///////////////////////////////////////////////////////////// end of loop //////////////////////
/*
   StartAP()

   function : setup WIFI access point, start server handling
   called by: setup()

*/
void StartAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig (IPAddress (192, 168, 4, 1), IPAddress (0, 0, 0, 0), IPAddress (255, 255, 255, 0));
  WiFi.softAP("EmbAJAXTest", "test");

  server.on("/", handlePage1);// Tell the server to handle pages
  server.on("/page2", handlePage2);
  server.on("/page3", handlePage3);
  server.begin();
}

/*
   StartEEPROM()

   function : prepare EEPROM for first use
   called by: setup()

*/
void StartEEPROM() {
  EEPROM.begin(4000);
  if ((EEPROM.read(4000) == 0xFF) && (EEPROM.read(4001) == 0xFF)) {                    //eeprom empty, first run
    Serial.println(" ******* EPROM EMPTY....Setting Default EEPROM values *****");
    for (int index = 0; index < 4000; index++) {
      EEPROM.write(index, 0);
    }
    EEPROM.commit();
    delay(5000);
  }
} // end of StartEEPROM()

/*
  Config()

  function : establish configuration of decoder
  called by: StartUp()
  calls    : StartPosition()

*/
void Config() {
  turnoutId = 0;
  turnoutNr = 1;                // number of turnouts for the PCB
  servoPin[0] = D6;             // servo pin number
  inverted[0] = false;          // if angleservoPos 0 > servoPos 1 than inverted = true
  relais[0] =  D0;              // relais pin number
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
  StartPosition();
} //end of Config()

/*
  UpdateUI

  Function : update return value of variables on screen
  Called by: setup, StartShunting

*/
void UpdateUI() {
  destination0_d.setValue(destination_opts[destination0.selectedOption()]);
  destination1_d.setValue(destination_opts[destination1.selectedOption()]);
  destination2_d.setValue(destination_opts[destination2.selectedOption()]);
  destination3_d.setValue(destination_opts[destination3.selectedOption()]);
  destination4_d.setValue(destination_opts[destination4.selectedOption()]);
  destination5_d.setValue(destination_opts[destination5.selectedOption()]);
  destination6_d.setValue(destination_opts[destination6.selectedOption()]);
  destination7_d.setValue(destination_opts[destination7.selectedOption()]);
  destination8_d.setValue(destination_opts[destination8.selectedOption()]);
  destination9_d.setValue(destination_opts[destination9.selectedOption()]);

  tag0_d.setValue(strncpy(tag0_buf, tag0.value(), BUFLENTAG));
  tag1_d.setValue(strncpy(tag1_buf, tag1.value(), BUFLENTAG));
  tag2_d.setValue(strncpy(tag2_buf, tag2.value(), BUFLENTAG));
  tag3_d.setValue(strncpy(tag3_buf, tag3.value(), BUFLENTAG));
  tag4_d.setValue(strncpy(tag4_buf, tag4.value(), BUFLENTAG));
  tag5_d.setValue(strncpy(tag5_buf, tag5.value(), BUFLENTAG));
  tag6_d.setValue(strncpy(tag6_buf, tag6.value(), BUFLENTAG));
  tag7_d.setValue(strncpy(tag7_buf, tag7.value(), BUFLENTAG));
  tag8_d.setValue(strncpy(tag8_buf, tag8.value(), BUFLENTAG));
  tag9_d.setValue(strncpy(tag9_buf, tag9.value(), BUFLENTAG));

  car0_d.setValue(strncpy(car0_buf, car0.value(), BUFLENCAR));
  car1_d.setValue(strncpy(car1_buf, car1.value(), BUFLENCAR));
  car2_d.setValue(strncpy(car2_buf, car2.value(), BUFLENCAR));
  car3_d.setValue(strncpy(car3_buf, car3.value(), BUFLENCAR));
  car4_d.setValue(strncpy(car4_buf, car4.value(), BUFLENCAR));
  car5_d.setValue(strncpy(car5_buf, car5.value(), BUFLENCAR));
  car6_d.setValue(strncpy(car6_buf, car6.value(), BUFLENCAR));
  car7_d.setValue(strncpy(car7_buf, car7.value(), BUFLENCAR));
  car8_d.setValue(strncpy(car8_buf, car8.value(), BUFLENCAR));
  car9_d.setValue(strncpy(car9_buf, car9.value(), BUFLENCAR));

  checkCarTag_d.setValue(checkCarTag.isChecked() ? "Car/Tag saved" : "Car/Tag NOT saved, check box");
  checkTrack_d.setValue(checkTrack.isChecked() ? "Track saved" : "Track NOT saved, check box");

  if (checkCarTag.isChecked() == 1) updateCarTagFlag = true;
  if (checkTrack.isChecked() == 1)  updateTrackFlag = true;

  sliderUp_d.setValue(itoa(sliderUp.intValue(), sliderUp_d_buf, 10));
  sliderDown_d.setValue(itoa(sliderDown.intValue(), sliderDown_d_buf, 10));

  ackUp_d.setValue(ackUp.isChecked() ? "UP confirmed" : "Confirm UP");
  ackDown_d.setValue(ackDown.isChecked() ? "DOWN confirmed" : "Confirm DOWN");
  AdjustServo_d.setValue(AdjustServo.isChecked() ? "Stop Servo Adjust" : "Start Servo Adjust");
  if (AdjustServo.isChecked()) adjustServoFlag = true;
  if (!AdjustServo.isChecked()) {
    adjustServoFlag = false;
    ackStraight = false;
    ackThrown = false;
  }
  if (ackUp.isChecked()) ackStraight = true;
  if (ackDown.isChecked()) ackThrown = true;

} // end of UpdateUI


/*
  StartShunting

  Function : load tables from EEPROM, refresh UI
  Called by: setup
  Calls    : ReadEEPROMCar, ReadEEPROMTag, UpdateUI, ReadEEPROMTrack, PrintDestinationTable

*/
void StartShunting() {
  if (startupFlag == false) {
    startupFlag = true;
    ReadEEPROMAngle();
    PrintText("read angles from EEPROM");
    PrintSingleAngle();
    ReadEEPROMCar();
    ReadEEPROMTag();
    ReadEEPROMTrack();

    car0.setValue(car[0]);
    car1.setValue(car[1]);
    car2.setValue(car[2]);
    car3.setValue(car[3]);
    car4.setValue(car[4]);
    car5.setValue(car[5]);
    car6.setValue(car[6]);
    car7.setValue(car[7]);
    car8.setValue(car[8]);
    car9.setValue(car[9]);

    ConvertTagToChar(tag[0][1], tag[0][0]);
    tag0.setValue(tagChar);
    ConvertTagToChar(tag[1][1], tag[1][0]);
    tag1.setValue(tagChar);
    ConvertTagToChar(tag[2][1], tag[2][0]);
    tag2.setValue(tagChar);
    ConvertTagToChar(tag[3][1], tag[3][0]);
    tag3.setValue(tagChar);
    ConvertTagToChar(tag[4][1], tag[4][0]);
    tag4.setValue(tagChar);
    ConvertTagToChar(tag[5][1], tag[5][0]);
    tag5.setValue(tagChar);
    ConvertTagToChar(tag[6][1], tag[6][0]);
    tag6.setValue(tagChar);;
    ConvertTagToChar(tag[7][1], tag[7][0]);
    tag7.setValue(tagChar);
    ConvertTagToChar(tag[8][1], tag[8][0]);
    tag8.setValue(tagChar);;
    ConvertTagToChar(tag[9][1], tag[9][0]);
    tag9.setValue(tagChar);

    destination0.selectOption(track[0]);
    destination1.selectOption(track[1]);
    destination2.selectOption(track[2]);
    destination3.selectOption(track[3]);
    destination4.selectOption(track[4]);
    destination5.selectOption(track[5]);
    destination6.selectOption(track[6]);
    destination7.selectOption(track[7]);
    destination8.selectOption(track[8]);
    destination9.selectOption(track[9]);

    sliderUp.setValue(servoPos[0][0]);
    sliderDown.setValue(servoPos[0][1]);

    UpdateUI();

    PrintDestinationTable();

  }
} // end of StartShunting

/*
  UpdateTrack

  Function : update Track table, write table to EEPROM, read and display tables
  Called by: loop
  Calls    : WriteEEPROMTrack, ReadEEPROMTrack, PrintDestinationTable

*/
void UpdateTrack() {
  if (updateTrackFlag == true) {
    memset(track, 0, sizeof(track));
    updateTrackFlag = false;

    track[0] = byte((destination0.selectedOption()));
    track[1] = byte((destination1.selectedOption()));
    track[2] = byte((destination2.selectedOption()));
    track[3] = byte((destination3.selectedOption()));
    track[4] = byte((destination4.selectedOption()));
    track[5] = byte((destination5.selectedOption()));
    track[6] = byte((destination6.selectedOption()));
    track[7] = byte((destination7.selectedOption()));
    track[8] = byte((destination8.selectedOption()));
    track[9] = byte((destination9.selectedOption()));

    WriteEEPROMTrack();
    ReadEEPROMTrack();
    checkTrack.setChecked(false);
    UpdateUI();
    PrintDestinationTable();
  }
} // end of UpdateTrack

/*
  UpdateCarTag

  Function : update Car table, update Tag table, write tables to EEPROM, read and display tables
  Called by: loop
  Calls    : ConvertTagLowByte, ConvertTagHighByte, WriteEEPROMTag, WriteEEPROMCar, ReadEEPROMTag,
             ReadEEPROMCar

*/
void UpdateCarTag() {
  if (updateCarTagFlag == true) {
    memset(car, 0, sizeof(car));
    memset(tag, 0, sizeof(tag));
    updateCarTagFlag = false;

    strncpy(car[0], car0_buf, strlen(car0_buf));
    strncpy(car[1], car1_buf, strlen(car1_buf));
    strncpy(car[2], car2_buf, strlen(car2_buf));
    strncpy(car[3], car3_buf, strlen(car3_buf));
    strncpy(car[4], car4_buf, strlen(car4_buf));
    strncpy(car[5], car5_buf, strlen(car5_buf));
    strncpy(car[6], car6_buf, strlen(car6_buf));
    strncpy(car[7], car7_buf, strlen(car7_buf));
    strncpy(car[8], car8_buf, strlen(car8_buf));
    strncpy(car[9], car9_buf, strlen(car9_buf));

    tag[0][0] = ConvertTagLowByte(tag0_buf[0], tag0_buf[1], tag0_buf[2], tag0_buf[3], tag0_buf[4]);
    tag[0][1] = ConvertTagHighByte();
    tag[1][0] = ConvertTagLowByte(tag1_buf[0], tag1_buf[1], tag1_buf[2], tag1_buf[3], tag1_buf[4]);
    tag[1][1] = ConvertTagHighByte();
    tag[2][0] = ConvertTagLowByte(tag2_buf[0], tag2_buf[1], tag2_buf[2], tag2_buf[3], tag2_buf[4]);
    tag[2][1] = ConvertTagHighByte();
    tag[3][0] = ConvertTagLowByte(tag3_buf[0], tag3_buf[1], tag3_buf[2], tag3_buf[3], tag3_buf[4]);
    tag[3][1] = ConvertTagHighByte();
    tag[4][0] = ConvertTagLowByte(tag4_buf[0], tag4_buf[1], tag4_buf[2], tag4_buf[3], tag4_buf[4]);
    tag[4][1] = ConvertTagHighByte();
    tag[5][0] = ConvertTagLowByte(tag5_buf[0], tag5_buf[1], tag5_buf[2], tag5_buf[3], tag5_buf[4]);
    tag[5][1] = ConvertTagHighByte();
    tag[6][0] = ConvertTagLowByte(tag6_buf[0], tag6_buf[1], tag6_buf[2], tag6_buf[3], tag6_buf[4]);
    tag[6][1] = ConvertTagHighByte();
    tag[7][0] = ConvertTagLowByte(tag7_buf[0], tag7_buf[1], tag7_buf[2], tag7_buf[3], tag7_buf[4]);
    tag[7][1] = ConvertTagHighByte();
    tag[8][0] = ConvertTagLowByte(tag8_buf[0], tag8_buf[1], tag8_buf[2], tag8_buf[3], tag8_buf[4]);
    tag[8][1] = ConvertTagHighByte();
    tag[9][0] = ConvertTagLowByte(tag9_buf[0], tag9_buf[1], tag9_buf[2], tag9_buf[3], tag9_buf[4]);
    tag[9][1] = ConvertTagHighByte();

    WriteEEPROMTag();
    WriteEEPROMCar();
    ReadEEPROMTag();
    ReadEEPROMCar();
    checkCarTag.setChecked(false);
    UpdateUI();
  }
} // end of UpdateCarTag

/*
  UpdateServoAngle

  Function :
  Called by:

*/
void UpdateServoAngle() { ///////////////////////////////
  if (adjustServoFlag == true) {
    if ((ackStraight == false) && (ackThrown == false)) {
      servoAngle = byte(sliderUp.intValue());
      ServoAdjust(0, 0, servoAngle);
    }
    if ((ackStraight == true) && (ackThrown == false)) {
      servoAngle = byte(sliderDown.intValue());
      ServoAdjust(0, 1, servoAngle);
    }
    if ((ackStraight == true) && (ackThrown == true)) {  // cycle concluded when both acks are received
      PrintText("store angles in EEPROM");
      PrintSingleAngle();
      WriteEEPROMAngle();
      ReadEEPROMAngle();                                     // verification of write action
      PrintText("read angles from EEPROM");
      PrintSingleAngle();
      adjustServoFlag = false;
      ackStraight = false;
      ackThrown = false;
      ackUp.setChecked(false);
      ackDown.setChecked(false);
      AdjustServo.setChecked(false);
      UpdateUI();
    }
  }
} // end of UpdateServoAngle

/*
  PrintSingleAngle()

  function : display angles of single servo
  called by: Config()

*/
void PrintSingleAngle() {
#ifdef DEBUG
  Serial.println();
  Serial.print("          servo: angle Straight - ");
  Serial.println(servoPos[0][0]);
  Serial.print("                 angle Thrown   - ");
  Serial.println(servoPos[0][1]);
  Serial.println();
#endif
} // end of PrintSingleAngle()

/*
  ServoAdjust

  function : adjust straight and thrown position of servo. positions sent by configuration web page.
  called by: webSocketEvent
  calls    : WriteEEPROMAngle(), ReadEEPROMAngle()

*/
void ServoAdjust(byte id, byte strthr, byte angle) {
  if ((angle) != (servoAngleOld)) {
#ifdef DEBUG
    if (strthr == 0) {
      Serial.print("angle STRAIGHT: ");
    }
    if (strthr == 1) {
      Serial.print("angle THROWN: ");
    }
    Serial.println(angle);
#endif
    servo[id].write(angle);                                // servo takes position sent by servo tool
    servoAngleOld = servo[id].read();
  }
  if ((ackStraight == false) && (ackThrown == false)) {
    servoPos[id][strthr] = angle;                          // angle accepted when ack received
  }
  if ((ackStraight == true) && (ackThrown == false )) {
    servoPos[id][strthr] = angle;                          // angle accepted when ack received
  }
} //end of servoAdjust

/*
   WriteEEPROMAngle

   function : write servo angles to EEPROM
   called by: servoAdjust()

*/
void WriteEEPROMAngle() {
  byte value[4];
  value[0] = servoPos[0][0];
  value[1] = servoPos[0][1];
  value[2] = servoPos[1][0];
  value[3] = servoPos[1][1];
  for (byte index = 0; index < 4 ; index++) {
    EEPROM.write(index, value[index]);
  }
  EEPROM.commit();
} // end of WriteEEPROMAngle()

/*
   ReadEEPROMAngle

   function : read servo angles from EEPROM
   called by: Config(), ServoAdjust()

*/
void ReadEEPROMAngle() {
  byte value[4];
  for (byte index = 0; index < 4 ; index++) {
    value[index] = EEPROM.read(index);
  }
  if (value[0] != 0) {
    servoPos[0][0] = value[0];
  }
  if (value[1] != 0) {
    servoPos[0][1] = value[1];
  }
  if (value[2] != 0) {
    servoPos[1][0] = value[2];
  }
  if (value[3] != 0) {
    servoPos[1][1] = value[3];
  }
} // end of ReadEEPROMAngle()

/*
  StartPosition()

  function : move servo to middle position and back into straight position. switch relais off
  called by: Config()

*/
void StartPosition() {
  for (byte index = 0; index < turnoutNr ; index++) {
    servo[index].write(relaisSwitchPoint[index]);
    delay(500);
    servo[index].write(servoPos[index][0]);
    digitalWrite(relais[index], LOW);
  }
} // end of StartPosition()

/*
  ConvertTagToChar

  Function : RFID codes are presented as sum by the rader. This sum is converted to single
             characters
  Called by: ConvertTagToChar

*/
void ConvertTagToChar(byte a, byte b) {
  int tag = word(a, b);
  memset(tagChar, 0, sizeof(tagChar));
  if (tag < 10) {
    tagChar[0] = tag + 48;
  } else {
    if (tag < 100) {
      tagChar[1] = tag % 10 + 48;
      tagChar[0] = (tag / 10) % 10 + 48;
    } else {
      if (tag < 1000) {
        tagChar[2] = tag % 10 + 48;
        tagChar[1] = (tag / 10) % 10 + 48;
        tagChar[0] = (tag / 100) % 10 + 48;
      } else {
        if (tag < 10000) {
          tagChar[3] = tag % 10 + 48;
          tagChar[2] = (tag / 10) % 10 + 48;
          tagChar[1] = (tag / 100) % 10 + 48;
          tagChar[0] = (tag / 1000) % 10 + 48;
        } else {
          if (tag < 100000) {
            tagChar[4] = tag % 10 + 48;
            tagChar[3] = (tag / 10) % 10 + 48;
            tagChar[2] = (tag / 100) % 10 + 48;
            tagChar[1] = (tag / 1000) % 10 + 48;
            tagChar[0] = (tag / 10000) % 10 + 48;
          } else {
            if (tag < 1000000) {
              tagChar[5] = tag % 10 + 48;
              tagChar[4] = (tag / 10) % 10 + 48;
              tagChar[3] = (tag / 100) % 10 + 48;
              tagChar[2] = (tag / 1000) % 10 + 48;
              tagChar[1] = (tag / 10000) % 10 + 48;
              tagChar[0] = (tag / 100000) % 10 + 48;
            }
          }
        }
      }
    }
  }
} // end of ConvertTagToChar

/*
  ConvertTagLowByte

  Function : convert five bytes into sum and lowbyte
  Called by: UpdateCarTag

*/
byte ConvertTagLowByte(byte a0, byte a1, byte a2, byte a3, byte a4) {
  RFIDsum = 0;
  if (a4 > 0) {
    RFIDsum = (a0 - 48) * 10000 + (a1 - 48) * 1000 + (a2 - 48) * 100 + (a3 - 48) * 10 + (a4 - 48);
  }
  if (a4 == 0) {
    RFIDsum = (a0 - 48) * 1000 + (a1 - 48) * 100 + (a2 - 48) * 10 + (a3 - 48);
  }
  if  ((a3 == 0) && (a4 == 0)) {
    RFIDsum = (a0 - 48) * 100 + (a1 - 48) * 10 + (a2 - 48);
  }
  if ((a2 == 0) && (a3 == 0) && (a4 == 0)) {
    RFIDsum = (a0 - 48) * 10 + a1 - 48;
  }
  if ((a1 == 0) && (a2 == 0) && (a3 == 0) && (a4 == 0)) {
    RFIDsum = a0 - 48;
  }
  if ((a0 == 0) && (a1 == 0) && (a2 == 0) && (a3 == 0) && (a4 == 0)) {
    RFIDsum = 0;
  }
  return lowByte(RFIDsum);
} // end of ConvertTagLowByte

/*
  ConvertTagHighByte

  Function : convert sum into highbyte
  Called by: UpdateCarTag

*/
byte ConvertTagHighByte() {
  return highByte(RFIDsum);
} // end of ConvertTagHighByte

/*
  WriteEEPROMTrack

  Function : write track table to EEPROM
  Called by: UpdateTrack

*/
void WriteEEPROMTrack() {
  for (int index = 0 ; index < carAmount; index++) {
    EEPROM.write(EEPROMOffset + index, track[index]);
  }
  EEPROM.commit();
} // end of WriteEEPROMTrack

/*
  WriteEEPROMTag

  Function : write Tag table to EEPROM
  Called by: UpdateCarTag

*/
void WriteEEPROMTag() {
  for (int index = 0 ; index < tagAmount; index++) {
    for (int index2 = 0 ; index2 < 2; index2++) {
      EEPROM.write((EEPROMOffset + 150 + index * 2 + index2), tag[index][index2]);
    }
  }
} // end of WriteEEPROMTag

/*
  ReadEEPROMTag

  Function : read Tag table from EEPROM
  Called by: Update CarTag, StartShunting

*/
void ReadEEPROMTag() {
  for (int index = 0 ; index < tagAmount; index++) {
    for (int index2 = 0 ; index2 < 2; index2++) {
      tag[index][index2] = EEPROM.read(EEPROMOffset + 150 + index * 2 + index2);
    }
  }
#ifdef DEBUG
  PrintText("Tag table");
  for (int index = 0; index < tagAmount; index++) {
    if ((word(tag[index][1], tag[index][0])) > 0) {
      Serial.print("          RFID code tag - ");
      Serial.print(index + 1);
      Serial.print(" - ");
      Serial.print(word(tag[index][1], tag[index][0]));
      Serial.println();
    }
  }
  Serial.println();
#endif
} // end of ReadEEPROMTag

/*
  WriteEEPROMCar

  Function : write Car table to EEPROM
  Called by: UpdateCarTag

*/
void WriteEEPROMCar() {
  for (int index = 0 ; index < carAmount; index++) {
    for (int index1 = 0; index1 < 20; index1++) {
      EEPROM.write((EEPROMOffset + 450 + (index * 20) + index1), car[index][index1]);
    }
  }
  EEPROM.commit();
} // end of WriteEEPROMCar

/*
  ReadEEPROMTrack

  Function : read Track table from EEPROM
  Called by: UpdateTrack, StartShunting

*/
void ReadEEPROMTrack() {
  for (int index = 0 ; index < carAmount; index++) {
    track[index] = EEPROM.read(EEPROMOffset + index);
  }
#ifdef DEBUG
  PrintText("Track table");
  for (int index = 0; index < carAmount; index++) {
    if (((track[index]) < 13 ) && (track[index]) > 0) {
      Serial.print("          Track - ");
      Serial.print(index + 1);
      Serial.print(" - ");
      Serial.print(track[index]);
      Serial.println();
    }
  }
  Serial.println();
#endif
} // end of ReadEEPROMTrack

/*
  ReadEEPROMCar

  Function : read Car table from EEPROM
  Called by: UpdateCarTag, StartShunting

*/
void ReadEEPROMCar() {
  for (int index = 0 ; index < carAmount; index++) {
    for (int index1 = 0; index1 < 20; index1++) {
      car[index][index1] = EEPROM.read((EEPROMOffset + 450 + (index * 20) + index1));
    }
  }
#ifdef DEBUG
  PrintText("Car table");
  for (int index = 0 ; index < carAmount; index++) {
    if ((car[index][0]) > 0) {
      Serial.print("          car - ");
      Serial.print(index + 1);
      Serial.print(" - ");
      for (int index1 = 0; index1 < 20; index1++) {
        Serial.print(car[index][index1]);
      }
      Serial.println();
    }
  }
  Serial.println();
#endif
} // end of ReadEEPROMCar

/*
  PrintDestinationTable

  Function : print RFID tag array and destination track array to serial monitor
  Called by: StartShunting, UpdateTrack

*/
void PrintDestinationTable() {
  PrintText("Destination table");
  for (int index = 0; index < carAmount; index++) {
    if ((word(tag[index][1], tag[index][0])) > 0) {
      Serial.print("          Wagon - ");
      Serial.print(index + 1);
      Serial.print(" - ");
      Serial.print(car[index]);
      Serial.print(" with RFID code ");
      Serial.print(word(tag[index][1], tag[index][0]));
      Serial.print(" goes to track ");
      Serial.print(track[index]);
      Serial.println();
    }
  }
  Serial.println();
  PrintText(" + + + + + ");
}  // end of PrintDestinationTable

/*
  PrintText()

  function : print formatted text to serial monitor

*/
void PrintText(char* text) {
  Serial.printf(" -------- % s -------- \n", text);
  Serial.println();
} // end of PrintText

/*
  PrintTextVar()

  function : print formatted text and variable to serial monitor

*/
void PrintTextVar(char* text, int val) {
  Serial.printf(" -------- % s - % i -------- \n", text, val);
  Serial.println();
} // end of PrintTextVar


