  /*

Date:   12-Mar-2019
Author: Chad McColm

See README.md for more details

*/

// Include external library files
#include "Adafruit_NeoPixel.h"
#include "WS2812_Definitions.h"
#include <SPI.h>
#include <math.h>
#include "mcp_can.h"
#include <stdbool.h>

// Include internal definition library
#include "defines.h"

// Activate the CAN interface on its SPI select pin
MCP_CAN CAN(spiCsPin);

// Create an instance of the Adafruit_NeoPixel class
Adafruit_NeoPixel leds = Adafruit_NeoPixel(ledCount, ledPin, NEO_GRB + NEO_KHZ800);

// Global variables for input storage (global scope variables are initialized to 0)
int rpm;
float coolantTemperature;
int stateOfCharge;
bool engineRunning;
bool readyToDrive;
bool buzzerOn = 0;
bool amsStatus;
bool imdStatus;
bool launchActivated;
int mode;
int tractionControl;
bool engineStart = 1;
bool motorStart = 1;

// Global variables for debouncing inputs
bool lastEngineStartReading;
bool lastMotorStartReading;
unsigned long lastEngineStartReadingTime;
unsigned long lastMotorStartReadingTime;

// CAN frequency tracking
unsigned long canLastSent;

// Flash frequency tracking
unsigned long flashMode;

// DEMO mode variables
int temp_counter = 0;
int temp_counterrpm = 0;
int temp_dir = 1;
unsigned long LedClearTime;
int rpm_hold = 500;
unsigned long rpmHoldTime;
int temp_dirRpm = 1;

// This function runs on setup
void setup()
{

  // Initialize the led strip, set them all to off, and update the strip
  leds.begin();

  // Set all LEDs to off initially
  clearLEDs();
  leds.show();

  // Start up a serial interface in case any data needs to be sent to the connected PC
  Serial.begin(115200);

  // Initialize the CAN bus and stick into an error loop if that is not possible
  while (CAN_OK != CAN.begin(CAN_500KBPS)){
    Serial.println("CAN BUS Initialization Failed");
    delay(100);
  }
  Serial.println("CAN BUS Initialized OK!");

  pinMode(engineStartPin, INPUT_PULLUP);
  pinMode(motorStartPin, INPUT_PULLUP);

}

void loop(){

  if ((millis()-LedClearTime) >= 5000){
    clearLEDs();
    leds.show();
    LedClearTime = millis();
  }
  
  // DEMO mode
  if ((mode == 6) && (tractionControl == 6)){
    if (temp_dir) {
      temp_counter += 20;
    }
    else {
    temp_counter -= 20;
    }
    if (temp_dirRpm){
        temp_counterrpm += 100;
    } 
    else {
    temp_counterrpm -= 100;
    }
    if ( rpmHoldTime && ((millis()-rpmHoldTime) >=rpm_hold) ) {
      rpmHoldTime = 0;
      temp_dirRpm = 0;
    }
    
    if (temp_counterrpm >13000){
    temp_counterrpm = 13000;
    if (!rpmHoldTime) rpmHoldTime = millis();
    }
    if (temp_counterrpm <2000){
    temp_counterrpm = 2000;
    }
    // Process the message into the global input variables
    rpm = map(temp_counterrpm, 0, 13000, 0, 13000);
    coolantTemperature = map(temp_counter, 0, 13000, 0, 200);
    stateOfCharge = map(temp_counter, 0, 13000, 0, 100);
    engineRunning = 0;
    readyToDrive = 0;
    buzzerOn = 0;
    amsStatus = 0;
    imdStatus = 0;

    if (temp_counter>13000)
    {
      temp_dir =0;
    }
    else if (temp_counter<0)
    {
      temp_dir =1;
      temp_dirRpm = 1;
    }

  }
  // NORMAL mode
  else{
    // Update all inputs
    // Check if there is a CAN message available on the bus
    if(CAN_MSGAVAIL == CAN.checkReceive()){

      // Create a place to store the incoming CAN message
      unsigned char incomingMessageLength = 0;
      unsigned char incomingMessageData[8];
      unsigned long incomingMessageId = 0;

      // Save the CAN message and id
      CAN.readMsgBuf(&incomingMessageLength, incomingMessageData);
      incomingMessageId = CAN.getCanId();

      // Store things sent from MCU
      if(incomingMessageId == canInputIdMCU){

        // Process the message into the global input variables
        stateOfCharge = incomingMessageData[0];
        engineRunning = incomingMessageData[1] & 0b00000001;
        readyToDrive = (incomingMessageData[1] & 0b00000010) >> 1;
        buzzerOn = (incomingMessageData[1] & 0b00000100) >> 2;
        amsStatus = (incomingMessageData[1] & 0b00001000) >> 3;
        imdStatus = (incomingMessageData[1] & 0b00010000) >> 4;
        launchActivated = (incomingMessageData[1] & 0b00100000) >> 5;

      }

      // Store things sent from Microsquirt (1)
      if(incomingMessageId == canInputIdRPM){

        // Process the message into the global input variables
        rpm = incomingMessageData[6] << 8 |incomingMessageData[7];

      }

      // Store things sent from Microsquirt (2)
      if(incomingMessageId == canInputIdCLT){

        // Process the message into the global input variables
        coolantTemperature = (incomingMessageData[6] << 8 | incomingMessageData[7])/10.0;

      }

    }
  }

  // If launch control is active then override the RPM bar with max to make it flash
  if(launchActivated){
    rpm = 12200;
  }

  // Update driver inputs

    // Update the analog inputs into the global input variables
    mode = round(analogRead(modePin)*5/1024.0)+1;
    tractionControl = round(analogRead(tractionControlPin)*5/1024.0)+1;

    // Get the current button state
    int engineStartReading = digitalRead(engineStartPin);
    int motorStartReading = digitalRead(motorStartPin);

    // If the reading has changed, reset the debounce timer
    if(engineStartReading != lastEngineStartReading)
      lastEngineStartReadingTime = millis();
    if(motorStartReading != lastMotorStartReading)
      lastMotorStartReadingTime = millis();

    // If the reading has been steady for longer than the delay, save that to the global variables
    if ((millis() - lastEngineStartReadingTime) > debounceDelay)
      engineStart = engineStartReading;
    if ((millis() - lastMotorStartReadingTime) > debounceDelay)
      motorStart = motorStartReading;

    // Update the last reading
    lastEngineStartReading = engineStartReading;
    lastMotorStartReading = motorStartReading;

  // Set all LED/buzzer outputs

    // Set the engine button light
    digitalWrite(engineLedPin, engineRunning);
    //if(engineRunning) Serial.println("engineRunning");

    // Set the motor button light
    digitalWrite(motorLedPin, readyToDrive);
    //if(readyToDrive) Serial.println("readyToDrive");

    // Set the buzzer to the inputted value
    digitalWrite(buzzerPin, buzzerOn);

    // Set the AMS Status LED
    leds.setPixelColor(amsLed, leds.Color(!amsStatus*255, 0, 0));

    // Set the IMD Status LED
    leds.setPixelColor(imdLed, leds.Color(!imdStatus*255, 0, 0));

    // Set the engine temperature LED based on temperature. Normal is off, low is green, hot is red
    if(coolantTemperature < coolantCold)
      leds.setPixelColor(coolantTemperatureLed, leds.Color(28, 135, 229));
    else if(coolantTemperature > coolantHot)
      leds.setPixelColor(coolantTemperatureLed, leds.Color(255, 0, 0));
    else
      leds.setPixelColor(coolantTemperatureLed, leds.Color(0, 0, 0));

    // Determine the correct color for the state of charge bar
    int stateOfChargeColor = round(stateOfCharge);

    // Loop through the bar's LEDs and set their color as required
    int i;
    for(i = 0; i < stateOfChargeLength; i++){
      if(i < stateOfCharge/100.0*stateOfChargeLength)
        leds.setPixelColor(i + stateOfChargeStart, HSBtoRGB(stateOfChargeColor,1,1));
      else
        leds.setPixelColor(i + stateOfChargeStart, 0);
    }

    // Determine the correct color for the state of RPM bar
    float rpmRatio = constrain(rpm/maxRpm, 0, 1);
    int rpmColor = round((1-rpmRatio)*120.0);

    // Loop through the bar's LEDs and set their color as required
    int j;
    int flasher = 1;
    
    if (rpm >= flashRpm){
      if ((millis()-flashMode) >= flashDelay){
        flasher = !flasher;
        flashMode = millis();
      }
      if (flasher){
        for(j = 0; j < rpmLength; j++){
           leds.setPixelColor(j + rpmStart, 0);
        }
      }
      else {
        for(j = 0; j < rpmLength; j++){
          if(j < rpmRatio*rpmLength){
            int flash = (flashMode / flashDelay) % 2;
            leds.setPixelColor(j + rpmStart, HSBtoRGB(rpmColor,1,1));
          }
          else
            leds.setPixelColor(j + rpmStart, 0);
          }
        }
    } else {
      for(j = 0; j < rpmLength; j++){
          if(j < rpmRatio*rpmLength){
            int flash = (flashMode / flashDelay) % 2;
            leds.setPixelColor(j + rpmStart, HSBtoRGB(rpmColor,1,1));
        }
        else
          leds.setPixelColor(j + rpmStart, 0);
        }
    }

  // Update the led changes just made
    Serial.println("RPM");
    Serial.println(rpm);
    leds.show();
    
  // Update all CAN outputs

    // Check if it's been long enough since the last CAN message was sent
    if(millis()-canLastSent > canDelay){

      // Build a new message
      uint8_t canMessage;
      canMessage = mode | tractionControl << 3 | !engineStart << 6 | !motorStart << 7;

      // Send the message
      CAN.sendMsgBuf(canOutputId, 0, 1, &canMessage);

      // Update the last send time
      canLastSent = millis();
    }

}

long HSBtoRGB(float _hue, float _sat, float _brightness) {
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;

  if (_sat == 0.0) {
    red = _brightness;
    green = _brightness;
    blue = _brightness;
  } else {
    if (_hue == 360.0) {
      _hue = 0;
    }

    int slice = _hue / 60.0;
    float hue_frac = (_hue / 60.0) - slice;

    float aa = _brightness * (1.0 - _sat);
    float bb = _brightness * (1.0 - _sat * hue_frac);
    float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));

    switch(slice) {
      case 0:
        red = _brightness;
        green = cc;
        blue = aa;
        break;
      case 1:
        red = bb;
        green = _brightness;
        blue = aa;
        break;
      case 2:
        red = aa;
        green = _brightness;
        blue = cc;
        break;
      case 3:
        red = aa;
        green = bb;
        blue = _brightness;
        break;
      case 4:
        red = cc;
        green = aa;
        blue = _brightness;
        break;
      case 5:
        red = _brightness;
        green = aa;
        blue = bb;
        break;
      default:
        red = 0.0;
        green = 0.0;
        blue = 0.0;
        break;
    }
  }

  long ired = red * 255.0;
  long igreen = green * 255.0;
  long iblue = blue * 255.0;

  return long((ired << 16) | (igreen << 8) | (iblue));

}

void clearLEDs()
{
  for (int i=0; i<ledCount; i++)
  {
    leds.setPixelColor(i, 0);
  }
}
