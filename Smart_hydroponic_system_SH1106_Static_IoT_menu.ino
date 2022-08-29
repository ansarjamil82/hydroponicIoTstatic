/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/* initialization for Favoriot*/
#define APIKEY  "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFuc2FyamFtaWwiLCJyZWFkX3dyaXRlIjp0cnVlLCJpYXQiOjE1MTEyNTc2MzN9.sM7jSnDtAXIphAx26iOyGUptnT7Sd839sHexMnG1XvQ"
#define DEVICE_DEV_ID "DosingHydroponic@ansarjamil"

char ssid[] = "hanisansar_2.4G@unifi"; //  your network SSID (name)
char pass[] = "han1sl1yana";    // your network password (use for WPA, or use as key for WEP)

const String DEVICE = "Dosing_1@ansarjamil"; // Replace with the id_developer of your device

int status = WL_IDLE_STATUS;

int IOT_ENABLE = 0;
int attempt = 0;

char server[] = "apiv2.favoriot.com/v2";    //  address for FAVORIOT Platform

WiFiClient client;

String txt = "OFF";          // Text to send


unsigned long SendDataPeriod = 20000;// To set period of sending data to favoriot
static unsigned long SendTimepoint = 0;


/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo_bmp[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x8f, 0xff, 0x0f, 0xe0, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x04, 0x00, 0x07, 0x9f, 0xff, 0x9f, 0xf8, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0c, 0x00, 0x0f, 0x9f, 0xff, 0x9f, 0xfc, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x0f, 0x1f, 0x8f, 0xff, 0x1f, 0xfe, 0x0f, 0xef, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x10, 0x9f, 0x80, 0xf0, 0x1c, 0x1e, 0x1f, 0x03, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x20, 0x3f, 0x80, 0xf0, 0x1c, 0x1f, 0x1e, 0x01, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x20, 0x7f, 0x80, 0xf0, 0x1c, 0x1f, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x20, 0xff, 0x80, 0xf0, 0x1c, 0x1e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x21, 0xf7, 0x80, 0xf0, 0x1c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x11, 0xe7, 0x80, 0xf0, 0x1d, 0xfc, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x03, 0xc7, 0x80, 0xf0, 0x1d, 0xf8, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x04, 0x07, 0x87, 0x80, 0xf0, 0x1c, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x04, 0x0f, 0x3f, 0x80, 0xf0, 0x1c, 0x7c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x02, 0x1f, 0x7f, 0x80, 0xf0, 0x1c, 0x7c, 0x1e, 0x01, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x3e, 0x7f, 0x80, 0xf0, 0x1c, 0x1e, 0x1f, 0x83, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x7c, 0x07, 0x80, 0xf0, 0x1c, 0x1f, 0x0f, 0xef, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x78, 0x07, 0x80, 0xf0, 0x1c, 0x0f, 0x8f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xf8, 0x07, 0x80, 0xf0, 0x1c, 0x07, 0x83, 0xff, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x70, 0x03, 0x80, 0x60, 0x1c, 0x03, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xa1, 0xb7, 0xfd, 0xc5, 0xbb, 0x0b, 0x94, 0x7f, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x68, 0xf4, 0xfc, 0xac, 0x83, 0x0b, 0xa4, 0x7c, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


int menustate = 0;
int TargetPPM = 0;
int TDSmeter = 0;
int TDSerror = 0;
//int TDSreading = 0;

//Dosing state
int Dosingstate = 0; //0 no dosing, 1: Dynamic Dosing 2: Static dosing

/*for JSN-SR04T Connections*/

// Define Trig and Echo pin:
const int trigPin = 25;
const int echoPin = 35;

// Define variables:
long duration;
int distance;

int WaterLevelstate = 0;


/*for pushbutton*/
// constants won't change. They're used here to set pin numbers:
const int buttonPinUP = 13;    // the number of the pushbutton pin
const int buttonPinDOWN = 12;    // the number of the pushbutton pin
const int buttonPinHOME = 14;    // the number of the pushbutton pin
//const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin

/*UP button*/
int buttonStateUP;             // the current reading from the input pin
int lastButtonStateUP = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTimeUP = 0;  // the last time the output pin was toggled

/*DOWN button*/
int buttonStateDOWN;             // the current reading from the input pin
int lastButtonStateDOWN = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTimeDOWN = 0;  // the last time the output pin was toggled

/*HOME button*/
int buttonStateHOME;             // the current reading from the input pin
int lastButtonStateHOME = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTimeHOME = 0;  // the last time the output pin was toggled


unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
 int changestateDOWN = 0;
 int changestateUP = 0;
 int changestateHOME = 0;

 //holdbutton
unsigned long holdButtonUP = 0; 
unsigned long holdButtonDOWN = 0; 
unsigned long holdingTime = 4000; 


//pump
const int PumpA = 33;    // the number of the pushbutton pin
const int PumpB = 32;    // the number of the pushbutton pin
int pumpstate = 0;

//tds
#define TdsSensorPin 34 // istihar pin 34 untuk variable data TDS sensor
#define VREF 3.3// analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT]; //istihar data type
int analogBufferIndex = 0,copyIndex = 0; //istihar data type
float averageVoltage = 0, ecValue = 0, temperature = 25, tdsError = 0, averageVoltage1 = 0; //istihar data type
int tdsValue = 0,tdsValue1= 0, tdsValuemap= 0, tdsShow = 0, tdsSensor = 0 ;
int tdsdifcounter=0;
int tdscounter=0;
int tdsbefore; //istihar data type
int tdsnew; //istihar data type
int tdsdif; //istihar data type
int grafstatic; //istihar data type

static unsigned long analogSampleTimepoint = 0;  
static unsigned long printTimepoint = 0;

//algorithm

int tdsDiff = 0;
int tdsOld = 0;
int tdsoldRef = 0;
int constReading =0;
int pumpCount = 0;
int timePump = 0;
float timePumpref = 0;
//int targetPPM = 0;


void setup() {
  Serial.begin(115200);

  display.begin(i2c_Address, true); // Address 0x3C default
 //display.setContrast (0); // dim display

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  //delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
//  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
 // display.display();
 // delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
  display.clearDisplay();
  display.drawBitmap(0, 0, logo_bmp, 126, 32, SH110X_WHITE);
  display.display();
  delay(3000);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SH110X_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("HYDRPONIC"));
  display.println(F("  AUTOMATIC"));
  display.println(F("    DOSING"));
  display.println(F("      SYSTEM"));
  display.display();
  delay(3000);

/**/
  pinMode(buttonPinUP, INPUT);
  pinMode(buttonPinDOWN, INPUT);
  pinMode(buttonPinHOME, INPUT);
 // pinMode(ledPin, OUTPUT);
  pinMode(PumpA, OUTPUT);
  pinMode(PumpB, OUTPUT);

 //both pump off

 digitalWrite(PumpA,HIGH);
 digitalWrite(PumpB,HIGH);

 pinMode(TdsSensorPin,INPUT);


 //JSN-SR04T

  // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  

 //connecting to wifi
/*
   while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/

  // set initial LED state
 // digitalWrite(ledPin, ledState);
}

void loop() {
checkpushbutton();
menucurrentstate();  
menudisplay();

}

void checkpushbutton(){
readbuttonUP();
readbuttonDOWN();
readbuttonHOME();

/*Not allowed Multiple button being pressed*/

 }


void readbuttonUP(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPinUP);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonStateUP) {
    // reset the debouncing timer
    lastDebounceTimeUP = millis();
    changestateUP = 1;
  }

  if ((millis() - lastDebounceTimeUP) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:   
    // if the button state has changed:
    
      // only toggle the LED if the new button state is HIGH
      if (reading == HIGH) { 
        if(changestateUP == 1){
        Serial.println(F("Button UP is Pressed "));
        buttonStateUP = 1;
        changestateUP = 0;
        holdButtonUP = millis();
        }else if ((millis() -  holdButtonUP) > holdingTime){
    //    Serial.println(F("Button UP is hold"));  
        buttonStateUP = 1;    
        }else{
        buttonStateUP = 0;  
        }
      }else{
        buttonStateUP = 0;
        holdButtonUP = millis();
    }
  }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonStateUP = reading;
}


void readbuttonDOWN(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPinDOWN);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonStateDOWN) {
    // reset the debouncing timer
    lastDebounceTimeDOWN = millis();
    changestateDOWN = 1;
  }

  if ((millis() - lastDebounceTimeDOWN) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:   
    // if the button state has changed:
    
      // only toggle the LED if the new button state is HIGH
      if (reading == HIGH) {
        if(changestateDOWN == 1){
        Serial.println(F("Button DOWN is Pressed "));
        buttonStateDOWN = 1;
        changestateDOWN = 0;
        holdButtonDOWN = millis();
        }else if ((millis() -  holdButtonDOWN) > holdingTime){
 //       Serial.println(F("Button UP is hold"));  
        buttonStateDOWN = 1;    
        }else{
        buttonStateDOWN = 0;  
        }
      }else{
        buttonStateDOWN = 0;
    }
  }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonStateDOWN = reading;
  
}

void readbuttonHOME(){
 // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPinHOME);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonStateHOME) {
    // reset the debouncing timer
    lastDebounceTimeHOME = millis();
    changestateHOME = 1;
  }

  if ((millis() - lastDebounceTimeHOME) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:   
    // if the button state has changed:
    
      // only toggle the LED if the new button state is HIGH
      if (reading == HIGH) {
        if(changestateHOME == 1){
 //       Serial.println(F("Button HOME is Pressed "));
        buttonStateHOME = 1;
        changestateHOME = 0;
        }else{
        buttonStateHOME = 0;  
        }
      }else{
        buttonStateHOME = 0;
    }
  }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonStateHOME = reading;
}

void menucurrentstate(){

  if (menustate == 0){
    if(buttonStateHOME == 1){
      menustate = 4;  // keyin PPM values
    }else if(buttonStateDOWN == 1){
      menustate = 1;
    }else{
      menustate = 0;
    }  
  }else if(menustate == 1){
    if(buttonStateHOME == 1){
      menustate = 5;  // keyin PPM values
    }else if(buttonStateUP == 1){
      menustate = 0;
    }else if(buttonStateDOWN == 1){
      menustate = 2;
    }else{
    menustate = 1;
    }
  
  }else if(menustate == 2){
  
    if(buttonStateHOME == 1){
      menustate = 15;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 1;
    }else if(buttonStateDOWN == 1){
      menustate = 3;
    }else{
      menustate = 2;
    }
  
  }else if(menustate == 3){// this state is for keyin Target PPM value

    if(buttonStateHOME == 1){
      menustate = 20;  // g1o to menu 
    }else if(buttonStateUP == 1){
      menustate = 2;  
    }else{
      menustate = 3;
    }

  }else if (menustate == 4){
    if(buttonStateHOME == 1){
      menustate = 0;  
    }else{
      menustate = 4;
    } 
  
  }else if (menustate == 5){
    if(buttonStateHOME == 1){
      menustate = 8;  
    }else if (buttonStateDOWN == 1){
      menustate = 6; 
    }else{
      menustate = 5;
    } 

  }else if(menustate == 6){
  
    if(buttonStateHOME == 1){
      menustate = 10;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 5;
    }else if(buttonStateDOWN == 1){
      menustate = 7;
    }else{
      menustate = 6;
    }

  }else if(menustate == 7){
  
    if(buttonStateHOME == 1){
      menustate = 1;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 6;
    }else{
      menustate = 7;
    }

  }else if(menustate == 8){
  
    if(buttonStateHOME == 1){
      menustate = 9;  // keyin TDS meter value
    }else{
      menustate = 8;
    }

  }else if(menustate == 9){
  
    if(buttonStateUP == 1){
      menustate = 5;
    }else if(buttonStateDOWN == 1){
      menustate = 8;
    }else{
      menustate = 9;
    }
  }else if(menustate == 10){
  
    if(buttonStateHOME == 1){
      menustate = 6;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 13;
    }else if(buttonStateDOWN == 1){
      menustate = 11;
    }else{
      menustate = 10;
    }
    
  }else if(menustate == 11){
  
    if(buttonStateHOME == 1){
      menustate = 10;  // keyin TDS meter value
    }else if(buttonStateDOWN == 1){
      menustate = 12;
    }else{
      menustate = 11;
    }

  }else if(menustate == 12){
  
    if(buttonStateHOME == 1){
      menustate = 10;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 11;
    }else{
      menustate = 12;
    }

  }else if(menustate == 13){
  
    if(buttonStateHOME == 1){
      menustate = 10;  // keyin TDS meter value
    }else if(buttonStateDOWN == 1){
      menustate = 14;
    }else{
      menustate = 13;
    }

  }else if(menustate == 14){
  
    if(buttonStateHOME == 1){
      menustate = 10;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 13;
    }else{
      menustate = 14;
    }
    
  }else if(menustate == 15){
  
    if(buttonStateHOME == 1){
      menustate = 18;  // keyin TDS meter value
    }else if(buttonStateDOWN == 1){
      menustate = 16;
    }else{
      menustate = 15;
    }

  }else if(menustate == 16){
  
    if(buttonStateHOME == 1){
      menustate = 19;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 15;
    }else if(buttonStateDOWN == 1){
      menustate = 17;
    }else{
      menustate = 16;
    }

  }else if(menustate == 17){
  
    if(buttonStateHOME == 1){
      menustate = 2;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 16;
    }else{
      menustate = 17;
    }

  }else if(menustate == 18){
  
    if(buttonStateHOME == 1){
      menustate = 15;  // keyin TDS meter value
    }else if(buttonStateUP == 1){
      menustate = 18;
    }else{
      menustate = 18;
    }

  }else if(menustate == 19){
  
    if(buttonStateHOME == 1){
      menustate = 16;  // keyin TDS meter value
    }else{
      menustate = 19;
    }

  }else if(menustate == 20){
  
    if(buttonStateHOME == 1){
      menustate = 3;  // keyin TDS meter value
    }else{
      menustate = 20;
    }
  }



}


void menudisplay(){
display.clearDisplay();

switch(menustate){
  case 0://selection 0
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(30,0);             // Start at top-left corner
    display.println(F("SETTINGS"));
    display.println(F(""));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("> 1. PPM Value"));
    display.println(F("2. Calibrate"));
    display.println(F("3. IoT"));
    display.println(F("4. Done"));
    display.display();
    delay(10);
    break;

  case 1: //selection 1
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(40,0);             // Start at top-left corner
    display.println(F("SETTINGS"));    
    display.println(F(""));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("1. PPM Value"));
    display.println(F("> 2. Calibrate"));
    display.println(F("3. IoT"));
    display.println(F("4. Done"));
    display.display();
    delay(10);
    break;

  case 2: //selection 2
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(40,0);             // Start at top-left corner
    display.println(F("SETTINGS"));
    display.println(F(""));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("1. PPM Value"));
    display.println(F("2. Calibrate"));
    display.println(F(">3. IoT"));
    display.println(F("4. Done"));
    display.display();
    delay(10);
    break;

  case 3: //selection 4
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(40,0);             // Start at top-left corner
    display.println(F("SETTINGS"));
    display.println(F(""));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("1. PPM Value"));
    display.println(F("2. Calibrate"));
    display.println(F("3. IoT"));
    display.println(F(">4. Done"));
    display.display();
    delay(10);
    break;

  case 4: //selection 0.1
    if(buttonStateUP == 1){
      TargetPPM = TargetPPM + 1;
    }else if (buttonStateDOWN == 1){
      TargetPPM = TargetPPM - 1;
    }

    if (TargetPPM <= 0){ // Lower limit of target PPM is 0
    TargetPPM = 0;  
    }
  
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(30,0);             // Start at top-left corner
    display.println(F("PPM Value"));
    display.setTextColor(SH110X_WHITE);        // Draw white text  
    display.println(F(""));            
    display.println(F("Set PPM Value"));
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(40,32);             // Start at top-left corner
    display.println(TargetPPM);
    display.display();
    delay(10);
    break;


  case 5:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner  
    display.println(F("CALIBRATE"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));            
    display.println(F(">1. Calibrate Sensor"));
    display.println(F("2. Pump Test"));
    display.println(F("3. Done"));
    display.display();
    delay(10);
    break;

 
  case 6:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("CALIBRATE"));
    display.setTextColor(SH110X_WHITE);        // Draw white text      
    display.println(F(""));        
    display.println(F("1. Calibrate Sensor"));
    display.println(F(">2. Pump Test"));
    display.println(F("3. Done"));
    display.display();
    delay(10);
    break;

 
  case 7:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("CALIBRATE"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));              
    display.println(F("1. Calibrate Sensor"));
    display.println(F("2. Pump Test"));
    display.println(F(">3. Done"));
    display.display();
    delay(10);
    break;
    

 case 8: //set TDS meter reading
    
    if(buttonStateUP == 1){
      TDSmeter = TDSmeter + 1;
    }else if (buttonStateDOWN == 1){
      TDSmeter = TDSmeter - 1;
    }
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("Enter TDS Meter Value"));
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(50,8);             // Start at top-left corner
    display.println(TDSmeter);
    display.setTextSize(1);             // Normal 1:1 pixel scale    
    display.println(F(""));  
    display.println(F("HOME: Start"));   
    display.display();
    delay(10);
    break;


   case 9: // find calibration offset and show the result
    tdsValue = 0;
    tdsSensor = readTDSsensor();  //read TDS sensor  
    tdsError = TDSmeter - tdsSensor; //calculate TDSerror
   // display.setTextSize(1);            
   // display.setTextColor(SH110X_WHITE);       
   // display.setCursor(0,16);             
   // display.println(F("Calibrating sensor.."));
   // display.println(F(""));
   // display.println(F("(TDS sensor must be in the tank) "));
   // display.display();
   // delay(3000);     

    Serial.print(tdsValue);
    Serial.print(" ");
    Serial.print(TDSmeter);
    Serial.print(" ");
    Serial.print(tdsError);
    Serial.println(" ");

    tdsValue = readTDSsensor()+ tdsError;
    tdsShow = tdsValue;
   
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("Calibration Done"));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(50,8);             // Start at top-left corner
    display.println(tdsShow);
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.println(F(""));
    display.println(F(""));
    display.println(F("Down: No, UP: Yes"));   
    display.display();
    delay(10);
    break;




  case 10:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(30,0);             // Start at top-left corner
    display.println(F("PUMP TEST"));
    display.println(F(""));    
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.println(F("PUMP A : DOWN Button"));
    display.println(F("PUMP B : UP Button"));
    display.display();
    delay(10);
    break; 

  case 11:  // turn on PUMP A
    digitalWrite(PumpA,LOW); //turn on pump A
    if(buttonStateHOME == 1){
      menustate = 9; 
       digitalWrite(PumpA,HIGH); //turn off pump A 
    }
    
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(20,0);             // Start at top-left corner
    display.println(F("PUMP A"));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(40,16);             // Start at top-left corner
    display.println(F("ON"));
    display.display();
    delay(10);
    break;

   case 12: // Turn off PUMP A
   
    digitalWrite(PumpA,HIGH); 
    if(buttonStateHOME == 1){
       digitalWrite(PumpA,HIGH); 
    }
    
    display.setTextSize(1);           
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        
    display.setCursor(20,0);             
    display.println(F("PUMP A"));
    display.setTextColor(SH110X_WHITE);              
    display.setTextSize(2);            
    display.setCursor(40,16);             
    display.println(F("OFF"));
    display.display();
    delay(10);
    break;


  case 13: //Turn on PUMP B
   digitalWrite(PumpB,LOW);
   if(buttonStateHOME == 1){
      digitalWrite(PumpB,HIGH);
   }
      
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(20,0);             // Start at top-left corner
    display.println(F("PUMP B"));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(40,16);             // Start at top-left corner
    display.println(F("ON"));
    display.display();
    delay(10);
    break;

   case 14:
   
   digitalWrite(PumpB,HIGH);
   if(buttonStateHOME == 1){
      digitalWrite(PumpB,HIGH);
   }
   
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(20,0);             // Start at top-left corner
    display.println(F("PUMP B"));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(40,16);             // Start at top-left corner
    display.println(F("OFF"));
    display.display();
    delay(10);
    break;

  case 15:
    
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("IoT Setting"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));              
    display.println(F(">1. Enable"));
    display.println(F("2. Disable"));
    display.println(F("3. Done"));
    display.display();
    delay(10);
    break;

  case 16:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("IoT Setting"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));              
    display.println(F("1. Enable"));
    display.println(F(">2. Disable"));
    display.println(F("3. Done"));
    display.display();
    delay(10);
    break;

  case 17:
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("IoT Setting"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));              
    display.println(F("1. Enable"));
    display.println(F("2. Disable"));
    display.println(F(">3. Done"));
    display.display();
    delay(10);
    break;

   case 18:

    IOT_ENABLE = 1; //  set IoT to enable to 

    if (WiFi.status() != WL_CONNECTED){
      display.setTextColor(SH110X_WHITE);        // Draw white text          
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setCursor(10,8);             // Start at top-left corner
      display.println(F("Connecting to WiFi"));
      display.println(ssid);
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.println(attempt);
      display.display();
      delay(10);
    // Connect to WPA/WPA2 network. 
    status = WiFi.begin(ssid, pass);
    // wait 1 seconds for connection:
    delay(1000);
    attempt++;
    }else{
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);         
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("Connected"));
    display.println(F(""));          
    display.println(WiFi.localIP());
    display.display();
    attempt = 0;
    delay(10);
    }
    break;


 case 19:
    IOT_ENABLE = 0; //  set IoT to enable to 
    WiFi.disconnect();   //disconnect wifi
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);        // Draw white text    
    display.setCursor(20,0);             // Start at top-left corner
    display.println(F("IoT disabled"));
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.println(F(""));              
    display.display();
    delay(10);
    break;

 case 20:
    dosing();
    if(buttonStateHOME == 1){
      timePumpref = 0;
      pumpCount = 0;
      tdsoldRef = 0;
      constReading = 0;   
    }else{
      tdsValue = readTDSsensor()+ tdsError;  //read TDS sensor  
      tdsShow = tdsValue ; 
    }  
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_BLACK,SH110X_WHITE);        // Draw white text
    display.setCursor(10,0);             // Start at top-left corner
    display.println(F("TDS READING (PPM)"));
    display.setTextColor(SH110X_WHITE);        // Draw white text          
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setCursor(40,16);             // Start at top-left corner
    display.println(tdsShow);
   // display.println(timePumpref);
   // display.println(timePump);
    display.display();
    delay(10);
    break;  
  
  default: 
    break;
  } 
}





int readTDSsensor() {
  while(analogBufferIndex <= SCOUNT){
   if(millis()-analogSampleTimepoint > 40U){  //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
    analogBufferIndex++;
     if(analogBufferIndex == SCOUNT){
        printTimepoint = millis();
        for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
        averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF/ 4095.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
        float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
        tdsValue1=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
        averageVoltage1 =  averageVoltage * 100; 
     //   tdsValuemap = map(tdsValue1,16,959,77,1875); 
        Serial.print("voltage:");
        Serial.print(averageVoltage,2);
        Serial.print(" voltage1:");
        Serial.print(averageVoltage1,2);
        Serial.print("V ");
        Serial.print("TDS Value_cal: ");
        Serial.print(tdsValue1);
        Serial.print("TDS Value_map: ");
        Serial.print(tdsValuemap);        
        Serial.println("ppm");
        analogBufferIndex = 0; 
        return (tdsValue1) ;     
    }
  }
  }
 return tdsValue1;  
}


void dosing(){

if((millis()-printTimepoint) > 20000U){

  tdsValue = readTDSsensor() + tdsError; 
  readWaterLevel ();
      
  tdsDiff = tdsValue - tdsOld;
  Serial.println(timePumpref);     
  Serial.println(pumpCount); 
   //  Serial.print("   ");  
   //  Serial.println(tdsDiff); 
    
  if((tdsDiff < 30) && (tdsDiff > -30)){
     constReading++;
  }else{
     constReading = 0; 
  }

 tdsOld = tdsValue;
 printTimepoint = millis(); 

//this part when the sensor reading is already stabilise
  if(constReading > 5){

    if(tdsValue  < (TargetPPM - 50)){
      
      timePump = 1000; // pump will be on for 1 second
      digitalWrite(PumpA, LOW); // tentukan digital pin pump dan "HIGH" untuk on
      delay(timePump); 
      digitalWrite(PumpA, HIGH); // tentukan digital pin pump dan "HIGH" untuk on  
      delay(500);       
      digitalWrite(PumpB, LOW); // tentukan digital pin relay dan "HIGH" untuk on
      delay(timePump);
      digitalWrite(PumpB, HIGH); // tentukan digital pin relay dan "HIGH" untuk on  
      delay(500);     
      Serial.println("PUMP ON ");   
      pumpstate = 100;
      if (IOT_ENABLE == 1){
      sendDataFavoriot();
      }
      pumpstate = 0;  
    }
  } 
}

  if (IOT_ENABLE == 1){
    if((millis()-SendTimepoint) > SendDataPeriod){
    sendDataFavoriot();
    Serial.println("Sending data to Favoriot");
    SendTimepoint = millis();
  }
 }
}


int getMedianNum(int bArray[], int iFilterLen){
    int bTab[iFilterLen];
    for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++){
      for (i = 0; i < iFilterLen - j - 1; i++){
        if (bTab[i] > bTab[i + 1]){
          bTemp = bTab[i];
          bTab[i] = bTab[i + 1];
          bTab[i + 1] = bTemp;
        }
      }
    }

    if ((iFilterLen & 1) > 0)
      bTemp = bTab[(iFilterLen - 1) / 2];
    else
      bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    return bTemp;
}


void sendDataFavoriot(){
if (status != WL_CONNECTED){
   while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 1 seconds for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");
}
    
    tdsValue = readTDSsensor(); 
    StaticJsonDocument<200> doc;
  
    JsonObject root = doc.to<JsonObject>(); // Json Object refer to { }
    root["device_developer_id"] = DEVICE_DEV_ID;
  
    JsonObject data = root.createNestedObject("data");
    data["tds"] = (int)tdsValue;
    data["waterlevel"] = (int)WaterLevelstate;
    data["pumpstate"] = (int)pumpstate;

    String body;
    serializeJson(root, body);
    Serial.println(body);
  
    HTTPClient http;
  
    http.begin("https://apiv2.favoriot.com/v2/streams");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Apikey", APIKEY);
  
    int httpCode = http.POST(body);
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    }
    else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  delay(500);
}

void readWaterLevel (){
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  
  delayMicroseconds(5);
 // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance:
  distance = duration*0.034/2;
  
  // Print the distance on the Serial Monitor (Ctrl+Shift+M):
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= 20){
    WaterLevelstate = 100;
  }else if (distance > 20 && distance <= 25){
    WaterLevelstate = 75;
  }else if (distance > 25 && distance <= 30){
    WaterLevelstate = 50;    
  }else if(distance > 30){
    WaterLevelstate = 25;
  }
  delay(100);
}
