/*
This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 Pinout for HP Bubble Display:
 1:  Cathode 1
 2:  Anode E
 3:  Anode C
 4:  Cathode 3
 5:  Anode dp
 6:  Cathode 4
 7:  Anode G
 8:  Anode D
 9:  Anode F
 10: Cathode 2
 11: Anode B
 12: Anode A
 
*/

#include "SevSeg.h"
#include "Wire.h"
#include <ADCTouch.h>
#define DS3231_I2C_ADDRESS 0x68
int ref0;       //reference values to remove offset
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

//Create an instance of the object.
SevSeg myDisplay;

//Create global variables
unsigned long timer;
int deciSecond = 0;

void setup()
{
   ref0 = ADCTouch.read(A1, 500);    //create reference values to

  int displayType = COMMON_CATHODE; //Your display is either common cathode or common anode

  
  //This pinout is for a bubble dispaly
       //Declare what pins are connected to the GND pins (cathodes)
       int digit1 = 8; //Pin 1
       int digit2 = 5; //Pin 10
       int digit3 = 11; //Pin 4
       int digit4 = 13; //Pin 6
       
       //Declare what pins are connected to the segments (anodes)
       int segA = 7; //Pin 12
       int segB = 6; //Pin 11
       int segC = 10; //Pin 3
       int segD = 3; //Pin 8
       int segE = 9; //Pin 2
       int segF = 4; //Pin 9
       int segG = 2; //Pin 7
       int segDP= 12; //Pin 5
   
  int numberOfDigits = 4; //Do you have a 1, 2 or 4 digit display?

  myDisplay.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);
  
  myDisplay.SetBrightness(100); //Set the display to 100% brightness level

   Wire.begin();
   //Serial.begin(9600);
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x1f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime(char *chtm)
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  String ttime;

  if (hour<10)
  {
    //Serial.print("0");
    ttime += " " + String(hour);
  }
  else
  {
    ttime +=  String(hour);
  }
  //Serial.print(hour, DEC);
  
 
  
  //Serial.print(':');


  if (minute<10)
  {
    //Serial.print("0");
    ttime += 0 + String(minute);
  }
  else
  {
    ttime +=  String(minute);
  }
  
  //Serial.println(minute, DEC);
  
  ttime.toCharArray(chtm,10);
  
  
  //Serial.println(ttime);
  
}


void loop()
{

  int value1 = ADCTouch.read(A1);  //   --> 100 samples
  value1 -= ref0;
  if(value1 > 350)
  {
  char curtime[10];
  displayTime(curtime);
  
  for(int i =0;i<200;i++)
  {
    
  myDisplay.DisplayString(curtime, 0); //(numberToDisplay, decimal point location)
  delay(10);
  
  }
  }

  delay(100);


}
  
 



