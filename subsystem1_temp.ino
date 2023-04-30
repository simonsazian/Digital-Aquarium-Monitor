// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

#include <LiquidCrystal.h>

int LEDRState = 0;
int LEDRPin= 6;

int LEDGState = 0;
int LEDGPin = 5;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 9, en = 8, d4 = 2, d5 = 3, d6 = 4, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//LiquidCrystal_I2C lcd(0x15, 16, 2); //The LCD address and size. You can change according you yours //changes here

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 11 //pin for sensor

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int counter = 0;
String b;

/*
   The setup function. We only start the sensors here
*/
void setup()
{
  // start serial port
  Serial.begin(115200);
  lcd.begin(16, 2);

  // Start up the library
  sensors.begin();

  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Ready to measure");
  lcd.setCursor(0, 1);
  lcd.print("temperature (C/F)");
  delay(3000);
  lcd.clear();
}

/*
   Main function, get and show the temperature
*/
void loop()
{
  if (Serial.available() > 0) { //***************************************
    int byte = Serial.read();
    counter++;  
  
    Serial.end();
    Serial.begin(115200);

    delay(3000);
  }

  
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures

  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {

    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print(tempC);
    lcd.print((char)223);
    lcd.print("C");
    lcd.print(" | ");
    lcd.print(DallasTemperature::toFahrenheit(tempC));
    lcd.print(" F");

    // INVALID BOUNDS
    if (tempC < 23 || tempC > 28 || ((counter % 2) != 0)) {
      digitalWrite(LEDGPin,LOW);
      digitalWrite(LEDRPin,HIGH);
    } 
    // VALID
    else{
      digitalWrite(LEDGPin,HIGH);
      digitalWrite(LEDRPin,LOW);
    }

  }
}