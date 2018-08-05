 

//LCD -----------------------------------------------------
/* 
  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
#include <LiquidCrystal.h>
#include <Wire.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int lcdPowerPin=6;
//-----------------------------------------------------------

//Clock -----------------------------------------------------
#include "DS3231.h"
DS3231 clock;
RTCDateTime dt;
//-----------------------------------------------------------


//Joystick -----------------------------------------------------
#define JOYSTICK_NONE 0
#define JOYSTICK_LEFT 1
#define JOYSTICK_RIGHT 2
#define JOYSTICK_UP 3
#define JOYSTICK_DOWN 4
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
const int joystickDeadZone=100;
int joystickX = 0;
int joystickY = 0;
int joystickButton = 0;
int joystickDirection = 0;
//-----------------------------------------------------------

//74HC595 -----------------------------------------------------
int tDelay = 1000;
int latchPin = 4;      // (4) ST_CP [RCK] on 74HC595
int clockPin = 3;      // (3) SH_CP [SCK] on 74HC595
int dataPin = 5;     // (5) DS [S1] on 74HC595
byte leds = 0;
//-----------------------------------------------------------

//DHT11 -----------------------------------------------------
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 13
#include "SimpleDHT.h"
int pinDHT11 = 13;
SimpleDHT11 dht11;
byte temperature = 0;
byte humidity = 0;
byte dataDHT11[40] = {0};
//-----------------------------------------------------------


//Serial Input  -------------------------------------------
String inputs;
//-----------------------------------------------------------


//State -----------------------------------------------------
char ON_LOGO=126;
char OFF_LOGO='x';
const char * systemName =    { "AmmBus Waterchip" };
const char * systemVersion = { "     v0.30      " };
const char * valveLabels[] =
{
    "Mikri Skala    ",
    "Leyland/Porta  " ,
    "Elato/Garage   ",
    "Triantafylla  ",
    "Agalma/Lemonia",
    "Gazon A      ",
    "Gazon B      ",
    "Gazon C      ", 
    //-----------------
    "Unknown"
};

const char * valveSpeeds[] =
{
    "Normal",
    "Extra" ,
    "Fast", 
    //-----------------
    "Unknown"
};

const byte numberOfMenus=12;
byte currentMenu=0;

byte valvesTimesNormal[8]={30,30,30,30,30,30,30,30};
byte valvesTimesHigh[8]={60,60,60,60,60,60,60,60};
byte valvesTimesLow[8]={15,15,15,15,15,15,15,15};
byte *armedTimes = 0;
byte *valvesTimes = valvesTimesNormal;

byte valvesState[8]={0};
uint32_t valveStartTimestamp[8]={0};

byte errorDetected = 0;
byte idleTicks=10000;

byte powerSaving=1;
byte autopilotOn=0;
byte jobConcurrency=1; //Max concurrent jobs
//-----------------------------------------------------------


void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void setRelayState( byte * valves )
{
  leds=0; 
  int i=0;
  for (i=0; i<8; i++) 
  {
    if (!valves[i] )  { bitSet(leds,i); }
  }
  updateShiftRegister();  
}


#define ON 1
#define OFF 0
void checkForSerialInput()
{
    while(Serial.available()) //Check if there are available bytes to read
    {
        delay(10); //Delay to make it stable
        char c = Serial.read(); //Conduct a serial read
        if (c == '#')
        {
            break; //Stop the loop once # is detected after a word
        }
        inputs += c; //Means inputs = inputs + c
    }
    if (inputs.length() >0)
    {
        Serial.println(inputs);


        if(inputs == "*")
        {
            valvesState[0]=ON;
            valvesState[1]=ON;
            valvesState[2]=ON;
            valvesState[3]=ON;
            valvesState[4]=ON;
            valvesState[5]=ON;
            valvesState[6]=ON;
            valvesState[7]=ON;
        }
        else if(inputs == "$")
        {
            valvesState[0]=OFF;
            valvesState[1]=OFF;
            valvesState[2]=OFF;
            valvesState[3]=OFF;
            valvesState[4]=OFF;
            valvesState[5]=OFF;
            valvesState[6]=OFF;
            valvesState[7]=OFF;
        }
        else if(inputs == "A")
        {
            valvesState[0]=ON;
        }
        else if(inputs == "a")
        {
            valvesState[0]=OFF;
        }
        else if(inputs == "B")
        {
            valvesState[1]=ON;
        }
        else if(inputs == "b")
        {
            valvesState[1]=OFF;
        }
        else if(inputs == "C")
        {
            valvesState[2]=ON;
        }
        else if(inputs == "c")
        {
            valvesState[2]=OFF;
        }
        else if(inputs == "D")
        {
            valvesState[3]=ON;
        }
        else if(inputs == "d")
        {
            valvesState[3]=OFF;
        }
        else if(inputs == "E")
        {
            valvesState[4]=ON;
        }
        else if(inputs == "e")
        {
            valvesState[4]=OFF;
        }
        else if(inputs == "F")
        {
            valvesState[5]=ON;
        }
        else if(inputs == "f")
        {
            valvesState[5]=OFF;
        }
        else if(inputs == "G")
        {
            valvesState[6]=ON;
        }
        else if(inputs == "g")
        {
            valvesState[6]=OFF;
        }
        else if(inputs == "H")
        {
            valvesState[7]=ON;
        }
        else if(inputs == "h")
        {
            valvesState[7]=OFF;
        }
        inputs="";

        setRelayState(valvesState);
    }


}





void turnLCDOn()
{
  if (powerSaving)
  {
   digitalWrite(lcdPowerPin, HIGH);
   delay(100);
   // set up the LCD's number of columns and rows:
   lcd.begin(16, 2);
   // Print a message to the LCD.
   lcd.print(systemName);
   lcd.setCursor(0, 1);
   lcd.print("powering up..");
   powerSaving=0;
  }
}

void turnLCDOff()
{
  if (!powerSaving)
  { 
    
   lcd.setCursor(0, 0);
   lcd.print(systemName);
   lcd.setCursor(0, 1);
   lcd.print("powersaving ..  ");   
   lcd.noDisplay();  
   
   
   
   digitalWrite(7, LOW); 
   digitalWrite(8, LOW); 
   digitalWrite(9, LOW); 
   digitalWrite(10, LOW);  
   digitalWrite(11, LOW);  
   digitalWrite(12, LOW);  
    


   delay(100);
   digitalWrite(lcdPowerPin, LOW); 
   powerSaving=1;
  }
}

void setup() 
{  
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);   
  setRelayState(valvesState);
  
  pinMode(lcdPowerPin, OUTPUT);
  digitalWrite(lcdPowerPin, LOW); 
  
  turnLCDOn();
  
  
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
  
  
  clock.begin();

  // Set sketch compiling time
  clock.setDateTime(__DATE__, __TIME__);
  dt = clock.getDateTime();

  // Set from UNIX timestamp
  // clock.setDateTime(1397408400);

  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
}





void grabMeasurements()
{  
 //DHT Reading 
 if (dht11.read(pinDHT11, &temperature, &humidity, dataDHT11))  { errorDetected=1; }  
 
 //Joystick Reading
 joystickX=analogRead(X_pin);
 joystickY=analogRead(Y_pin);
 joystickButton = digitalRead(SW_pin);
 if (joystickButton) { joystickButton=0; } else
                     { joystickButton=1; }
  
 if (joystickX<512-joystickDeadZone)  {  joystickDirection=JOYSTICK_LEFT;  } else
 if (joystickX>512+joystickDeadZone)  {  joystickDirection=JOYSTICK_RIGHT; } else
 if (joystickY<512-joystickDeadZone)  {  joystickDirection=JOYSTICK_UP;    } else
 if (joystickY>512+joystickDeadZone)  {  joystickDirection=JOYSTICK_DOWN;  } else
                                      {  joystickDirection=JOYSTICK_NONE;  } 
 
 
 //Get Time
 dt = clock.getDateTime();
}


void idleMessageTicker(int seconds)
{
  #define MESSAGES 5
  int messageTicker = (seconds/2) % MESSAGES;
  lcd.setCursor(0,0);
  switch (messageTicker)
  {
    case 0 : lcd.print(systemName);  
             lcd.setCursor(0, 1); 
             lcd.print(systemVersion);
    break; 
    case 1 : lcd.print(systemName);  
             lcd.setCursor(0, 1); 
             lcd.print("No Job Scheduled");
    break; 
    case 2 :  
             lcd.print("Temp / Humidity ");
             lcd.setCursor(0, 1);  
             lcd.print("  ");
             lcd.print((int)temperature); lcd.print(" *C, ");
             lcd.print((int)humidity); lcd.print(" %   ");  
    break; 
    case 3 : 
             lcd.print(clock.dateFormat(" d F Y   ",  dt));
             lcd.setCursor(0, 1);
             lcd.print(clock.dateFormat("    H:i:s   ",  dt));
    break; 
    case 4 : 
             lcd.print(systemName);  
             lcd.setCursor(0, 1); 
             lcd.print("Uptime : ");
             lcd.print((unsigned int)millis() / 1000);  
             lcd.print("  mins");
    break; 
    //REMEMBER TO UPDATE numberOfMenus
  }
}





void joystickMenuHandler()
{
  
  switch (joystickDirection)
  {
    case JOYSTICK_NONE : break;
    //-------------------------
    case JOYSTICK_RIGHT : 
     if (currentMenu==numberOfMenus-1) { currentMenu=0; } else
                                       { ++currentMenu; }     
    idleTicks=0;
    break;
    //-------------------------    
    case JOYSTICK_LEFT : 
     if (currentMenu==0) { currentMenu=numberOfMenus-1; } else
                         { --currentMenu; }
    idleTicks=0;
    break;
    //-------------------------    
    case JOYSTICK_UP : 
    
    idleTicks=0;
    break;
    //-------------------------    
    case JOYSTICK_DOWN : 
    
    idleTicks=0;
    break;
    //-------------------------
  }
   
   
 if ( (idleTicks==0) && (joystickDirection!=JOYSTICK_NONE))
  {
   turnLCDOn();
  } 
}


void valveAutopilot()
{
  unsigned int i=0;
  unsigned int valvesRunning=0;
  
  for (i=0; i<8; i++)
  {
    if (valvesState[i]) {++valvesRunning;} 
  }
  
  //Should a job start..?
  if (valvesRunning==0)
  {
  }
  
  for (i=0; i<8; i++)
  {
    if (valvesState[i])
    {
      //This valve is running, should it stop?
     // valveStartTimestamp[i]
    }
  }
  
}



void joystickTimeHandler(int valve)
{
  
  switch (joystickDirection)
  {
    case JOYSTICK_NONE : break;  
    case JOYSTICK_UP : 
     valvesTimes[valve]+=5;
     idleTicks=0;
    break;
    //-------------------------    
    case JOYSTICK_DOWN : 
     valvesTimes[valve]-=5; 
     idleTicks=0;
    break;
    //-------------------------
  }
   
   
  if (joystickButton) 
  {
   if ( valvesState[valve] ) { valvesState[valve]=0; } else
                             { 
                               valvesState[valve]=1;   
                               valveStartTimestamp[valve]=dt.unixtime;
                             } 
   setRelayState(valvesState);
  } 
}

void menuDisplay(int menuOption)
{
  byte valveNum = 0;
  if (menuOption>=3) { valveNum=menuOption-3; }
  byte valveHumanNum = valveNum+1;
  
  lcd.setCursor(0,0);
  
  byte *selectedSpeeds = 0;             
  switch (menuOption)
  {
    case 0 :
    case 1 :
    case 2 :
    lcd.print(systemName);  
             if (menuOption==0) { selectedSpeeds = valvesTimesNormal;  } else
             if (menuOption==1) { selectedSpeeds = valvesTimesHigh;    } else
             if (menuOption==2) { selectedSpeeds = valvesTimesLow;     }  
             
             lcd.setCursor(0, 1); 
             if (armedTimes==selectedSpeeds) 
                  { 
                    if (autopilotOn)
                     {
                      lcd.print(" Running "); 
                     } else
                     {
                      lcd.print(" Start "); 
                     }
                  } else
                  { 
                    if (autopilotOn)
                     {
                      lcd.print(" SwitchTo "); 
                     } else
                     { 
                      lcd.print("   Arm ");   
                     }
                  }
             lcd.print(valveSpeeds[menuOption]);
             lcd.print("   ");
             
              
             
             if (joystickButton)
               { 
                 if (armedTimes==selectedSpeeds) 
                  {
                   valvesTimes=selectedSpeeds; 
                   //Do start 
                   autopilotOn=1;
                  } else
                  {
                   valvesTimes=selectedSpeeds; 
                   armedTimes=selectedSpeeds;
                  }
               }
    break;   
    break;  
    //------------------------------------ 
    case 3 : 
    case 4 : 
    case 5 : 
    case 6 : 
    case 7 : 
    case 8 : 
    case 9 : 
    case 10: 
             lcd.print("V");                          
             lcd.print((int) (valveHumanNum));
             lcd.print(" ");
             lcd.print(valveLabels[valveNum]);
             lcd.setCursor(0, 1); 
             if (valvesState[valveNum]) { lcd.print((char)ON_LOGO);  } else  
                                        { lcd.print((char)OFF_LOGO); }
             lcd.print("  Time: ");
             lcd.print((int) (valvesTimes[valveNum]));
             lcd.print("min  ");
             joystickTimeHandler(valveNum);
    break;   
    case 11 :
             lcd.print(systemName);  
             lcd.setCursor(0, 1); 
             lcd.print("    Settings    ");
    break;
  } 
}


 
void loop() 
{
  grabMeasurements(); 
  checkForSerialInput();
  int seconds = millis() / 1000; 
  
  joystickMenuHandler();
  
  if ( (idleTicks>120) || (powerSaving) )
  {
    //Switch monitor off 
    turnLCDOff();
  } else
  if (idleTicks>60)
  {
    //Display ScreenSaver logos
    idleMessageTicker(seconds);
  } else 
  {
    //Display Menu 
    menuDisplay(currentMenu);
  }
   
  if (autopilotOn)
  { 
   valveAutopilot(); 
  }
  
   //Stop counter from overflow..
   if (idleTicks<1000) { ++idleTicks; } 
   
   //Everything works at 1Hz
   delay(500); 
}

