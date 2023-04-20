/*
Code written by Simon Sazian in Spring 2023 for CS 362
Digital Aquarium Monitor- Subsystem 2: Ammonia/nitrite sensor

The purpose of the code is to detect and output the nitrite or ammonia parts per million (ppm) of
chemically treated aquarium water from a test tube placed on top of a color sensor. 

Using Adafruit TCS 34725 RGB Color sensor
  Connect SCL    to Arduino SCL
  Connect SDA    to Arduino SDA
  Connect VIN    to 5V DC
  Connect GROUND to common ground GND 

Pin numbers for LCD:
  VSS(GND): Connects to GND
  VDD(+5V): Power Source
  RS: Data pin 9
  RW: Connected to GND
  E(Enable): Data pin 8
  D4: Data pin 2
  D5: Data pin 3
  D6: Data pin 4
  D7: Data pin 7
  A(BKlt+): Connected to VDD via 220 Ohm resistor
  K(BKlt-): Connected to GND
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Declare the color sensor object
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);

// Declare and define the lcd screen
const int rs = 9, en = 8, d4 = 2, d5 = 3, d6 = 4, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Define constant values for all possbile nitrite and ammonia values
// Reference array of all known ammonia values
const int ammonia_references[7][3] = {{87, 101, 78}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
// Values stored in order: 0 ppm, 0.25 ppm, 0.5 ppm, 1 ppm, 2 ppm, 4 ppm, 8 ppm

// Reference array of all known nitrate values
const int nitrite_references[6][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
// Values stored in order: 0 ppm, 0.25 ppm, 0.5 ppm, 1 ppm, 2 ppm, 5 ppm


// button values to determine whether to read ammonia or nitrite
int ammoniaButtonPin=5;
int ammoniaButtonNew;
int ammoniaButtonOld = 1;

int nitriteButtonPin = 6;
int nitriteButtonNew;
int nitriteButtonOld = 1;

// global var for communication
int counter = 0;

void setup() {
  // Begin a serial monitor
  Serial.begin(9600);

  // Setup an LCD screen's number of columns and rows:
  lcd.begin(16, 2);
  
  // Ensure that the sensor is found
  if (tcs.begin()) {
    Serial.println("Found sensor");

    // Print a starting message to the LCD.
    set_lcd();
  } 
  else {
    // the color sensor was not found, display error
    Serial.println("No TCS34725 found, check connections");
    lcd.setCursor(0, 0);
    lcd.print("Sensor not found");
    lcd.setCursor(0, 1);
    lcd.print("Restart program");

    while (1);  // freeze program if sensor is not found
  }
}
 

void loop() {
  // declare variables to hold RGB and clear values
  uint16_t red, green, blue, clear;
  
  // check whether to measure ammonia or nitrite

  // if button to measure ammonia is read
  ammoniaButtonNew=digitalRead(ammoniaButtonPin);
  if (ammoniaButtonOld==0 && ammoniaButtonNew==1) {
    // begin taking 20 measurements to find the average
    int redSum = 0, greenSum = 0, blueSum = 0;

    // values passed by reference to be updated
    take_readings(redSum, greenSum, blueSum);

    // now, calculate the average reading for each value
    red = redSum/20;
    green = greenSum/20;
    blue = blueSum/20;

    // calculate the ammonia reading
    int ammonia_value = measure_value(ammonia_references, 7, red, green, blue);

    // output the found value
    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Ammonia reading:");
    lcd.setCursor(0, 1);

    // check which value was read, and output accordingly    
    if (ammonia_value == 0) {
      lcd.print("0 ppm");

      // Write to serial communication that ammonia levels are safe
      if ((counter % 2) != 0) {
        // when LED changes from red -> green
        Serial.write((char)1);
        lcd.println(" SENT");
        counter++;
      }
      delay(5000);
    }
    else if (ammonia_value == 1) {
      lcd.print("0.25 ppm");

      // Write that levels are unsafe
      if ((counter % 2) == 0) {
        // when LED changes from green -> red
        Serial.write((char)1);
        lcd.println(" SENT");
        counter++;
      }
      delay(5000);
    }
      else if (ammonia_value == 2) {
        lcd.print("0.5 ppm");
        // Write that levels are unsafe
        if ((counter % 2) == 0) {
          // when LED changes from green -> red
          Serial.write((char)1);
          counter++;
        }
      }
      else if (ammonia_value == 3) {
        lcd.print("1 ppm");
        // Write that levels are unsafe
        if ((counter % 2) == 0) {
          // when LED changes from green -> red
          Serial.write((char)1);
          counter++;
        }
      }
      else if (ammonia_value == 4) {
        lcd.print("2 ppm");
        // Write that levels are unsafe
        if ((counter % 2) == 0) {
          // when LED changes from green -> red
          Serial.write((char)1);
          counter++;
        }
      }
      else if (ammonia_value == 5) {
        lcd.print("4 ppm");
        // Write that levels are unsafe
        if ((counter % 2) == 0) {
          // when LED changes from green -> red
          Serial.write((char)1);
          counter++;
        }
      }
      else if (ammonia_value == 3) {
        lcd.print("8 ppm");
        // Write that levels are unsafe
        if ((counter % 2) == 0) {
          // when LED changes from green -> red
          Serial.write((char)1);
          counter++;
        }
      }
      else {
        lcd.print("Error, please try again");
      }
    
    // Pause for a few seconds for readability
    delay(6000); 

    // Reset the monitor
    set_lcd();
  }
  // reset ammonia button
  ammoniaButtonOld=ammoniaButtonNew;


  // now, check whether the nitrite button was pushed
  /*
  nitriteButtonNew=digitalRead(nitriteButtonPin);
  if (nitriteButtonOld==0 && nitriteButtonNew==1) {
    // begin taking 20 measurements to find the average
    int redSum = 0, greenSum = 0, blueSum = 0;

    // values passed by reference to be updated
    take_readings(redSum, greenSum, blueSum);

    // calculate the average reading for each value
    red = redSum/20;
    green = greenSum/20;
    blue = blueSum/20;

    // calculate the ammonia reading based on average values
    int nitrite_value = measure_value(nitrite_references, 6, red, green, blue);

    Serial.print("Nitrite value: ");
    Serial.println(nitrite_value);

    // output the found value
    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Nitrite reading:");
    lcd.setCursor(0, 1);

    // check which value was read, and output accordingly    
    switch(nitrite_value) {
      case 0:
        lcd.print("0 ppm");
        Serial.write("1");
        break;
      case 1:
        lcd.print("0.25 ppm");
        Serial.write((char)1);  // serial communcation
        break;
      case 2:
        lcd.print("0.5 ppm");
        Serial.write((char)1);
        break;
      case 3:
        lcd.print("1 ppm");
        Serial.write((char)1);
        break;
      case 4:
        lcd.print("2 ppm");
        Serial.write((char)1);
        break;
      case 5:
        lcd.print("5 ppm");
        Serial.write((char)1);
        break;
      default:
        lcd.print("Error, please try again");
        Serial.write((char)1);
        break;
    }
    
    // Pause for a few seconds for readability
    delay(6000); 

    // Reset the monitor
    set_lcd();
  }
  // reset nitrite button
  nitriteButtonOld = nitriteButtonNew;
  */

}

// Function to take the average of 20 readings from the color sensor
// Values are summed to their averages, and return the average reading
void take_readings(int &redSum, int &greenSum, int &blueSum) {
  // local variables for RGB values
  int red, green, blue, clear;

  for (int i = 0; i < 20; i++) {
    // change display to show that we are to read measurements
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Taking ");
    lcd.print(i+1);
    lcd.print("/20");
    lcd.setCursor(0, 1);
    lcd.print("readings...");      

    tcs.getRawData(&red, &green, &blue, &clear);

    // Map RGB values based on the light intensity
    // Redefine scale to be 0-256
    red = map(red, 0, clear, 0, 256);
    green = map(green, 0, clear, 0, 256);
    blue = map(blue, 0, clear, 0, 256);

    // add to the sums of each
    redSum = redSum + red;
    greenSum = greenSum + green;
    blueSum = blueSum + blue;

    delay(500); // delay for readability
  }
}

// Helper function to find the closest known reference value
// We take in RGB values as integers to represent the sensors read values
// Using a simple distance formula to find the closest reference value,
// we will return an integer signifying the index of the closest found value.
int measure_value(int arr[][3], int length, int red, int green, int blue) {
  // set a minimum value, to be updated
  int min = INT8_MAX;
  int value;

  // loop through all known reference
  for (int i = 0; i < length; i++) {
    // measure distance between given RGB values and all references
    double distance = sqrt((red - arr[i][0])^2 + (green - arr[i][1])^2 + (blue - arr[i][2])^2);

    // if a closer reference was found
    if (distance < (double) min) {
      // update values
      min = (int) distance;
      value = i;
    }
  }
  // once all values have been checked, return the closest found value
  return value;
}

// Simple function to set LCD screen
void set_lcd() {
  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to measure");
    lcd.setCursor(0, 1);
    lcd.print("Ammonia/Nitrite");
}
