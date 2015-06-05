Change font size
 /*
  KAKU NEW SPECIFICATIONS
  ***********************
  Datagram Symbols
  ----------------  
   Float  
    __        ______
   |  |      |      | 
   |  |______|      |__
    T    3T     3T   T  
 
  bit 0
    __    __
   |  |  |  | 
   |  |__|  |__________
    T  T  T     5T
 
  bit 1
    __            __
   |  |          |  | 
   |  |__________|  |__
     T     5T     T  T
  
  SYNC/Start
    __
   |  | 
   |  |____________________
    T     10T 
 
 From the tests the following values are workable
  200us <=T<= 300us Typical 250us
  
﻿﻿  Datagram Structure
  ------------------
  ON/OFF
  ====== 
  Bit order    	  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
  Weight       	  1  2  4  8        1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2
  Symboles  Stop  D  D  D  D  L  G  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  Start
                !     0     !  !  !   0 (LSB)  !     1     !     2     !     3     !     4     !     5    !6(MSB)! 
                
  DIM
  === 
  Bit order       35 34 33 32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
  Weight          1  2  4  8  1  2  4  8        1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2  4  8  1  2
  Symboles  Stop  l  l  l  l  D  D  D  D  d  G  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  Start
                !     0     !     0     !  !  !  0(LSB)    !     1     !     2     !     3     !     4     !    5     !6(MSB)! 

  
  S = SYNC/Start/Stop bit
  L/d = Level (or remote switch state) bit   L: ON = 1, OFF = 0 ; 'L' symbol is replaced by 'd' symbol if a Dim level is specified
  D = Device (or remote switch unit) bits    0...15; Practical handset values are 1...16 
  A = Address (or handset) bits              0...(2^26)67.108.863; Practical value of handset addresses are defined by Manufacturer
  G = Group bit				     Group = 1
  l = Dim Level for Dim option 		     1...15;  0 means no Dim
  
  NOTES: 
    1. MSB bits have to be send first
    2. The sketch doesn't allow GROUP and DIM together (is it useful?)
    3. One Datagram should be repeated (at least 4 times)
    4. Delay between repeated datagrams is typically 10ms
  
  USAGE
  *****
  This sketch is tested with an RFM12B 433MHz module on a JeeNode V6 and with Arduino Uno/ATMEGA2560 with and without Ethernet Shield
  It is also tested with Arduino Ethernet and Leonardo (but without Ethernet Shield, no access to the ISCP connector!).
  While using Arduino together with an Ethernet Shield, the RFMB12B SPI SELECT should be connected to another pin than the default pin 10;
  In the sketch, pin 9 is used, selection is done via the “define ENET_SHIELD” statement.
  The Handset address (26 bits) to use is a clone of an exiting one (with power switch appliances programmed), one of mine (1332798 is used as
  example. This address can be discovered using the Arduino + RX433 or XY—MK-5V module and the NewRemoteSwitch/examples/ShowReceivedCode.ino 
  sketch (see https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home ).
  
  Notes: 
    1. The JeeNode ID should be 0
    2. DIM function only works with dimmable remote switches
    3. DIM option with this sketch is only active while setting level to "ON" 
  
  DEVICES
  *******
   The new Kaku devices used for the test are:
   CHACON Handset       Type: LYCT-505 
   CHACON Remote switch Type: LYCR-3500 (no-dimmable)
   CHACON Remote switch Type: LYCR-300 (dimmable)
   
  TRACING
  *******
  Tracing is done using AUDICITY
  see http://harizanov.com/2012/04/controlling-433mhz-rf-power-sockets-with-a-rfm12b-module/
 
  CONFIGURATION PINOUT
  ********************
        UNO  ENET 2560  Leonardo(ICSP)
  MISO  12   12   50    1                         
  SCK   13   13   52    3
  MOSI  11   11   51    4                        
  GND  GND   GND  GND   6
  3v3  3v3   3V3  3V3   3v3
  IRQ  2     2    2     2       
  SEL  9/10* 10   53    9
 */