#define BUFLENTRIP 4
#define BUFLENSSID 26
#define BUFLENPWD 20
#define BUFLENSLIDER 5
#define BUFLENSRVSPD 3
#define BUFLENPULS 5
#define BUFLENDEBOUNCE 3
#define BUFLENSENSORHIGH 5

#define _GENERAL 20
#define _SSID 30
#define _PWD 20
#define _IP 10
#define _OUTPUT 10
#define _PULSLENGTH 20
#define _OUTPUTSENSOR 10

#define PIN_RESET 255                                // Clock
#define DC_JUMPER 0                                  // I2C Addres: 0 - 0x3C, 1 - 0x3D Clock

static boolean FLdebug = true;                       // display debug messages
static boolean FLshowHB = false;                     // display hart beat
static boolean FLshowOwnMessage = false;              // display own messages
static boolean FLshowBootAll = false;                 // show all EEPROM content during boot
static boolean FLshowClockMessage = false;
static String topicPub = "rocnet/sr";                // rocnet/rs for sensor
static String topicSub1 = "rocnet/ot";               // rocnet/ot for turnout control
static String topicSub2 = "rocnet/sr";               // rocnet/rs for sensor
static String topicSub3 = "eltraco/hb";              // eltraco/hb for Mosquitto hart beat signal
static String topicSub4 = "rocnet/ck";               // rocnet/ck for clock signal
static String topicIn = "";                          // topic incoming message;

static boolean startUpReady = false;


static String version = "2018-10-30";               // software version

static char ClientName[80];
static char APName[30];
static byte msgLengthOut = 0;                        // message number of bytes sent by actual message
static byte msgOut[16];                              // outgoing messages
static byte msgLengthIn = 0;                         // message number of bytes received by actual message
static byte msgIn[16];                               // incoming messages

static unsigned long HBSendTime = 0;
static unsigned long HBPeriod = 10000;

static byte txPower = 0;

static boolean FLinitial = true;
static boolean FLonce = false;
static boolean FLprint = false;                      // do not set manually
static boolean FLfirstOrder = true;
static byte flow = 0;
static byte decoderId = 1;
static byte turnoutNr = 0;
static byte sensorNr = 0;
static byte scanNegativeNr = 0;
static int sensorPositiveTime = 500;
static byte outputNr = 0;
static byte currentPosition[2];                      // servo position
static unsigned long waitTimer;

static int total = 0;
static byte servoMoveCount = 0;
static byte D0State = LOW;
static byte D0StateOld = LOW;

static byte decoderType = 0;
static byte decoderTypeOld = 0;                     // introduced to reboot on type change
enum decoderType {Single, Double, Sensor, Switch, Clock, None};

static byte reboot = 0;
enum reboot {WifiIp = 1, DecoderIp, DecoderType, NoWifi, NoMqtt};


struct Relais_t {
  byte Pin = 0;                                    // relais GPIO number
  byte SwitchPoint = 0;                            // middle of two servo angles
};
struct Relais_t relais[2];

struct Output_t {
  byte Pin = 0;                                    // output GPIO number
  boolean Order = false;                           // received output order
  boolean OrderOld = false;
  boolean Executed = false;
  boolean State = false;                           // ON - true / OFF - false
  boolean StateOld = false;                        // earlier state
  boolean Pulse = false;                           // output active during puls time
  int PulseDuration = 250;                         // duration of pulse
  byte SensorOn = 1;                               // sensor message on
  unsigned long Timer;
};
struct Output_t output[8];

struct Motor_t {
  byte Pin = 0;                                    // servo GPIO number
  byte Spd = 40;                                   // controls speed servo movement, higher is slower
  byte Delay = 5;
  byte AngleA = 90;                                // first angle
  byte AngleAOld = 0;
  byte AngleB = 90;                                // second angle
  byte AngleBOld = 0;
  boolean Swap = false;                            // posibility to swap angles after installation of turnout decoder
  unsigned long MoveTime;                          // servo timer
  boolean Invert;                                  // internal
};
struct Motor_t servo[2];

struct Sensor_t {
  byte Pin = 0;                                    // sensor GPIO number
  byte Address = 0;                                // sensor address
  boolean State = false;                           // ACTIVE - true / INACTIVE - false
  boolean StateOld = false;                        // earlier state
  boolean Invert = false;                          // invert value of "state" when Sensor Message is sent.
  volatile boolean Itr = false;                    // interrupt signal reacting on chages
  volatile unsigned long ItrTime = millis();       // time interrupt arrived
  byte CountOff = 0;                               // counter for negative scans
  byte ScanNegativeNr = 20;                        // default 10 for stable current sensor reading
  byte Debounce = 5;                               // time to wait in mSec before sending Sensor Message
  //  byte ProcessTime = 5;
};
struct Sensor_t sensor[8];

struct Buf_t {
  byte targetId = 0;
  boolean order = false;
  boolean orderOld = false;
  boolean orderNew = false;
};
struct Buf_t toBuffer;

struct Order_t {
  byte targetId = 0;
  boolean in = false;
  boolean executed = true;
  boolean msgMove = false;
  boolean msgStop = false;
};
struct Order_t toOrder;



static boolean FLwifiReset = true;
static boolean FLwifiResetOld = true;
static boolean FLwifiEstablished = false;
static boolean FLmqttEstablished = false;
static boolean FLwifiOK = false;
static boolean FLoledOnce = false;
static boolean FLupdateGeneral = false;
static boolean FLupdateSensor = false;
static boolean FLupdateOutput = false;
static boolean FLupdateSingle = false;
static boolean FLupdateDouble = false;
static boolean FLadjustServo1 = false;
static boolean FLadjustServo2 = false;
static boolean FLackAngleA1 = false;               // ack straight for servo adjust
static boolean FLackAngleB1 = false;               // ack thrown for servo adjust
static boolean FLackAngleA2 = false;               // ack straight for servo adjust
static boolean FLackAngleB2 = false;               // ack thrown for servo adjust
static boolean FLswapAngleA = false;               // invert servo angle
static boolean FLswapAngleB = false;               // invert servo angle
static boolean FLpolling = false;
static boolean FLreboot = false;
static boolean FLdetachServo = false;
static boolean FLservoMovePrint = false;
static boolean FLtakeA1 = false;
static boolean FLtakeB1 = false;
static boolean FLtakeA2 = false;
static boolean FLtakeB2 = false;

static char netName[BUFLENSSID];
static char passWord[BUFLENPWD];
static char IP[6][3];
static byte IPTriplet[6];
static char BTA[4];
static char ITA[9];

///////////////////////////////////////////// clock /////////////////////////////
static byte hours = 8; // Use these variables to set the initial time
static byte minutes = 5;
static byte seconds = 30;
static int CLOCK_SPEED = 1000;// Set this to 1000 to get _about_ 1 second timing
static int MIDDLE_Y; // Global variables to help draw the clock face:
static int MIDDLE_X;

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


