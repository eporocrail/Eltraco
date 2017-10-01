ELTRACO (Ellard's track control system) 

constitutes a wireless model rail-road track control system to be used with “ROCRAIL™”.

It consists of hardware and software.

The hardware components are:
Wifi router
Raspberry Pi
Servo tool
Smart Turnout Decoder
Sensor Decoder
LED Decoder
Hall sensor holder.
Current detection Sensor
Servo motor
Servo motor bracket

The software components are:
Raspbian
Rocrail
Mosquitto
Servo tool software
Smart Turnout decoder software
Sensor decoder software
LED decoder software

Functions performed:
Wireless control of turnouts:
the decoder moves the turnout
signals this movement by means of a software alert
signals occupation by means of current detection
Wireless occupation detection:
the decoder advertises track occupation by means of
hall sensor and/or by means of current detection sensor
Wireless on/off switching of LEDs

OVERVIEW

The router creates a closed WiFi network. Rocrail and Mosquitto are installed on a Raspi.
Rocrail is the model rail road controlling software. It communicates wireless via the “MQTT”(Message Queue Telemetry Transport) protocol with the Eltraco decoders. Mosquitto is the “MQTT – post office”.

Each decoder has its dedicated software package. After the initial installation, updates can be performed “OTA” (Over The Air). After installation of a servo underneath the baseboard of the layout it needs adjustment of two angles, one for the straight position and one for the thrown position. This adjustment is done with the wireless servo tool.

Each turnout decoder controls two turnouts. 
Each sensor decoder accommodates eight digital and one analogue sensor.
With each LED decoder eight individual LEDs can be switched on and off wireless by Rocrail.

MORE INFORMATION

Information on how to make the hardware and how to use the system with Rocrail is available on my user pages of the Rocrail Website.

Ellard Postma
September 2017
