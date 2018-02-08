
#include <PubSubClient.h>
#include <Servo.h>
#include <stdlib.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <WiFiManager.h>

#include "Notes.h"
#include "Defaults.h"

ESP8266WebServer server = ESP8266WebServer(80);       // create a web server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);    // create a websocket server on port 81
File fsUploadFile;                                    // a File variable to temporarily store the received file
Servo servo[2];
WiFiClient espClient;
PubSubClient client(espClient);
/////////////////////////////////////////////////////////////// set-up////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.println();
  system_phy_set_max_tpw(0);                     // TX power of ESP module (0 -> 0.25dBm) (0...85)
  StartEEPROM();                                 // Prepare EEPROM for first use
  StartSPIFFS();                                 // Start the SPIFFS and list all contents
  ReadEEPROMConfig();                            // Read decoder configuration data
  ReadFileFFS();                                 // Read "resetWifi" variable
  StartWifi();                                   // Start WiFi access
  StartWebSocket();                              // Start a WebSocket server
  StartMDNS();                                   // Start the mDNS responder
  StartWebServer();                              // Start a HTTP server with a file read handler and an upload handler
  StartOTA();                                    // Start Over The Air functionality
  StartMQTT();                                   // Start MQTT client
  FixedFields();                                 // load parts of outgoing message with fixed values
  startUpTimer = millis();
}
///////////////////////////////////////////////////////////////end of set-up////////////////////////
/////////////////////////////////////////////////////////////// program loop ////////////////////////////////
void loop() {
  ArduinoOTA.handle();                          // OTA handle must stay here in loop
  yield();                                      // allow processor to handle WiFi
  webSocket.loop();                             // constantly check for websocket events
  server.handleClient();                        // run the web server
  StartUp();
  switch (decoderType) {
    case 0:                   // double turnout
      CheckBuffer();
      ProcessOrderTurnout();
      ScanSensor();
      break;
    case 1:                   // single turnout
      CheckBuffer();
      ProcessOrderTurnout();
      ScanSensor();
      break;
    case 2:                   // switch
      ProcessOrderSwitch();
      break;
    case 3:                   // sensor
      ScanSensor();
      break;
  }
  if (!client.connected()) {                    // maintain connection with Mosquitto
    Reconnect();
  }
  client.loop();                                // content of client.loop can not be moved to function
  if (sendMsg == true) {                        // set sendMsg = true  to transmit message
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

///////////////////////////////////////////////////////////// Core Eltraco functions ///////////////////////
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
} //end of CheckBuffer()


/*
   ServoAdjust

   function : adjust straight and thrown position of servo. positions sent by configuration web page.
   called by: webSocketEvent
   calls    : WriteEEPROMAngle(), ReadEEPROMAngle()

*/
void ServoAdjust(byte id, byte strthr, byte angle) {
  if ((angle) != (servoAngleOld)) {
    Serial.print("angle: ");
    Serial.println(angle);
    servo[id].write(angle);                                // servo takes position sent by servo tool
    servoAngleOld = servo[id].read();
  }
  if ((ackStraight == true) && (ackThrown != true)) {
    servoPos[id][strthr] = angle;                          // angle accepted when ack received
  }
  if ((ackThrown == true) && (ackStraight != true)) {
    servoPos[id][strthr] = angle;                          // angle accepted when ack received
  }
  if ((ackStraight == true) && (ackThrown == true)) {      // cycle concluded when both acks are received
    if (debugFlag == true) {
      Serial.println();
      Serial.print("Decoder address: ");
      Serial.print(WiFi.localIP());
      Serial.print(" servo number: ");
      Serial.print(id);
      Serial.print(" angle 1 - ");
      Serial.print(servoPos[id][0]);
      Serial.print(" angle 2 - ");
      Serial.print(servoPos[id][1]);
      Serial.println();
    }
    WriteEEPROMAngle();
    memset(msgOut, 0, sizeof(msgOut));
    msgOut[0] = id;
    msgOut[4] = 1;
    sendMsg == true;
    configFlag = true;
    ReadEEPROMAngle();                                     // verification of write action
    configFlag = false;
    ackStraight = false;
    ackThrown = false;
  }
} //end of servoAdjust

/*

   ScanSensor

   function : read status of input pins. adjust status of sensor, generate outgoing message,
              sensorCountOff used to decide after how many scans "LOW" the sensor status is adjusted
              required for current detector
              set sendMsg = true to send message, set sendMsg = false for scan loop
   called by: loop
   calls .  : FixedFields();

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
        FixedFields();
        msgOut[10] = sensorStatus[index];
        msgOut[11] = addressSr[index];
      }
    }
  }
} // end of Scansensor()

/*
   Thrown

   function : move servo into thrown position
              compare current servo position with ordered position, signal start servo movement,
              switch relais in middle of servo movement, signal end of movement, set flag order executed
   called by: ProcessOrder()
   calls    : txMsgMove(), txMsgMoveStop()

*/
byte Thrown(byte toNmbr) {
  currentPosition[toNmbr] = servo[toNmbr].read();
  if ((currentPosition[toNmbr]) != (servoPos[toNmbr][1]) && millis() >= servoMoveTime[toNmbr]) {
    if (inverted[toNmbr] == true) {
      servo[toNmbr].write((currentPosition[toNmbr]) - 1);
    } else servo[toNmbr].write((currentPosition[toNmbr]) + 1);
    servoMoveTime[toNmbr] = millis() + servoDelay[toNmbr];
    if (order.msgMove == false) {
      order.msgMove = true;
      TxMsgMove(addressSr[toNmbr]);
    }
    if ((currentPosition[toNmbr]) == (relaisSwitchPoint[toNmbr])) digitalWrite(relais[toNmbr], HIGH);
  }
  if ((currentPosition[toNmbr]) == (servoPos[toNmbr][1])) {
    order.executed = true;
    if (order.msgStop == false) {
      order.msgStop = true;
      TxMsgMoveStop(addressSr[toNmbr]);
    }
  }
} // end of Thrown()

/*

   function : move servo into straight position
              compare current servo position with ordered position, signal start servo movement
              switch relais in middle of servo movement, signal end of movement, set flag order executed
   called by: ProcessOrder()
   calls    : txMsgMove(), txMsgMoveStop()

*/
byte Straight(byte toNmbr) {
  currentPosition[toNmbr] = servo[toNmbr].read();
  if ((currentPosition[toNmbr]) != (servoPos[toNmbr][0]) && millis() >= servoMoveTime[toNmbr]) {
    if (inverted[toNmbr] == true) {
      servo[toNmbr].write((currentPosition[toNmbr]) + 1);
    } else servo[toNmbr].write((currentPosition[toNmbr]) - 1);
    servoMoveTime[toNmbr] = millis() + servoDelay[toNmbr];
    if (order.msgMove == false) {
      order.msgMove = true;
      TxMsgMove(addressSr[toNmbr]);
    }
    if ((currentPosition[toNmbr]) == (relaisSwitchPoint[toNmbr])) digitalWrite(relais[toNmbr], LOW);
  }
  if ((currentPosition[toNmbr]) == (servoPos[toNmbr][0])) {
    order.executed = true;
    if (order.msgStop == false) {
      order.msgStop = true;
      TxMsgMoveStop(addressSr[toNmbr]);
    }
  }
} // end of Straight()

/*
   txMsgMove

   function : signal start movement servo to Rocrail
   called by: Thrown(), Straight()

*/
byte TxMsgMove(byte msNmbr) {
  FixedFields();
  msgOut[11] = msNmbr;
  msgOut[10] = 1;
  sendMsg = true;
} // end of txMsgMove()

/*
   txMsgMoveStop

   function : signal stop movement servo to Rocrail
   called by: Thrown(), Straight()

*/
byte TxMsgMoveStop(byte msNmbr) {
  FixedFields();
  msgOut[11] = msNmbr;
  msgOut[10] = 0;
  sendMsg = true;
} // end of txMsgMoveStop()


/*
   ReadEEPROMAngle

   function : read servo angles from EEPROM
   called by: SelectConfig(), ServoAdjust()

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
  delay(1000);
} // end of WriteEEPROMAngle()

/*
   StartUp()

   function : configuration of the decoder
   called by: loop
   calls    : SelectConfig(), SoftwareVersion()
*/
void StartUp() {
  if (startUp == false) {
    if (millis() - startUpTimer > startUpDelay) {    // built in delay to allow messages stored on Mosquitto
      startUp = true;                                // to enter without being acted upon
      buf.orderNew = true;
      order.executed = true; // do NOT remove!!!!!!!
      SelectConfig();
      if (debugFlag == true) {
        SoftwareVersion();
      }
    }
  }
} // end of StartUp()

/*
   SelectConfig()

   function : establish configuration of decoder depending of decoder type
   called by: StartUp()
   calls    : ReadEEPROMAngle(), PrintSingleAngle(), StartPosition(), ConfigIO()
              PrintDoubleAngle()

*/
void SelectConfig() {
  switch (decoderType) {
    case 0:                         // double turnout decoder
      ReadEEPROMAngle();
      PrintDoubleAngle();
      turnoutNr = 2;                // number of turnouts for the PCB
      sensorNr = 2;                 // number of sensors
      servoPin[0] = D6;             // servo pin number
      servoPin[1] = D7;             // servo pin number
      inverted[0] = false;          // if angleservoPos 0 > servoPos 1 than inverted = true
      inverted[1] = false;          // if angleservoPos 0 > servoPos 1 than inverted = true
      relais[0] =  D0;              // relais pin number
      relais[1] =  D5;              // relais pin number
      servoDelay[0] = 40;           // controls speed servo movement, higher is slower
      servoDelay[1] = 40;           // controls speed servo movement, higher is slower
      addressSr[0] = 1;
      addressSr[1] = 2;
      sensor[0] = D1;
      sensor[1] = D2;
      ConfigIO();
      StartPosition();
      break;
    case 1:                         // single turnout decoder
      ReadEEPROMAngle();
      PrintSingleAngle();
      turnoutNr = 1;                // number of turnouts for the PCB
      sensorNr = 5;                 // number of sensors
      servoPin[0] = D6;             // servo pin number
      inverted[0] = false;          // if angleservoPos 0 > servoPos 1 than inverted = true
      relais[0] =  D0;              // relais pin number
      servoDelay[0] = 40;           // controls speed servo movement, higher is slower
      for (int index = 0; index < 5; index++) {
        addressSr[index] = index + 1;
      }
      sensor[0] = D1;               // sensor pin numbers
      sensor[1] = D2;
      sensor[2] = D3;
      sensor[3] = D4;
      sensor[4] = D5;
      ConfigIO();
      StartPosition();
      break;
    case 2:                         // switch decoder
      outputNr = 8;                 // number of outputs
      output[0] = D0;               // output pins
      output[1] = D1;
      output[2] = D2;
      output[3] = D3;
      output[4] = D4;
      output[5] = D5;
      output[6] = D6;
      output[7] = D7;
      ConfigIO();
      break;
    case 3:                         // sensor decoder
      sensorNr = 8;                 // number of sensors
      for (int index = 0; index < 8; index++) {
        addressSr[index] = index + 1;
      }
      sensor[0] = D0;               // sensor pins
      sensor[1] = D1;
      sensor[2] = D2;
      sensor[3] = D3;
      sensor[4] = D4;
      sensor[5] = D5;
      sensor[6] = D6;
      sensor[7] = D7;
      for (int index = 0; index < 8; index++) {
        sensorInverted[index] = false;
      }
      ConfigIO();
      break;
  }
} //end of SelectConfig()


/*
   PrintSingleAngle()

   function : display angles of single servo
   called by: SelectConfig()

*/
void PrintSingleAngle() {
  if (debugFlag == true) {
    Serial.println();
    Serial.println("Servo angles:");
    Serial.print("  straight: ");
    Serial.print(servoPos[0][0]);
    Serial.print("   thrown: ");
    Serial.println(servoPos[0][1]);
    Serial.println();
  }
} // end of PrintSingleAngle()

/*
   PrintDoubleAngle()

   function : display angles of double servo
   called by: SelectConfig()

*/
void PrintDoubleAngle() {
  if (debugFlag == true) {
    Serial.println();
    Serial.println(" Servo angles:");
    Serial.print("  Servo 1 - straight: ");
    Serial.print(servoPos[0][0]);
    Serial.print("   thrown: ");
    Serial.println(servoPos[0][1]);
    Serial.print("  Servo 2 - straight: ");
    Serial.print(servoPos[1][0]);
    Serial.print("   thrown: ");
    Serial.println(servoPos[1][1]);
    Serial.println();
  }
} // end of PrintDoubleAngle()
/*


  /*
   ConvertAngle()

   function : messages are numbers transmitted as ASCII characters.
              these characters are converted into byte and angle values.
              servo and one of two angles are extracted.
              controlling values are converted into flow control flags.
              data transferred to servo adjust module
   calls    : ServoAdjust()

*/
void ConvertAngle(byte lgth) {
  webAngleOld = webAngle;
  switch (lgth) {
    case 1:
      webAngle = webMsg[0] - 48;
      break;
    case 2:
      webAngle = (webMsg[0] - 48) * 10 + (webMsg[1] - 48);
      break;
    case 3:
      webAngle = (webMsg[0] - 48) * 100 + (webMsg[1] - 48) * 10 + (webMsg[2] - 48);
      break;
  }
  skip = true;
  switch (webAngle) {
    case 201:     // servo 1 Straight
      servoId = 0;
      straightThrown = 0;
      break;
    case 202:     // servo 1 Thrown
      servoId = 0;
      straightThrown = 1;
      break;
    case 203:     // servo 2 Straight
      servoId = 1;
      straightThrown = 0;
      break;
    case 204:     // servo 2 Thrown
      servoId = 1;
      straightThrown = 1;
      break;
    case 205:     // servo 1 Straight ack
      webAngle = webAngleOld;
      ackStraight = true;
      servoId = 0;
      skip = false;
      break;
    case 206:     // servo 1 Thrown ack
      webAngle = webAngleOld;
      ackThrown = true;
      servoId = 0;
      skip = false;
      break;
    case 207:     // servo 2 Straight ack
      webAngle = webAngleOld;
      servoId = 1;
      ackStraight = true;
      skip = false;
      break;
    case 208:     // servo 2 Thrown ack
      webAngle = webAngleOld;
      servoId = 1;
      ackThrown = true;
      skip = false;
      break;
    case 209:     // servo 1
      configFlag = true;
      break;
    case 210:     // servo 2
      configFlag = true;
      break;
    default:
      skip = false;
      break;
  }
  if (turnoutNr == 0) {
    Serial.println();
    Serial.println("Decoder does not control a turnout");
    skip = true;
  } else {
    if (servoId > (turnoutNr - 1)) {
      Serial.println();
      Serial.println("Decoder only controls ");
      Serial.print(turnoutNr);
      Serial.println(" turnout");
      skip = true;
      servoId = 0;
    }
  }
  if (configFlag == true) {
    if ((skip == false) && (servoAngle < 181)) {
      ServoAdjust(servoId, straightThrown, webAngle);
    }
  }
}

/*
   ProcessOrder

   function : start servo action
   called by: loop
   calls    : Thrown, Straight

*/
void ProcessOrderTurnout() {
  if (order.executed == false) {
    if ((order.in == 1)) Thrown(order.targetId - 1);
    if ((order.in == 0)) Straight(order.targetId - 1);
  }
} // end of ProcessOrderTurnout()

/*
   ProcessOrderSwitch

   function : check output orders if action required, perform output action
   called by: loop

*/
void ProcessOrderSwitch() {
  ordrOld = ordr;
  ordr = buffer;
  ordrIdOld = ordrId;
  ordrId = bufferId - 1;
  if ((ordrOld != ordr) | (ordrIdOld != ordrId)) {
    digitalWrite(output[ordrId], ordr);
  }
} // end of ProcessOrderSwitch()


/*
   FixedFields()

   function : fill fixed values outgoing message
   called by: setup(), Scansensor(), TxMsgMove(), TxMsgStop()

*/
void FixedFields() {
  msgOut[2] = 1;
  msgOut[4] = decoderId;
  msgOut[5] = 8;
  msgOut[6] = 1;
  msgOut[7] = 4;
} // end of FixedFields()


/*
   Callback

   function : receive incoming message, test topic, test on recipient,
   select returning sensor messages
   select incoming turnout switch orders, select turnout number, store order into buffer.
   select incoming switch orders, store order into buffer

*/
void Callback(const MQTT::Publish & pub) {
  if ((pub.topic()) == ("rocnet/sr")) {
    if (((byte)pub.payload()[4]) == (decoderId)) {
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

  if ((pub.topic()) == ("rocnet/ot")) {
    if (((byte)pub.payload()[2]) == (decoderId)) {
      buf.targetId = ((byte)pub.payload()[11]);
      buf.orderOld = buf.order;
      buf.order = ((byte)pub.payload()[8]);
      buf.orderNew = true;
      if (firstOrder == true) {
        order.executed = true;
        firstOrder = false;
      }
      if (debugFlag == true) {
        Serial.println();
        Serial.print("Msg received [");
        Serial.print(pub.topic());
        Serial.print(" - DEC, dotted] <== ");
        for (byte index = 0; index < (pub.payload_len()); index++) {
          Serial.print(((byte)pub.payload()[index]), DEC);
          if (index < (pub.payload_len()) - 1) Serial.print(F("."));
        }
        if ((decoderType == 0) || (decoderType == 1)) {
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
            Serial.print(F("switch turnout "));
            Serial.print(buf.targetId);
            Serial.print(" of decoder ");
            Serial.print(decoderId);
            Serial.print(F(" to "));
            Serial.print(turnoutOrder);
            Serial.println();
          }
        }

        if (decoderType == 2) {
          buffer = ((byte)pub.payload()[8]);
          bufferId = ((byte)pub.payload()[11]);
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
            if (buffer == 1) {
              ordroutput = "ON";
            } else {
              ordroutput = "OFF";
            }
            Serial.print(F("Message in ["));
            Serial.print(pub.topic());
            Serial.print(F("] "));
            Serial.print(F("switch decoder "));
            Serial.print(((byte) pub.payload()[2]));
            Serial.print(F(" output "));
            Serial.print(bufferId);
            Serial.print(F(" to "));
            Serial.print(ordroutput);
            Serial.println();
          }
        }
      }
    }
  }
} // end of Callback()


/*
   ConfigIO()

   function : configure IO for decoder type
   called by: SelectConfig()

*/
void ConfigIO() {
  switch (decoderType) {
    case 2:
      for (byte index = 0; index < outputNr; index++) {                              // initialising output pins
        pinMode(output[index], OUTPUT);
        digitalWrite(output[index], LOW);
      }
      break;
    case 3:
      for (byte index = 0; index < sensorNr; index++) {                              // initialising sensor pins
        pinMode(sensor[index], INPUT_PULLUP);
      }
      break;
    default:
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
      break;
  }
} // end of ConfigIO()


/*
   webSocketEvent

   function : handle incoming messages from webpage.

   called by: loop

   calls    : ConvertAngle(), ConvertGroup(), ReadEEPROMConfig()

*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      for (byte index = 0; index < length; index++) {
        webMsg[index] = payload[index];
      }
      if (webMsg[0] != 35) {               // ASCII code for #
        ConvertAngle(length);
      }
      else ConvertGroup(length);
  }
} // end of webSocketEvent()

/*
   ConvertGroup()

   function : convert "webMsg[2]" into group
   called by: webSocketEvent()
   calls    : ConvertGroup0Member(), ConvertGroup1Member(), ConvertGroup2Member()

*/
void ConvertGroup(byte lgth) {
  switch (webMsg[2] - 48) {
    case 0:
      ConvertGroup0Member(lgth);
      break;
    case 1:
      ConvertGroup1Member(lgth);
      break;
    case 2:
      ConvertGroup2Member(lgth);
      break;
    default:
      if (debugFlag == true) Serial.println("Notdefined");
      break;
  }
} // end of ConvertGroup()

/*
   ConvertGroup0Member()

   function : converts "webMsg[3]" into group0 member
   calls    : convertIpSub()

*/
void ConvertGroup0Member(byte lgth) {
  if (debugFlag == true) Serial.print("IP-address: ");
  switch  (webMsg[3] - 48) {
    case 0:                                      // decoder
      WriteEEPROMSingle(5, convertIpSub(lgth));
      if (debugFlag == true) {
        Serial.print("Decoder - ");
        Serial.print(decoderId);
        Serial.println();
      }
      break;
    case 1:                                      // gateway
      WriteEEPROMSingle(6, convertIpSub(lgth));
      if (debugFlag == true) {
        Serial.print("Gateway - ");
        Serial.print(subIpGateway);
        Serial.println();
      }
      break;
    case 2:                                      // mosquitto
      WriteEEPROMSingle(4, convertIpSub(lgth));
      if (debugFlag == true) {
        Serial.print("Mosquitto - ");
        Serial.print(subIpMosquitto);
        Serial.println();
      }
      break;
    default:
      if (debugFlag == true) {
        Serial.print("Notdefined");
        Serial.println();
      }
      break;
  }
  ReadEEPROMConfig();
  Serial.print(" -------- Reboot required ");
  Serial.println("----------");
  ESP.reset();
} // end of ConvertGroup0Member()

/*
   ConvertGroup1Member()

   function : converts "webMsg[3]" into group1 member

*/
void ConvertGroup1Member(byte lgth) {
  WriteEEPROMSingle(7, (webMsg[3] - 48));
  if (debugFlag == true) Serial.print("Decoder: ");
  switch  (webMsg[3] - 48) {
    case 0:
      if (debugFlag == true) {
        Serial.print("Double turnout");
        Serial.println();
      }
      break;
    case 1:
      if (debugFlag == true) {
        Serial.print("Single turnout");
        Serial.println();
      }
      break;
    case 2:
      if (debugFlag == true) {
        Serial.print("Switch");
        Serial.println();
        break;
      case 3:
        if (debugFlag == true) {
          Serial.print("Sensor");
          Serial.println();
        }
        break;
      default:
        if (debugFlag == true) {
          Serial.println("Notdefined");
          Serial.println();
        }
        break;
      }
  }
  ReadEEPROMConfig();
  Serial.print(" -------- Reboot required ");
  Serial.println("----------");
  ESP.reset();
} // end of ConvertGroup1Member()

/*
   ConvertGroup2Member()

   function : converts "webMsg[3]" into group2 member

*/
void ConvertGroup2Member(byte lgth) {
  if (debugFlag == true) Serial.print("Flag: ");
  switch  (webMsg[3] - 48) {
    case 0:
      WriteFileFFS(webMsg[4] - 48);
      if (debugFlag == true) {
        Serial.print("New WiFi network - ");
        Serial.print(webMsg[4] - 48);
        Serial.println();
      }
      break;
    case 1:
      WriteEEPROMSingle(8, (webMsg[4] - 48));
      if (debugFlag == true) {
        Serial.print("Debug - ");
        Serial.print(webMsg[4] - 48);
        Serial.println();
      }
      break;
    default:
      if (debugFlag == true) {
        Serial.println("Notdefined");
        Serial.println();
      }
      break;
  }
  ReadEEPROMConfig();
}


/*

   WriteEEPROMSingle()

   function : write a single value to EEPROM
   called by: ConvertGroup0Member(), ConvertGroup0Member(), ConvertGroup1Member(), ConvertGroup2Member()
*/
void WriteEEPROMSingle(byte location, byte value) {
  EEPROM.write(location, value);
  EEPROM.commit();
  delay(100);
} // end of WriteEEPROMSingle()

/*
   convertIpSub()

   function : convert "webMsg[4], webMsg[5], webMsg[6]" to sub IP
   called by: ConvertGroup0Member()

*/
byte convertIpSub(byte lgth) {
  byte x;
  switch (lgth) {
    case 5:
      x = webMsg[4] - 48;
      break;
    case 6:
      x = (webMsg[4] - 48) * 10 + (webMsg[5] - 48);
      break;
    case 7:
      x = (webMsg[4] - 48) * 100 + (webMsg[5] - 48) * 10 + (webMsg[6] - 48);
      break;
  }
  return x;
}

///////////////////////////////////////////////////////////// end of Core Eltraco functions ///////////////////////

///////////////////////////////////////////////////////////// Start-up functions ///////////////////////
/*
   StartPosition()

   function : move servo to middle position and back into straight position. switch relais off
   called by: setup()

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
   StartSPIFFS()

   function : Start the SPIFFS and list all contents
   called by: setup()
   calls    : FormatBytes()

*/
void StartSPIFFS() {
  SPIFFS.begin();
  Serial.println();
  Serial.print(" -------- SPIFFS started. Content: ");
  Serial.println("----------");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\t File: %s, size: %s\r\n", fileName.c_str(), FormatBytes(fileSize).c_str());
    }
    Serial.print(" -------- SPIFFS content");
    Serial.println(" ----------");
    Serial.println();
  }
} // end of StartSPIFFS()

/*
   StartWebSocket()

   function : Start a WebSocket server
   called by: setup()

*/
void StartWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println();
  Serial.print(" -------- WebSocket server started ");
  Serial.println("----------");
} // end of StartWebSocket()

/*
   StartMDNS()

   function : Start the mDNS responder
   called by: setup()

*/
void StartMDNS() {
  CreateName();
  MDNS.begin(ClientName);
  Serial.println();
  Serial.print(" -------- mDNS responder started. ");
  Serial.print("URL: http://");
  Serial.print(ClientName);
  Serial.print(".local");
  Serial.println(" ----------");
} // end of StartMDNS()

/*
   StartWebServer()

   function : Start a HTTP server with a file read handler and an upload handler
   called by: setup()
   calls    : HandleNotFound(), HandleFileUpload

*/
void StartWebServer() {
  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", "");
  }, HandleFileUpload);                       // go to 'HandleFileUpload'
  server.onNotFound(HandleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
  // and check if the file exists
  server.begin();                             // start the HTTP server
  Serial.println();
  Serial.print(" -------- HTTP server started. ");
  Serial.print("URL: http://");
  Serial.print(ipDecoder);
  Serial.println(" ----------");

} // end of StartWebServer()

/*
   StartOTA()

   function : start Over The Air function
   called by: setup()

*/
void StartOTA() {
  ArduinoOTA.setPort(8266);                         // Port defaults to 8266
  ArduinoOTA.setHostname(ClientName);               // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setPassword((const char *)"123");    // No authentication by default
  ArduinoOTA.onStart([]() {
    Serial.println();
    Serial.print(" -------- OTA started ");
    Serial.println(" ----------");
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
} // end of StartOTA()


/*
   HandleNotFound()

   function : if the requested file or page doesn't exist, return a 404 not found error
   called by: StartWebServer()
   calls    : HandleFileRead()

*/
void HandleNotFound() {
  if (!HandleFileRead(server.uri())) {                   // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
} // end of HandleNotFound()

/*
   HandleFileRead()

   function : send the right file to the client (if it exists)
   called by: HandleNotFound()
   calls    : GetContentType

*/
bool HandleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";              // If a folder is requested, send the index file
  String contentType = GetContentType(path);                 // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {    // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                           // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                      // Open the file
    size_t sent = server.streamFile(file, contentType);      // Send it to the client
    file.close();                                            // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);       // If the file doesn't exist, return false
  return false;
} // end of HandleNotFound()

/*
   HandleFileUpload()

   function : upload a new file to the SPIFFS
   called by: StartWebServer()

*/
void HandleFileUpload() {
  HTTPUpload& upload = server.upload();
  String path;
  if (upload.status == UPLOAD_FILE_START) {
    path = upload.filename;
    if (!path.startsWith("/")) path = "/" + path;
    if (!path.endsWith(".gz")) {                                                   // The file server always prefers a compressed version of a file
      String pathWithGz = path + ".gz";                                            // So if an uploaded file is not compressed, the existing compressed
      if (SPIFFS.exists(pathWithGz))                                               // version of that file must be deleted (if it exists)
        SPIFFS.remove(pathWithGz);
    }
    Serial.print("HandleFileUpload Name: "); Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");                                         // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);                          // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                                            // If the file was successfully created
      fsUploadFile.close();                                                        // Close the file again
      Serial.print("HandleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location", "/success.html");                              // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
} // end of HandleFileUpload()

/*
   FormatBytes()

   function : convert sizes in bytes to KB and MB
   called by : StartSPIFFS()

*/
String FormatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
} // end of FormatBytes()

/*
   GetContentType()

   function : determine the filetype of a given filename, based on the extension
   called by: HandleFileRead()

*/
String GetContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
} // end of GetContentType()

/*
   StartWifi()

   function: start Wifi
   called by: setup

*/
void StartWifi() {
  WiFiManager wifiManager;
  if (resetWifi == true) {
    WriteFileFFS(0);
    WriteEEPROMSingle(9, (0));
    wifiManager.resetSettings();
  }
  if (!wifiManager.autoConnect("ELTRACO AP")) {
    Serial.println("failed to connect, we should reset to see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  ipTemp = WiFi.localIP();
  for (int index = 0; index < 3; index++) {
    ipMosquitto[index] = ipTemp[index];
    ipDecoder[index] = ipTemp[index];
    ipGateway[index] = ipTemp[index];
    ipSubNet[index] = ipTemp[index];
  }
  ipMosquitto[3] = subIpMosquitto;
  ipDecoder[3] = decoderId;
  ipGateway[3] = subIpGateway;
  ipSubNet[3] = 0;
  WiFi.config(ipDecoder, ipGateway, ipSubNet);
  Serial.println();
  Serial.print(" -------- WiFi Connected to SSID: ");
  Serial.print(WiFi.SSID());
  Serial.print(" with IP-address: ");
  Serial.print(WiFi.localIP());
  Serial.println(" ----------");
} // end of StartWifi()

/*
   StartMQTT()

   function : start MQTT
   called by: setup

*/
void StartMQTT() {
  CreateName();
  client.set_server(ipMosquitto, 1883);
  static const char *MQTTclientId = ClientName;
  client.set_callback(Callback);                 // set up callback for Mosquitto
} // end of StartMQTT()

/*
   StartEEPROM()

   function : prepare EEPROM for first use
   called by: setup()

*/
void StartEEPROM() {
  EEPROM.begin(512);
  if ((EEPROM.read(255) == 0xFF) && (EEPROM.read(256) == 0xFF)) {                    //eeprom empty, first run
    Serial.println(" ******* EPROM EMPTY....Setting Default EEPROM values *****");
    for (int index = 0; index < 512; index++) {
      EEPROM.write(index, 0);
    }
    EEPROM.commit();
    delay(10000);
  }
} // end of StartEEPROM()


/*
   ReadEEPROMConfig()

   function : read servo angles and user data from EEPROM
   called by: setup

*/
void ReadEEPROMConfig() {
  byte value[15];
  for (byte index = 0; index < 9 ; index++) {
    value[index] = EEPROM.read(index);
  }
  if (value[4] != 0) {
    subIpMosquitto = value[4];
  }
  if (value[5] != 0) {
    decoderId = value[5];
  }
  if (value[6] != 0) {
    subIpGateway = value[6];
  }
  decoderType = value[7];
  debugFlag = value[8];
  //  resetWifi = value[9];
  if (debugFlag == true) {
    Serial.println();
    Serial.print(" -------- Config memory content ");
    Serial.println("----------");
    Serial.print("          Straight position servo 1: ");
    Serial.println(value[0]);
    Serial.print("          Thrown position servo 1..: ");
    Serial.println(value[1]);
    Serial.print("          Straight position servo 2: ");
    Serial.println(value[2]);
    Serial.print("          Thrown position servo 2..: ");
    Serial.println(value[3]);
    Serial.print("          IP-triplet Mosquitto.....: ");
    Serial.println(value[4]);
    Serial.print("          IP-triplet Decoder.......: ");
    Serial.println(value[5]);
    Serial.print("          IP-triplet Gateway.......: ");
    Serial.println(value[6]);
    Serial.print("          Decoder type.............: ");
    Serial.println(value[7]);
    Serial.print("          Debug flag...............: ");
    Serial.println(value[8]);
    Serial.print(" -------- Config memory content ");
    Serial.println("----------");
    Serial.println();
  }
} // end of ReadEEPROM()

/*
   re-establish connection with MQTT clientID.
   publish topic topicPub. subscribe to topic topicSub1 and topicSub2.
   when Mosquitto not available try again. after 5 attemts, leave loop

*/
void Reconnect() {
  if (connectedMosquitto == true) {
    byte retries = 0;
    while (!client.connected()) {
      if (client.connect(ClientName)) {
        Serial.println();
        Serial.print(" -------- MQTT Connected: ");
        Serial.print(ClientName);
        Serial.print(" to: ");
        Serial.print(ipMosquitto);
        Serial.println(" ----------");
        Serial.print(" -------- Supscription: ");
        Serial.println(" ----------");
        Serial.print("          ");
        Serial.println(topicSub1);
        Serial.print("          ");
        Serial.println(topicSub2);
        Serial.print(" -------- Supscription  ");
        Serial.println(" ----------");
        client.subscribe(topicSub1);
        client.subscribe(topicSub2);
      } else {
        retries++;
        Serial.print(" -------- MQTT Connection, 5 attempts to establish connection. ");
        Serial.print("Attempt number: ");
        Serial.print(retries);
        Serial.println(" ----------");
        delay(500);
        if (retries == 5) {
          Serial.println();
          Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
          Serial.println(" -------- MQTT Connection. Wrong IP triplet, correct via configuration. ----------");
          Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
          delay(2000);
          connectedMosquitto = false;
          break;
        }
      }
    }
  }
}
// end of reconnect()


/*
   CreateName()

   function : create decoder name
   called by: StartMQTT

*/
void CreateName() {
  char myName[17] = "ELTRACO-decoder-";
  char st[4] = "ST-";
  char dt[4] = "DT-";
  char sw[4] = "SW-";
  char sr[4] = "SR-";
  switch (decoderType) {
    case 0:
      sprintf(ClientName, "%s%s%i", myName, dt, decoderId);
      break;
    case 1:
      sprintf(ClientName, "%s%s%i", myName, st, decoderId);
      break;
    case 2:
      sprintf(ClientName, "%s%s%i", myName, sw, decoderId);
      break;
    case 3:
      sprintf(ClientName, "%s%s%i", myName, sr, decoderId);
      break;
  }
  Serial.println();
  Serial.print("Clientname: ");
  Serial.println(ClientName);
} // end of CreateName()

/*
   SoftwareVersion

    function : display decoder type and sofware version on serial monitor
    called by: reconnect
    calls    : NameDecoderType()

*/
void SoftwareVersion() {
  NameDecoderType();
  Serial.println();
  Serial.print("\n===================================================================");
  Serial.print("\n");
  Serial.print("\n        EEEEE  LL   TTTTTT  RRR        A        CCC     OO");
  Serial.print("\n        EE     LL     TT    RR RR    AA AA     CC     OO  OO");
  Serial.print("\n        EEEE   LL     TT    RRR     AAAAAAA   CC     OO    OO");
  Serial.print("\n        EE     LL     TT    RR RR   AA   AA    CC     OO  OO");
  Serial.print("\n        EEEEE  LLLLL  TT    RR  RR  AA   AA     CCC     OO");
  Serial.print("\n");
  Serial.print("\n===================================================================");
  Serial.println();
  Serial.print("\n                    decoder: ");
  Serial.println(text);
  Serial.print("\n                    version: ");
  Serial.print(version);
  Serial.println();
  Serial.print("\n-------------------------------------------------------------------");
  Serial.println();
} // end of SoftwareVersion()

/*
   NameDecoderType()

   function : create decoder display name
   called by: DisplayDecoderType(), SoftwareVersion()

*/
void NameDecoderType() {
  switch (decoderType) {
    case 0:
      text = "double turnout";
      break;
    case 1:
      text = "single turnout";
      break;
    case 2:
      text = "switch";
      break;
    case 3:
      text = "sensor";
      break;
  }
} // end of NameDecoderType()

/*
   ReadFileFFS()

   function : read "resetWifi" from SPIFFS file "wifi.txt"
   called by: setup

*/
void ReadFileFFS() {
  File f = SPIFFS.open("/wifi.txt", "r");
  byte cntr = 0;
  if (!f) {
    Serial.print(" -------- File doesn't exist. ");
    Serial.println(" ----------");
    // open the file in write mode
    File f = SPIFFS.open("/wifi.txt", "w");
    if (!f) {
      Serial.print(" -------- NOT able to create file. ");
      Serial.println(" ----------");
    } else {
      f.println("0");
      Serial.print(" -------- File created. ");
      Serial.println(" ----------");
    }
  } else {
    String in = f.readStringUntil('n');
    resetWifi = (in.toInt());
    Serial.print(" -------- WiFi reset on SPIFFS has value: ");
    Serial.print(resetWifi);
    Serial.println(" ----------");
    Serial.println();
    /*
        while ((f.available())&& (cntr=0)) {
          //Lets read line by line from the file
          String in = f.readStringUntil('n');
          resetWifi = (in.toInt());
          Serial.print(" -------- WiFi reset on SPIFFS has value: ");
          Serial.print(resetWifi);
          Serial.println(" ----------");
          Serial.println();
          cntr++;
        }
    */

  }
  f.close();
}

/*
   WriteFileFFS()

   function : write "resetWifi" to file "wifi.txt" in SPIFFS
   called by: ConvertGroup2Member()

*/
void WriteFileFFS(byte b) {
  File f = SPIFFS.open("/wifi.txt", "w+");
  if (b == 0) f.println("0");
  else f.println("1");
  f.close();
  f = SPIFFS.open("/wifi.txt", "r");
  String in = f.readStringUntil('n');
  boolean val = (in.toInt());
  Serial.println();
  Serial.print(" -------- WiFi reset on SPIFFS has value: ");
  Serial.print(val);
  Serial.println(" ----------");
  Serial.println();
  f.close();
}
///////////////////////////////////////////////////////////// end of Start-up functions ///////////////////////


