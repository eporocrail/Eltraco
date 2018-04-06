IPAddress ipTemp;
IPAddress ipMosquitto;
IPAddress ipDecoder;
IPAddress ipGateway;
IPAddress ipSubNet;

static byte decoderId = 252;
static byte subIpMosquitto = 254;
static byte subIpGateway = 254;
static byte decoderType = 3;
static boolean resetWifi = false;
static boolean debugFlag = true;                 // display debug messages

static boolean connectedMosquitto=true;

static String version = "2018-04-06";                     // software version
char ClientName[80];

static byte servoPin[2];                      // servo pin number
static byte servoPos[2][2]={
  {80,100},
  {70,110}
};
static boolean inverted[2];                      // if angleservoPos 0 > servoPos 1 than inverted = true
static byte relais[2];                       // relais pin number
static byte servoDelay[2];                          // controls speed servo movement, higher is slower
static byte addressSr[8];           // sensor addresses
static byte sensor[8];         // sensor pins with each a pull-up resistor
static byte output[8];                           // output pins
static byte turnoutNr;
static byte sensorNr;
static byte outputNr;
static boolean sensorStatus[8];                                     // status sensor pins
static boolean sensorStatusOld[8];                                  // old status sensor pins
static unsigned long sensorProcessTime[8];                          // sensor timer
static byte sensorCountOff[8];                                      // counter negative sensor values
static boolean sensorInverted[8];
static boolean scan = false;                                             // sensorvalue
static const byte scanDelay = 5;                                         // delay in sensor scan processing
static const byte scanNegativeNr = 15;                                   // number of negative scan values for negative sensorstatus
static byte control=0;
static long scanTimer=millis();
static byte scanPulse=300;                                          // duration of positive sensor message
static byte sensorScan=0;

static String(turnoutOrder) = "";                                        // used with debugging
static unsigned int relaisSwitchPoint[8];                           // relais switch start time
static unsigned int relaisSwitchDelay[8];                           // calculated relais switch time
static boolean turnoutInit[8];                                      // servo initiation flag
static byte currentPosition[8];                                     // servo position
static byte targetPosition[8];                                      // servo position to be reached
static unsigned long servoMoveTime[8];                              // servo timer
static byte turnoutId = 0;                                               // id turnout
static boolean turnoutMoving = false;

static byte servoId = 0;                                                 // id servo
static byte servoAngle = 0;                                              // angle servo
static byte servoAngleOld = 0;
static byte straightThrown = 0;
static boolean ackStraight = false;                                      // ack straight for servo adjust
static boolean ackThrown = false;                                        // ack thrown for servo adjust

static boolean buffer = false;
static byte bufferId = 0;
static boolean ordr = false;                                               // orders sent by rocrail
static boolean ordrOld = false;                                            // old orders to detect changes
static byte ordrId = 0;
static byte ordrIdOld = 0;
static String(ordroutput) = "";                                        // used with debugging
static byte outputId = 0;

static const String topicPub = "rocnet/sr";                                 // rocnet/rs for sensor
static const String topicSub1 = "rocnet/ot";                                // rocnet/ot for turnout control
static const String topicSub2 = "rocnet/sr";                                // rocnet/rs for sensor
static const int msgLength = 12;                                            // message number of bytes
static byte msgOut[msgLength];                                              // outgoing messages
static boolean sendMsg = false;                                             // set true to publish message
static boolean configFlag = true;                                          // control servoconfiguration
static boolean firstOrder = true;
static String text="";
static boolean printFlagOnce=false;
static boolean serialOnce = false;

struct {
  byte targetId = 0;
  boolean order = false;
  boolean orderOld = false;
  boolean orderNew = false;
} buf;

struct {
  byte targetId = 0;
  boolean in = false;
  boolean executed = true;
  boolean msgMove = false;
  boolean msgStop = false;
} order;

/////////////////////////////////////////////////////////////// adjust servo ///////////////////////
static char webMsg[7];
static boolean skip = false;
static byte webAngle = 0;
static byte webAngleOld = 0;
/////////////////////////////////////////////////////////////// start-up delay ///////////////////
static unsigned long startUpTimer = 0;
static int startUpDelay = 500;
static boolean startUp = false;
///////////////////////////////////////////////////////////////set-up//////////////////////////////
