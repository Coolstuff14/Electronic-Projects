/**
Jake Lee 2016
FM Radio w/ LCD and Keypad

Arduino FM Radio Receiver Lee

10
This code untilizes an I2C LCD and a 4x3 matrixed number keypad and the I2C Fm radio
chip TEA5767
The user can change the radio station by typing it in using the * as a period and # as the
submit button. The LCD shows the current station and whether or not its in stereo. It also
show the signal strength out of 15.
*/
//Wire library used for I2C comunication
#include <Wire.h>
//LCD defines for sending data and commands
#define ADDR 0x27
#define RSC 0
#define RSD 1
#define RW_READ 2
#define WR 0
#define EN_HIGH 4
#define EN_LOW 0
#define BL 8
//Radio frequency change variables
unsigned char frequencyH = 0;
unsigned char frequencyL = 0;
unsigned int frequencyB;
float frequency = 0;
float count=0.1;
//Keypad Functions variables
byte h=0,v=0; //variables used in for loops
const unsigned long period=50; //little period used to prevent error
const unsigned long setPeriod = 5000; // period of wait for
unsigned long kdelay=0; // variable used in non-blocking delay
unsigned long setDly=millis(); // var used for reseting screen after no more button
presses
const byte rows=4; //number of rows of keypad
const byte columns=3; //number of columnss of keypad
const byte Output[rows]={5,6,7,8}; //array of pins used as output for rows of keypad
const byte Input[columns]={2,3,4}; //array of pins used as input for columnss of keypad
//Other variables
int i=0;
char station[4]; //Holds the buttons pushed
String masterLCD = "Frequency: "; //used for holding last value presed on keypad when
chaging station

Arduino FM Radio Receiver Lee

11

int lbpF=0; //Last button pushed timer flag
// function used to detect which button is used
byte keypad()
{
static bool no_press_flag=0; //static flag used to ensure no button is pressed
for(byte x=0;x<columns;x++) // for loop used to read all inputs of keypad to ensure no
button is pressed
{
if (digitalRead(Input[x])==HIGH); //read every input if high continue else break;
else
break;
if(x==(columns-1)) //if no button is pressed
{
no_press_flag=1;
h=0;
v=0;
}
}
if(no_press_flag==1) //if no button is pressed
{
for(byte r=0;r<rows;r++) //make all outputs low
digitalWrite(Output[r],LOW);
for(h=0;h<columns;h++) //check if one of inputs is low
{
if(digitalRead(Input[h])==HIGH) //if specific input is remain high (no press on it) continue
continue;
else //if one of inputs is low
{
for (v=0;v<rows;v++) //specify the number of row
{
digitalWrite(Output[v],HIGH); //make specified output as HIGH
if(digitalRead(Input[h])==HIGH) //if the input that selected from first sor loop is change
to high
{
no_press_flag=0; //reset the no press flag;
for(byte w=0;w<rows;w++) // make all outputs low
digitalWrite(Output[w],LOW);
return v*4+h; //return number of button
}
}
}
}
}
return 50;
}

Arduino FM Radio Receiver Lee

12

//Return the coresponding character from the value returned from keypad
char getKey(int val){
switch (val)
{
case 0:
return '#';
break;
case 1:
return '0';
break;
case 2:
return '.';
break;
case 4:
return '9';
break;
case 5:
return '8';
break;
case 6:
return '7';
break;
case 8:
return '6';
break;
case 9:
return '5';
break;
case 10:
return '4';
break;
case 12:
return '3';
break;
case 13:
return '2';
break;
case 14:
return '1';
break;
default:
;
}
}

Arduino FM Radio Receiver Lee

13

//Creates the whole lcd layout with frequency, stereo, and signal strength
void currentLcd(){
resetLCD();
String tmp = "Frequency: ";
tmp.concat(frequency); //Concat allows me to combine a string and float
tmp.remove(tmp.length()-1); //Removes the extra 0 from the 2 decimal float
stringLcd(tmp); //Sends to lcd
getStereo(); //Displays ST if stereo reception
dispADC(); //Displays the signal strength bar graph
masterLCD="Frequency: "; //Resets masterLCD
}
void chkBtns(){
if(millis()-kdelay>period) //used to make non-blocking delay
{
kdelay=millis(); //capture time from millis function
int keyV=keypad(); //get button press value
char key = getKey(keyV); //get char assosiated with button press value
//This is the timout code. If you don't finish typing in your station or
//don't hit enter it resets the screen by calling currentLCD() because
//frequency has not be changed yet.
if (millis()-setDly>setPeriod){ //This checks to see if the system time millis() - the last time you
pressed a button (setDly) is greater then the timout delay
while (lbpF==1){ //This gets set to 1 when a button is pushed and 0 when the station is
entered
currentLcd();
masterLCD="Frequency: ";
i=0; //necessary to reset the char array that holds button pushes or else it shows the
buttons pushed before the timeout
lbpF = 0; //reset flag
}
}
//MAGIC RIGHT HERE
//This is where all the magic happens if keyV is less than 50 meaning that a button was
pushed
if (keyV<50){
setDly=millis(); //Get system time that a button was pushed
lbpF=1; //Set timeout flag
resetLCD(); //resets the lcd
if (i<=5){ //stations can only be a max of 5 chars ex. 101.2
i++;
stringLcd(masterLCD += key); //add the key pressed to the lcd display
station[i-1] = key; //add the key pressed to the station char array

Arduino FM Radio Receiver Lee

14


}
}
if (key == '#'){ //The station has be entered
if (i==4){ //checks for a station that is only 4 chars long like 94.5 and adds an extra 0 to fill
the char array
station[4] = '0';
}
float sVal=atof(station); //converts the char array station containing the station to a float
frequency = sVal; //sets the global frequency
setFrequency(); //set the frequency on the chip
delay(100);
currentLcd(); //shows the new frequency on the lcd
lbpF=0; //resets timout flag
i=0; //resets station char array
}

}
}
//Sets the frequency on the TEA5767
void setFrequency()
{
frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
frequencyH = frequencyB >> 8;
frequencyL = frequencyB & 0XFF;
delay(100);
Wire.beginTransmission(0x60);
Wire.write(frequencyH);
Wire.write(frequencyL);
Wire.write(0xB0);
Wire.write(0x10);
Wire.write((byte)0x00);
Wire.endTransmission();
delay(100);
}
//Sends data bytes to the LCD
void sendData(byte dataByte) {
byte highNibble = dataByte & 0xF0;
byte lowNibble = dataByte << 4;
Wire.beginTransmission(ADDR);
Wire.write(byte(highNibble|BL|EN_HIGH|WR|RSD));
Wire.write(byte(highNibble|BL|EN_LOW|WR|RSD));
Wire.write(byte(highNibble|BL|EN_HIGH|WR|RSD));
Wire.write(byte(lowNibble|BL|EN_HIGH|WR|RSD));

Arduino FM Radio Receiver Lee

15

Wire.write(byte(lowNibble|BL|EN_LOW|WR|RSD));
Wire.endTransmission();
}
//Send command bytes to the LCD
void sendCmd(byte cmdByte) {
byte highNibble = cmdByte & 0xF0;
byte lowNibble = cmdByte << 4;
Wire.beginTransmission(ADDR);
Wire.write(byte(highNibble|BL|EN_HIGH|WR|RSC));
Wire.write(byte(highNibble|BL|EN_LOW|WR|RSC));
Wire.write(byte(highNibble|BL|EN_HIGH|WR|RSC));
Wire.write(byte(lowNibble|BL|EN_HIGH|WR|RSC));
Wire.write(byte(lowNibble|BL|EN_LOW|WR|RSC));
Wire.endTransmission();
}
//Resets the lcd
void resetLCD() {
sendCmd(0x00); // reset display
sendCmd(0x01);
sendCmd(0x02); // go home
sendCmd(0x06); // entry mode
sendCmd(0x0C); // display on
delay(100);
}
//Sends strings to the lcd
void stringLcd(String text){
char charBuf[text.length()+1]; //creates a char array of the strings length
text.toCharArray(charBuf, text.length()+1); //converts the string to a char array
for (int j = 0 ; j < text.length() ; j++) //loops though array
{
sendData(charBuf[j]); //sends each char to the lcd
}
}
//Gets the stereo bit from the TEA5767 in byte 3, bit 7
//if its 1 then stereo
//if its 0 then no stereo
//displays to the lcd
void getStereo(){
unsigned char buffer[5];
Wire.requestFrom(0x60,5); //reading TEA5767
if (Wire.available())
{

Arduino FM Radio Receiver Lee

16

for (int i=0; i<5; i++) {
buffer[i]= Wire.read();
}
byte stereo = buffer[2]&0x80; //mask out bit 7
if (stereo == 0x80){
stringLcd(" ST");
}else{
stringLcd(" ");
}

}
delay(100);
}
//Gets the analog to digital conversion of the signal strength from the TEA5767 byte 4 bit 4-7
int getAdcLvl(){
unsigned char buffer[5];
Wire.requestFrom(0x60,5); //reading TEA5767
if (Wire.available())
{
for (int i=0; i<5; i++) {
buffer[i]= Wire.read();
}
int adcLvl = (buffer[3]>>4);
return adcLvl;
}
}
//Displays the signal strength to the lcd in the form of a progress bar 0-15
void dispADC(){
int lvl = getAdcLvl();
int olvl = lvl;
stringLcd(" [");
for (int q=0; q<15; q++){
if (lvl!=0){
stringLcd("#");
lvl--;
}else{
stringLcd(" ");
}
}
stringLcd("] SIG: ");
stringLcd((String)olvl);
stringLcd("/15");
}

Arduino FM Radio Receiver Lee

17

void setup()
{
//make pin mode of outputs as output
for(byte i=0;i<rows;i++){pinMode(Output[i],OUTPUT);}
//make pin mode of inputs as inputpullup
for(byte s=0;s<columns;s++){pinMode(Input[s],INPUT_PULLUP);}
Serial.begin(9600);
Wire.begin();
frequency = 94.5; //starting frequency
currentLcd(); //Set the lcd
delay(100);
setFrequency(); //Set the frequency
delay(100);
}
void loop()
{
chkBtns(); //Check for button pushes
}
