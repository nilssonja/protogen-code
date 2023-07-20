/*
  Version 7.0   Last update: 12-06-2022
  This code is made for using the MAX7219 led matrices in protogen helmets.
  Current features:
  - Multiple expressions with a single button
  - Visor detection by using a hall sensor
  - Boop detection by using an infrared or linetracking sensor
  - Mouth animation with sound detection by using the max4466 microphone from Adafruit

  BEFORE UPLOADING!!
  Included in the dowloaded folder are libraries that need to be installed first for the code to work.
  If you don't know to do that, google for 'installing library Arduino', there are plenty of guides.

  The library for controlling the matrices has to be added to the Arduino IDE before the code can work. The library
  that comes with this code has been modified so that you can link up to 14 matrices instead of 10.

  ---IMPORTANT---
  1. The MAX7219 matrices can be glitchy if you use dupont pins, it is highly recommend to solder the wires directly to the boards!
  2. Because the max7219 led matrices are sometimes flipped you may have to rotate some icons.
     Icons can easily be rotated in the 'setup void()' part of the code below this.

  If something doesn't work properly make sure all the wired connections are correct and that the code actually uploaded.
  If issues continue to appear, send me an e-mail to feronium128@gmail.com
  or contact me on twitter @Feronium


  Have fun!
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "arduinoFFT.h"
#include "Icons.h"

// You can activate features like visor detection and sound detection by
// deleting the 2 slashes before the features in the lines below, this will uncomment them.
//#define VisorSensor
// #define BoopSensor
#define Microphone

const int pinCS = 8; // Din = pin 11   CLK = pin 13
const int numberOfHorizontalDisplays = 14;
const int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

/////////////////////////// Expressions \\\\\\\\\\\\\\\\\\\\\\\\\\\

int expressions = 4; // Here you can change the amount of expressions you want to use

/////////////////////////// I/O Pins \\\\\\\\\\\\\\\\\\\\\\\\\\\

const int ButtonPin = 2;
const int VisorPin = 3;
const int redPin = 5; // These pins are used for the expression indicating rgb LED.
const int greenPin = 9; //" "
const int bluePin = 10; // " "
const int irPin = A7; // Analog input for infrared or 'boop' sensor
const int micPin = A0;  // Analog input for microphone

//////////////////////////////// Icon Posistions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

signed int EyeLeft = 96; // Horizontal positions off the icons on the screens
signed int MawLeft = 64;
signed int NoseLeft = 56;
signed int NoseRight = 48;
signed int MawRight = 16;
signed int EyeRight = 0;

//////////////////////////////// Variables \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

volatile unsigned long glitchDelay = 0;
volatile unsigned long glitchSpeed = 0;
volatile unsigned long blinkTime = 0;
volatile unsigned long blinkSpeed = 0;
volatile unsigned long boopTime = 0;
volatile unsigned long speakingDelay = 0;
volatile unsigned long debounceTime = 0;
volatile unsigned long debounceTime2 = 0;
volatile unsigned long debounceTime3 = 0;
volatile unsigned long previousMillis = 0;


bool visorState = 0;
bool micState = 0;
bool boopState = 0;
bool boopTimer = 0;
bool boopActive = 0;
bool speakingActive = 0;
bool boopDisabled = 0;

bool rising = 1;
bool ledstate = 0;
byte state = 0;
int timer = 1;
int y;
int x;
int Step = 0;
int counter = 0;
const unsigned long ledTime = 1000;

//Blink variables
unsigned long blinkDelay = 9000;
bool blinkState = 0;


//Smoothing Microphone
int sensitivity = 65; // Change this value if your microphone is too sensitive, or not sensitive enough
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
float filterDCComponent = 510;

//FFT variables
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
#define CHANNEL micPin
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 4000; //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[samples];
double vImag[samples];
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

///////////////////////////Setup runs once\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setup() {
  Serial.begin(9600);
  matrix.setIntensity(1);                   //If some of your icons are turned upside down, try
  matrix.setRotation(0, 1); // Right eye    //changing the second number Like 1, 2 or 3.
  matrix.setRotation(1, 1); // Right eye
  matrix.setRotation(2, 1); // Right mouth
  matrix.setRotation(3, 1); // Right mouth
  matrix.setRotation(4, 1); // Right mouth
  matrix.setRotation(5, 1); // Right mouth
  matrix.setRotation(6, 1); // Right nose
  matrix.setRotation(7, 1); // Left nose
  matrix.setRotation(8, 1); // Left mouth
  matrix.setRotation(9, 1); // Left mouth
  matrix.setRotation(10, 1); // Left mouth
  matrix.setRotation(11, 1); // Left mouth
  matrix.setRotation(12, 1); // Left eye
  matrix.setRotation(13, 1); // Left eye
  matrix.fillScreen(LOW);

  expressions--;

  pinMode(ButtonPin, OUTPUT);
  pinMode(VisorPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(irPin, INPUT);
  pinMode(micPin, INPUT);
  digitalWrite(irPin, LOW);
  digitalWrite(micPin, LOW);
  digitalWrite(ButtonPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(ButtonPin), ISR_button, FALLING);

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));

#ifdef VisorSensor
  visorState = 1;
#endif
#ifdef Microphone
  micState = 1;
#endif
#ifndef BoopSensor
  boopDisabled = 1;
#endif
}

///////////////////////////Actual code begins here\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loop() {
  if ((digitalRead(VisorPin) == 0) || (visorState == 0)) {
    // Serial.println(analogRead(irPin));
    // Serial.println(analogRead(micPin));
    if ((analogRead(irPin) > 500)||(boopDisabled == 1)) { //Change the '>' to '<' if the boop detection works the wrong way round
      boopState = 0;
      boopTimer = 1;
    }
    else {
      boopState = 1;
    }
    if ((boopState * boopTimer) != 1) {
      boopActive = 0;
      boopTime = millis();
      if (rising == 1) {
        matrix.fillScreen(LOW);
        matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, HIGH);
        matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, HIGH);
        matrix.drawBitmap(NoseRight, 0, nose, 8, 8, HIGH);
        matrix.drawBitmap(MawRight, 0, maw, 32, 8, HIGH);
        switch (state) {
          case 0:                             //First button press: Happy expression
              matrix.drawBitmap(EyeLeft, 0, EyeL, 16, 8, HIGH);
              matrix.drawBitmap(EyeRight, 0, Eye, 16, 8, HIGH);
              rising = 0;
            if (blinkState == 0) {
              setColor(0, 8, 0);               //Makes the colour of the rgb LED green
            }
            previousMillis = millis();
            break;

          case 1:                             //Second button press: Surprised
              matrix.drawBitmap(EyeLeft, 0, SpookedL, 16, 8, HIGH);
              matrix.drawBitmap(EyeRight, 0, Spooked, 16, 8, HIGH);
              rising = 0;
            if (blinkState == 0) {
              setColor(0, 0, 8);                //Makes the colour of the LED blue
            }
            previousMillis = millis();
            break;

          case 2:                             //Third button press: Angry expression
              matrix.drawBitmap(EyeLeft, 0, AngryL, 16, 8, HIGH);
              matrix.drawBitmap(EyeRight, 0, Angry, 16, 8, HIGH);
              rising = 0;
            if (blinkState == 0) {
              setColor(8, 0, 0);               //Makes the colour of the LED red
            }
            previousMillis = millis();
            break;

          case 3:
              matrix.drawBitmap(EyeLeft, 0, vwvL, 16, 8, HIGH);
              matrix.drawBitmap(EyeRight, 0, vwv, 16, 8, HIGH);
              rising = 0;
            if (blinkState == 0) {
              setColor(4, 0, 4);                //Makes the colour of the LED orange
            }
            previousMillis = millis();
            break;
        }
        blinkState = 0;
      }
      glitch();
      Blink();
      microphone();
      matrix.write();
      if (millis() - previousMillis >= ledTime) {
        setColor(0, 0, 0);
        ledstate = 0;
      }
    }
    else {
      if ((millis() - boopTime) < 6000) {
        blinkTime = millis();
        if (boopActive == 0) {
          matrix.fillRect(EyeRight, 0, 16, 8, LOW);
          matrix.fillRect(EyeLeft, 0, 16, 8, LOW);
          matrix.drawBitmap(EyeLeft, 0, vwvL, 16, 8, HIGH);
          matrix.drawBitmap(EyeRight, 0, vwv, 16, 8, HIGH);
          setColor(16, 4, 0);
          boopActive = 1;
          rising = 1;
        }
      }
      else {
        boopTimer = 0;
      }
    }
    matrix.write();
  }
  else {
    matrix.fillScreen(LOW);
    matrix.write();
  }
}

///////////////////////////Button interrupt\\\\\\\\\\\\\\\\\\\\\\\\\\\

void ISR_button() {                               //Stuff you shouldn't touch :P
  if ((millis() - debounceTime) > 150) {
    if (state < expressions) {
      state++;
    }
    else {
      state = 0;
    }
    rising = 1;
    debounceTime = millis();
  }
}

///////////////////////////LED\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
