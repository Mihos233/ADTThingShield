//ADT to SmartThings interface
//Mark Bowling
//Nov 2014


#include <SoftwareSerial.h>   //TODO need to set due to some weird wire language linker, should we absorb this whole library into smartthings
#include <SmartThings.h>


//*****************************************************************************
// Pin Definitions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
#define PIN_LED         13
#define PIN_THING_RX    3
#define PIN_THING_TX    2
#define PIR_MOTION_SENSOR 8
#define PIR_GDOOR_SENSOR 11
#define PIR_FDOOR_SENSOR 9
#define PIR_BDOOR_SENSOR 10

//*****************************************************************************
// Global Variables   | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  // constructor

bool isDebugEnabled;    // enable or disable debug in this example
int stateLED;           // state to track last set value of LED
int stateNetwork;       // state of the network 
    
//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 10;  

boolean lockLow = true;
boolean takeLowTime;

//stores state of last door poll
String fdoorStatus = "fdoorclosed";
String bdoorStatus = "bdoorclosed";
String gdoorStatus = "gdoorclosed";

void setNetworkStateLED()
{
  SmartThingsNetworkState_t tempState = smartthing.shieldGetLastNetworkState();
  if (tempState != stateNetwork)
  {
    switch (tempState)
    {
      case STATE_NO_NETWORK:
        if (isDebugEnabled) Serial.println("NO_NETWORK");
        smartthing.shieldSetLED(2, 0, 0); // red
        break;
      case STATE_JOINING:
        if (isDebugEnabled) Serial.println("JOINING");
        smartthing.shieldSetLED(2, 0, 0); // red
        break;
      case STATE_JOINED:
        if (isDebugEnabled) Serial.println("JOINED");
        smartthing.shieldSetLED(0, 0, 0); // off
        break;
      case STATE_JOINED_NOPARENT:
        if (isDebugEnabled) Serial.println("JOINED_NOPARENT");
        smartthing.shieldSetLED(2, 0, 2); // purple
        break;
      case STATE_LEAVING:
        if (isDebugEnabled) Serial.println("LEAVING");
        smartthing.shieldSetLED(2, 0, 0); // red
        break;
      default:
      case STATE_UNKNOWN:
        if (isDebugEnabled) Serial.println("UNKNOWN");
        smartthing.shieldSetLED(0, 2, 0); // green
        break;
    }
    stateNetwork = tempState; 
  }
}

//*****************************************************************************
// API Functions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                  V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
void setup()
{
  // setup default state of global variables
  isDebugEnabled = true;
  stateLED = 0;                 // matches state of hardware pin set below
  stateNetwork = STATE_JOINED;  // set to joined to keep state off if off
  
 
  // setup hardware pins 
  pinMode(PIR_MOTION_SENSOR, INPUT);
  digitalWrite(PIR_MOTION_SENSOR, LOW);
  pinMode(PIN_LED, OUTPUT);     // define PIN_LED as an output
  digitalWrite(PIN_LED, LOW);   // set value to LOW (off) to match stateLED=0
  
  pinMode(PIR_GDOOR_SENSOR, INPUT);
  pinMode(PIR_FDOOR_SENSOR, INPUT);
  pinMode(PIR_BDOOR_SENSOR, INPUT);


  if (isDebugEnabled)
  { // setup debug serial port
    Serial.begin(9600);         // setup serial with a baud rate of 9600
    Serial.println("setup..");  // print out 'setup..' on start
    Serial.println("SENSOR ACTIVE");

  }
}

// Intervals 
unsigned long   motionInterval = (60 * 1000);
unsigned long 	lastReportAt = 0; 
unsigned long   motionArmed  = 0;


void loop()
{
  // run smartthing logic
  smartthing.run();
  detectMotion();
  detectDoors();
  setNetworkStateLED();
}


void detectMotion() 
{
	if(digitalRead(PIR_MOTION_SENSOR) == HIGH)//if the sensor value is HIGH?
	{
         
         String motionMessage = "active"; 
         if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         smartthing.send(motionMessage);
         Serial.println("Motion Detected");
         delay(500);
         }         
         takeLowTime = true;
      
                
                
	}
	if (digitalRead(PIR_MOTION_SENSOR) == LOW)
	{
          
          String motionMessage = "inactive";
	  
        if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           smartthing.send(motionMessage);
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(500);
           }
	}
}

void detectDoors()
{
if(digitalRead(PIR_FDOOR_SENSOR) == HIGH)
    {
    if(fdoorStatus == "fdoorclosed")
	{
         String fdoorMessage = "fdooropen"; 
         smartthing.send(fdoorMessage);
         Serial.println("Front Door Open");
         delay(500);
         fdoorStatus = "fdooropen";
	}
    }
if (digitalRead(PIR_FDOOR_SENSOR) == LOW)
    {
    if(fdoorStatus == "fdooropen")
	{
          String fdoorMessage = "fdoorclosed";
          smartthing.send(fdoorMessage);
          Serial.println("Front Door Closed");
          delay(500);
          fdoorStatus = "fdoorclosed";
        }
    }   
	
if(digitalRead(PIR_BDOOR_SENSOR) == HIGH)
    {
    if(bdoorStatus == "bdoorclosed")
	{
         String bdoorMessage = "bdooropen"; 
         smartthing.send(bdoorMessage);
         Serial.println("Back Door Open");
         delay(500);
         bdoorStatus = "bdooropen";
	}
    }
if (digitalRead(PIR_BDOOR_SENSOR) == LOW)
  {
      if(bdoorStatus == "bdooropen")
	{
          String bdoorMessage = "bdoorclosed";
          smartthing.send(bdoorMessage);
          Serial.println("Back Door Closed");
          delay(500);
          bdoorStatus = "bdoorclosed";
        }   
  }
  
if(digitalRead(PIR_GDOOR_SENSOR) == HIGH)
  {
      if(gdoorStatus == "gdoorclosed")
	{
         String gdoorMessage = "gdooropen"; 
         smartthing.send(gdoorMessage);
         Serial.println("Garage Door Open");
         delay(500);
         gdoorStatus = "gdooropen";
	}
  }
if (digitalRead(PIR_GDOOR_SENSOR) == LOW)
  {
      if(gdoorStatus == "gdooropen")
	{
          String gdoorMessage = "gdoorclosed";
          smartthing.send(gdoorMessage);
          Serial.println("Garage Door Closed");
          delay(500);
          gdoorStatus = "gdoorclosed";
        }
  }
}

void messageCallout(String message)
{
  // if debug is enabled print out the received message
  if (isDebugEnabled)
  {
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("' ");
  }
   
  else if (message.equals("poll"))
  {
    detectDoors();
  }
  
  
}
