
#include <SoftwareSerial.h>
#include <WString.h>
SoftwareSerial GPSSerial(4,5);  //GPS Serial 
SoftwareSerial LCD(3, 2);//LCD Serial
const int LCDdelay=10;
char inByte;
char chkByte;
int i = 5;
int resetPin = 7;
int page = 1;
float speedMPH;
float MPS;
float ds;
float Miles = 0;
String velocity = String();
String chkStr = String();
String dataStr = String();
String finStr = String();
String returnStr = String(12);


// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}
void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
   delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}



void setup()
{
Serial.begin(57600); //Arduino Serial
pinMode(2, OUTPUT);
LCD.begin(9600); //LCD Serial 
clearLCD();  
lcdPosition(0,0); 
GPSSerial.begin(9600); //GPS Serial

pinMode(resetPin, INPUT);




GPSSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);


// 1 Hz update rate
//mySerial.println(PMTK_SET_NMEA_UPDATE_1HZ);
// 5 Hz update rate- for 9600 baud you'll have to set the output to RMC only (see above)
GPSSerial.println(PMTK_SET_NMEA_UPDATE_5HZ);
// 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
//mySerial.println(PMTK_SET_NMEA_UPDATE_10HZ);

}

void waitForByte() {
  while (true) {
    if (GPSSerial.available() > 0) {
	break;
    }
    else {
	continue;
    }
  }
}

void resetData() {
  dataStr = '\0';
  
                               
  LCD.flush();
  Serial.flush();
}

void printSpeed() {
  dataParse(7);
  clearLCD();
  LCD.listen(); 
  lcdPosition(0,0); 
  speedMPH = returnStr.toFloat() * 1.15;
  if (speedMPH < 0.75) {
    speedMPH = 0;
    LCD.print(speedMPH);
    LCD.print(" MPH");}
    else {
      LCD.print(speedMPH);
      LCD.print(" MPH");}
  MPS = speedMPH * .0002777777;
  ds = MPS
  
  ;
  if (digitalRead(resetPin) == 1) {
    Miles = 0; }
    else { 
        Miles = Miles + ds; }
  lcdPosition(1,0);
  LCD.print(Miles,2);
  LCD.print(" Miles");
  Serial.println(speedMPH);
    
}

void resetReturn() {
  returnStr = '\0';
} 

void dataParse(int section) {    
  char nextChar;
  int commas = 0;
  resetReturn();
  for (int x = 0; x <= dataStr.length(); x++) {
    nextChar = dataStr.charAt(x);
    if (nextChar == ',') {
     commas++;
     continue;
    }
    if (commas == section) {
	 returnStr.concat(nextChar);
    }
    else if (commas > section) {
	break;
    }
  }  
}

void loop() {
  GPSSerial.listen();
  chkStr = '\0';
  
  if (GPSSerial.available() > 0) {
    
    chkByte = (char)GPSSerial.read();
    while (chkByte == '$') {
	if (i > 0) {
	  if (GPSSerial.available() > 0) {
	    inByte = GPSSerial.read();
	    chkStr.concat(inByte);
	    i--;
	  }
	  else {
	    waitForByte();
	  }
	}
	else {
	  i = 5;
	  break;
	}
	while (chkStr.equals("GPRMC")) {
	  if (GPSSerial.available() > 0) {
            if(dataStr.length() == 65) {
              break;
	       }
            else { 
               inByte = GPSSerial.read();
	       dataStr.concat(inByte);
            }
}
	  else {
	    waitForByte();
	  }
           if (dataStr.length() == 65) {
                printSpeed();
                        
                resetData();
                break;
               
                  
            
               }
              }
            }
	  }

	}
      
  

      
 







