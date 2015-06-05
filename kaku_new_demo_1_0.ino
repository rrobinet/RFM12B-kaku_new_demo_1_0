
/* 2014-23-12  by  Rob
   This example sends commands to the KlikAanKlikUit units via OOK at 433 MHz 
   using the so called kaku NEW protocol with the RFM12B RF transmitter/receiver.
   It is inspired by the following sketches and references:
     1.  RFM12B kaku_demo.ino by <jc@wippler.nl> (see https://github.com/jcw/jeelib )
     2.  NewRemoteSitwch Library by Randy Simons (see http://randysimons.nl/26-433mhz-libraries-for-arduino )
     3.  Wieltje (see http://www.circuitsonline.net/forum/view/message/1181410 )
   See _SPECS Tab for more details.
*/

 
 #define DEBUG 1                        // 1, Used for Serial print debugging
 #define ENET_SHIELD 1                  // 1, configure RFM SPI slave select with Ethernet Shield
 
 /* Versioning */
 int MINOR = 0;                         // Minor version
 int MAJOR = 1;                         // Major version
 
 /* OOK default settings */
 byte rf12_cs = 10;			// Default RFM12B Slave Select pin                  
 unsigned long int _periodusec = 250;	// Default pulse period
 byte _repeats = 4;			// Default message repetition 
 byte _repDly = 10;                     // Repeat delay between successive repeats in ms (Typical 10)
 
 /* JeeNode server settings */
  #include <JeeLib.h>			// JeeNode library
 byte myJeeNode = 0;			// JeeNode node address
 byte myJeeGroup = 1;			// JeeNode group address   
 byte myJeeFreq = RF12_433MHZ;		// JeeNode RFM12B frequency
 
 /* Handset and Remote switch variables */
 unsigned long int hAddress;		// Handset address (from actual handset)
 byte sDevice;				// Remote switch address (from actual switch)
 boolean sGroup;			// Group option  (ON is true or OFF is false)
 byte sDimLevel;			// Dimmer Level  (from 1 to 4)
 boolean sOnOff;			// Remote switch level (ON is true, OFF is false)
    
/****************************************************** setup ******************************************************
* Function:  Sketch set-up
/******************************************************************************************************************/
 void setup()
 {
     if(DEBUG)
     {
       Serial.begin(115200);
       while (!Serial);               // Wait for serial port to connect. Needed for Leonardo only
     }
   
   if (ENET_SHIELD) rf12_set_cs(9);	// Use SPI slave select pin 9 (10 is already used by the Ethernet shield)

   rf12_initialize(myJeeNode, myJeeFreq,myJeeGroup);	// Initialise the RFM Board for KAKU OOK protocol (node ID = 0) 
   RemoteRfmTransmitter(250, 4, 10); 	// Set period to 250 and retries set to 4 wit a repat delay of 10 ms

     if(DEBUG) Serial.print ("KAKU New Demo Sketch settings Version: "),Serial.print (MAJOR), Serial.print ('.'), Serial.println (MINOR);
               Serial.print ("Period (us): "), Serial.print( _periodusec), Serial.print(" - Repeat: "), Serial.print (_repeats),
               Serial.print (" - Repeat Delay (ms): "), Serial.println (_repDly);
     
 }
/****************************************************** loop ******************************************************
* Function:  Demo Sketch Loop example
/******************************************************************************************************************/
  void loop() 
  { 
    /* Configue your KAKU command */
    hAddress = 1332798;			// Handset address, this value is to be cloned from an existing kaku handset (see _SPECS) 
    sDevice = 16;			// Remote switch address
    sOnOff = true;			// Remote switch is set to ON
    sDimLevel = 0;			// Remote switch dim level is set to 5
    sGroup = false;			// No group option 
    /* Execute the KAKU command */
    myNewKakuRfmSend(hAddress,  sDevice,  sOnOff, sGroup, sDimLevel); 
    delay (5000);
    
    /* Configue your KAKU command */    
    hAddress = 1332798;			// Handset address, this value is to be cloned from an existing kaku handset (see _SPECS) 
    sDevice = 16;			// Remote switch address
    sOnOff = false;			// Remote switch is set to OFF
    sDimLevel = 0;			// Remote switch dim level is set to 0
    sGroup = false;			// No group option
    /* Execute the KAKU command */    
    myNewKakuRfmSend(hAddress,  sDevice,  sOnOff, sGroup, sDimLevel);     
    delay (5000);
  }
/*********************************************** RemoteRfmTransmitter *********************************************
* Function:  Initialise the RFM Transmitter settings
* Parametres:  
*              symbol pulse period in us
*              datagram repeat count
*              datagram repeat delay
/******************************************************************************************************************/
 void RemoteRfmTransmitter(unsigned int periodusec, byte repeats, byte repDly)
 {
   _periodusec=periodusec;
   _repeats=repeats;
   _repDly = repDly;
 }
/************************************************ myNewKakuRfmSend *************************************************
* Function:  KAKU New send message command with RFM12B
* Parametres:
*              Handset address
*              Remote swich device address
*              Remote switch state level
*              Remote switch group command option
*              Remote switch dim level
/******************************************************************************************************************/
 void myNewKakuRfmSend(unsigned long int addr, unsigned long int device, boolean on, boolean group, byte dimLevel)
 {
   /* Form  the Command datagram:
   Device = (-1) From 0...15
   Address = From 0...﻿67.108.864 shift left 6 positions
   DimLevel = 1 to 15
   */
   if(device !=0) device--;                             // Set Device address to logical
   if (group) device = 0x20 ;				// Ensure that Device is be set to 0 in case of group
   unsigned long int cmd = 0 | addr << 6 | device;	// Build the command datagram with the address and device info
   if (on) cmd |= 0x10;                                 // Set the Level bit to ON in the command datagram
   else  dimLevel = 0;			                // Avoid DIM to be active while setting the level to OFF
      if(DEBUG)
      {
        Serial.print ("Using Handset: "), Serial.print(hAddress),  Serial.print (" Set Logical Device: "),
        Serial.print (device & 0x0F), Serial.print (" to: "), Serial.print(on), 
        Serial.print(" Group: "), Serial.print (group); 
        if (dimLevel)
        {
          Serial.print(" with DIM level: "), Serial.println (dimLevel);
        }
        else Serial.println();
      }
  for (int i = 0; i< _repeats; i++)
  {
    ookNewKakuRfmPulse(0, _periodusec*10);						// Send Start  bit
    for (byte bit = 0; bit <27  ; bit++)						// Send Address and Group bits MSB first
    {
      unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec;	// bit 1: 5,1: bit 0: 1,5
      ookNewKakuRfmPulse(level, _periodusec*6-level);
    }
    if (dimLevel)				                                        // Case Dim Level option
    {
      ookNewKakuRfmPulse(_periodusec, _periodusec);					// Send Dimmer pulse
      for (byte bit = 28; bit <32  ; bit++)						// Send Device MSB bits first
      {
        unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec;	// bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
      for (byte bit = 0; bit < 4  ; bit++)						// Send Dimmer Level MSB bits first
      {
        unsigned long int level = bitRead(dimLevel,3-bit) ? _periodusec*5 : _periodusec;// bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
    }
    else										// Case ON / OFF option
    {
      for (byte bit = 27; bit <32  ; bit++)						// Send Level and Device MSB bits first
      {
        unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec;	// bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
     }
     ookNewKakuRfmPulse(0, _periodusec*10);						// Stop bit
     delay (_repDly);                                                                   // wait some delay between reties 
  }                                                                  
 }
/*************************************************** ookNewKakuRfmPulse ************************************************
* Function:  KAKU New pulse switching function
* Parametres:
*              duration of the symbol levels
/***********************************************************************************************************************/
static void ookNewKakuRfmPulse(int l1, int l2)
{
 if (l1 != 0)				// Skip for Start/Stop SYNC
 {
  rf12_onOff(HIGH);			// Send SPI command to the RFM module
  delayMicroseconds (_periodusec+150);	// Increase the delay for the SPI transfer                     
  rf12_onOff(LOW);			// Send SPI command to the RFM module
  delayMicroseconds (l1-200);		// Decrease the duration to cope with the SPI delay
 }
  rf12_onOff(HIGH);           
  delayMicroseconds (_periodusec+150);  
  rf12_onOff(LOW);
  delayMicroseconds (l2-200);
}
/***********************************************************************************************************************/

