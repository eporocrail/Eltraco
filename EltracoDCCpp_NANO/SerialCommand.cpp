/**********************************************************************

  SerialCommand.cpp
  COPYRIGHT (c) 2013-2016 Gregg E. Berman

  Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

// DCC++ BASE STATION COMMUNICATES VIA THE SERIAL PORT USING SINGLE-CHARACTER TEXT COMMANDS
// WITH OPTIONAL PARAMTERS, AND BRACKETED BY < AND > SYMBOLS.  SPACES BETWEEN PARAMETERS
// ARE REQUIRED.  SPACES ANYWHERE ELSE ARE IGNORED.  A SPACE BETWEEN THE SINGLE-CHARACTER
// COMMAND AND THE FIRST PARAMETER IS ALSO NOT REQUIRED.

// See SerialCommand::parse() below for defined text commands.

#include "SerialCommand.h"
#include "DCCpp_Uno.h"

extern int __heap_start, *__brkval;

///////////////////////////////////////////////////////////////////////////////

char SerialCommand::commandString[MAX_COMMAND_LENGTH + 1];
volatile RegisterList *SerialCommand::mRegs;
volatile RegisterList *SerialCommand::pRegs;
CurrentMonitor *SerialCommand::mMonitor;

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::init(volatile RegisterList *_mRegs, volatile RegisterList *_pRegs, CurrentMonitor *_mMonitor) {
  mRegs = _mRegs;
  pRegs = _pRegs;
  mMonitor = _mMonitor;
  sprintf(commandString, "");
} // SerialCommand:SerialCommand

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::process() {
  char c;

#if COMM_TYPE == 0

  while (INTERFACE.available() > 0) { // while there is data on the serial line
    c = INTERFACE.read();
    if (c == '<')                 // start of new command
      sprintf(commandString, "");
    else if (c == '>')            // end of new command
      parse(commandString);
    else if (strlen(commandString) < MAX_COMMAND_LENGTH) // if comandString still has space, append character just read from serial line
      sprintf(commandString, "%s%c", commandString, c);  // otherwise, character is ignored (but continue to look for '<' or '>')
  } // while

#endif

} // SerialCommand:process

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::parse(char *com) {

  switch (com[0]) {

    /***** SET ENGINE THROTTLES USING 128-STEP SPEED CONTROL ****/

    case 't':       // <t REGISTER CAB SPEED DIRECTION>
      /*
            sets the throttle for a given register/cab combination

            REGISTER: an internal register number, from 1 through MAX_MAIN_REGISTERS (inclusive), to store the DCC packet used to control this throttle setting
            CAB:  the short (1-127) or long (128-10293) address of the engine decoder
            SPEED: throttle speed from 0-126, or -1 for emergency stop (resets SPEED to 0)
            DIRECTION: 1=forward, 0=reverse.  Setting direction when speed=0 or speed=-1 only effects directionality of cab lighting for a stopped train

            returns: <T REGISTER SPEED DIRECTION>

      */
      mRegs->setThrottle(com + 1);
      break;

    /***** OPERATE ENGINE DECODER FUNCTIONS F0-F28 ****/

    case 'f':       // <f CAB BYTE1 [BYTE2]>
      /*
            turns on and off engine decoder functions F0-F28 (F0 is sometimes called FL)
            NOTE: setting requests transmitted directly to mobile engine decoder --- current state of engine functions is not stored by this program

            CAB:  the short (1-127) or long (128-10293) address of the engine decoder

            To set functions F0-F4 on (=1) or off (=0):

            BYTE1:  128 + F1*1 + F2*2 + F3*4 + F4*8 + F0*16
            BYTE2:  omitted

            To set functions F5-F8 on (=1) or off (=0):

            BYTE1:  176 + F5*1 + F6*2 + F7*4 + F8*8
            BYTE2:  omitted

            To set functions F9-F12 on (=1) or off (=0):

            BYTE1:  160 + F9*1 +F10*2 + F11*4 + F12*8
            BYTE2:  omitted

            To set functions F13-F20 on (=1) or off (=0):

            BYTE1: 222
            BYTE2: F13*1 + F14*2 + F15*4 + F16*8 + F17*16 + F18*32 + F19*64 + F20*128

            To set functions F21-F28 on (=1) of off (=0):

            BYTE1: 223
            BYTE2: F21*1 + F22*2 + F23*4 + F24*8 + F25*16 + F26*32 + F27*64 + F28*128

            returns: NONE

      */
      mRegs->setFunction(com + 1);
      break;



    /***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/

    case 'w':      // <w CAB CV VALUE>
      /*
            writes, without any verification, a Configuration Variable to the decoder of an engine on the main operations track

            CAB:  the short (1-127) or long (128-10293) address of the engine decoder
            CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
            VALUE: the value to be written to the Configuration Variable memory location (0-255)

            returns: NONE
      */
      mRegs->writeCVByteMain(com + 1);
      break;

    /***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/

    case 'b':      // <b CAB CV BIT VALUE>
      /*
            writes, without any verification, a single bit within a Configuration Variable to the decoder of an engine on the main operations track

            CAB:  the short (1-127) or long (128-10293) address of the engine decoder
            CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
            BIT: the bit number of the Configurarion Variable regsiter to write (0-7)
            VALUE: the value of the bit to be written (0-1)

            returns: NONE
      */
      mRegs->writeCVBitMain(com + 1);
      break;

    /***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'W':      // <W CV VALUE CALLBACKNUM CALLBACKSUB>
      /*
            writes, and then verifies, a Configuration Variable to the decoder of an engine on the programming track

            CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
            VALUE: the value to be written to the Configuration Variable memory location (0-255)
            CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
            CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function

            returns: <r CALLBACKNUM|CALLBACKSUB|CV Value)
            where VALUE is a number from 0-255 as read from the requested CV, or -1 if verificaiton read fails
      */
      pRegs->writeCVByte(com + 1);
      break;

    /***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'B':      // <B CV BIT VALUE CALLBACKNUM CALLBACKSUB>
      /*
            writes, and then verifies, a single bit within a Configuration Variable to the decoder of an engine on the programming track

            CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
            BIT: the bit number of the Configurarion Variable memory location to write (0-7)
            VALUE: the value of the bit to be written (0-1)
            CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
            CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function

            returns: <r CALLBACKNUM|CALLBACKSUB|CV BIT VALUE)
            where VALUE is a number from 0-1 as read from the requested CV bit, or -1 if verificaiton read fails
      */
      pRegs->writeCVBit(com + 1);
      break;

    /***** READ CONFIGURATION VARIABLE BYTE FROM ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'R':     // <R CV CALLBACKNUM CALLBACKSUB>
      /*
            reads a Configuration Variable from the decoder of an engine on the programming track

            CV: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
            CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
            CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function

            returns: <r CALLBACKNUM|CALLBACKSUB|CV VALUE)
            where VALUE is a number from 0-255 as read from the requested CV, or -1 if read could not be verified
      */
      pRegs->readCV(com + 1);
      break;

    /***** TURN ON POWER FROM MOTOR SHIELD TO TRACKS  ****/

    case '1':      // <1>
      /*
            enables power from the motor shield to the main operations and programming tracks

            returns: <p1>
      */
      digitalWrite(SIGNAL_ENABLE_PIN_PROG, HIGH);
      digitalWrite(SIGNAL_ENABLE_PIN_MAIN, HIGH);
      INTERFACE.print("<p1>");
      break;

    /***** TURN OFF POWER FROM MOTOR SHIELD TO TRACKS  ****/

    case '0':     // <0>
      /*
            disables power from the motor shield to the main operations and programming tracks

            returns: <p0>
      */
      digitalWrite(SIGNAL_ENABLE_PIN_PROG, LOW);
      digitalWrite(SIGNAL_ENABLE_PIN_MAIN, LOW);
      INTERFACE.print("<p0>");
      break;

    /***** READ MAIN OPERATIONS TRACK CURRENT  ****/

    case 'c':     // <c>
      /*
            reads current being drawn on main operations track

            returns: <a CURRENT>
            where CURRENT = 0-1024, based on exponentially-smoothed weighting scheme
      */
      INTERFACE.print("<a");
      INTERFACE.print(int(mMonitor->current));
      INTERFACE.print(">");
      break;

    /***** READ STATUS OF DCC++ BASE STATION  ****/

    case 's':      // <s>
      /*
            returns status messages containing track power status, throttle status, turn-out status, and a version number
            NOTE: this is very useful as a first command for an interface to send to this sketch in order to verify connectivity and update any GUI
            to reflect actual throttle and turn-out settings

            returns: series of status messages that can be read by an interface to determine status of DCC++ Base Station and important settings
      */
      if (digitalRead(SIGNAL_ENABLE_PIN_PROG) == LOW)   // could check either PROG or MAIN
        INTERFACE.print("<p0>");
      else
        INTERFACE.print("<p1>");

      for (int i = 1; i <= MAX_MAIN_REGISTERS; i++) {
        if (mRegs->speedTable[i] == 0)
          continue;
        INTERFACE.print("<T");
        INTERFACE.print(i); INTERFACE.print(" ");
        if (mRegs->speedTable[i] > 0) {
          INTERFACE.print(mRegs->speedTable[i]);
          INTERFACE.print(" 1>");
        } else {
          INTERFACE.print(-mRegs->speedTable[i]);
          INTERFACE.print(" 0>");
        }
      }
      INTERFACE.print("<iDCC++ BASE STATION FOR ARDUINO ");
      INTERFACE.print(ARDUINO_TYPE);
      INTERFACE.print(" / ");
      INTERFACE.print(MOTOR_SHIELD_NAME);
      INTERFACE.print(": V-");
      INTERFACE.print(VERSION);
      INTERFACE.print(" / ");
      INTERFACE.print(__DATE__);
      INTERFACE.print(" ");
      INTERFACE.print(__TIME__);
      INTERFACE.print(">");

      INTERFACE.print("<N");
      INTERFACE.print(COMM_TYPE);
      INTERFACE.print(": ");

#if COMM_TYPE == 0
      INTERFACE.print(" SERIAL via USB >");
#endif

      break;


    /***** PRINT CARRIAGE RETURN IN SERIAL MONITOR WINDOW  ****/

    case ' ':     // < >
      /*
            simply prints a carriage return - useful when interacting with Ardiuno through serial monitor window

            returns: a carriage return
      */
      INTERFACE.println("");
      break;

    ///
    /// THE FOLLOWING COMMANDS ARE NOT NEEDED FOR NORMAL OPERATIONS AND ARE ONLY USED FOR TESTING AND DEBUGGING PURPOSES
    /// PLEASE SEE SPECIFIC WARNINGS IN EACH COMMAND BELOW
    ///

    /***** ENTER DIAGNOSTIC MODE  ****/

    case 'D':       // <D>
      /*
            changes the clock speed of the chip and the pre-scaler for the timers so that you can visually see the DCC signals flickering with an LED
            SERIAL COMMUNICAITON WILL BE INTERUPTED ONCE THIS COMMAND IS ISSUED - MUST RESET BOARD OR RE-OPEN SERIAL WINDOW TO RE-ESTABLISH COMMS
      */

      Serial.println("\nEntering Diagnostic Mode...");
      delay(1000);

      bitClear(TCCR1B, CS12);   // set Timer 1 prescale=8 - SLOWS NORMAL SPEED BY FACTOR OF 8
      bitSet(TCCR1B, CS11);
      bitClear(TCCR1B, CS10);


#ifdef ARDUINO_AVR_NANO      // Configuration for NANO
      bitSet(TCCR0B, CS02);   // set Timer 0 prescale=256 - SLOWS NORMAL SPEED BY A FACTOR OF 4
      bitClear(TCCR0B, CS01);
      bitClear(TCCR0B, CS00);

#endif

      CLKPR = 0x80;         // THIS SLOWS DOWN SYSYEM CLOCK BY FACTOR OF 256
      CLKPR = 0x08;         // BOARD MUST BE RESET TO RESUME NORMAL OPERATIONS
      break;

  }
}
///////////////////////////////////////////////////////////////////////////////


