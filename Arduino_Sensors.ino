#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
// Define pin for shackle sensor
const int shackleSensorPin = 9;    // Shackle sensor pin

// Define pin for shackle relay
const int shackleRelayPin = 10;     // Shackle relay pin

// Flag to track if the shackle sensor has been triggered
bool shackleSensorTriggered = false;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Torque Sensor pins & setup:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin
int TarePin = 2; //For Tare Button
//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
unsigned long t = 0;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Set pin mode for shackle sensor
  pinMode(shackleSensorPin, INPUT);

  // Set pin mode for shackle relay
  pinMode(shackleRelayPin, OUTPUT);

  // Initialize shackle relay to OFF
  digitalWrite(shackleRelayPin, HIGH);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  pinMode(TarePin,INPUT);
  Serial.begin(57600); 
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = -33.76; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 3500; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Read shackle sensor input
  int shackleSensorState = digitalRead(shackleSensorPin);

  // If shackle sensor is triggered and it hasn't been triggered before, turn on the relay
  if (shackleSensorState == LOW && !shackleSensorTriggered) {
    digitalWrite(shackleRelayPin, LOW); // Turn on shackle relay
    shackleSensorTriggered = true; // Set flag to indicate sensor has been triggered
  }

  // If shackle sensor has been triggered and relay is on, turn off the relay
  if (shackleSensorTriggered && digitalRead(shackleRelayPin) == LOW) {
    digitalWrite(shackleRelayPin, HIGH); // Turn off shackle relay
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (digitalRead(TarePin) == HIGH) { // Assuming HIGH when activated
    LoadCell.tareNoDelay();}
   
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print(" ");
      Serial.println(i);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }
 // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println();
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
