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
int mode;
int tractionControl;
bool engineStart = 1;
bool motorStart = 1;
bool cockpitShutdown = 1;

// Global variables for debouncing inputs
bool lastEngineStartReading;
bool lastMotorStartReading;
bool lastCockpitShutdownReading;
unsigned long lastEngineStartReadingTime;
unsigned long lastMotorStartReadingTime;
unsigned long lastCockpitShutdownReadingTime;

// CAN frequency tracking
unsigned long canLastSent;

// Flash frequency tracking
unsigned long flashMode;

// This function runs on setup
void setup()
{
  
  // Initialize the led strip, set them all to off, and update the strip
  leds.begin();
  
  // Set all LEDs to off initially 
  for (int i = 0; i < ledCount; i++){
    leds.setPixelColor(i, 0);
  }
  leds.show();
  
  // Start up a serial interface in case any data needs to be sent to the connected PC
  Serial.begin(115200);

  // Initialize the CAN bus and stick into an error loop if that is not possible
  while (CAN_OK != CAN.begin(CAN_500KBPS)){
    Serial.println("CAN BUS Initialization Failed");
    delay(100);
  }
  Serial.println("CAN BUS Initialized OK!");
  
  // By default, both masks are fully open and no identifier bits are masked
  // Set the hardware filters to only look for the expected input id
  CAN.init_Filt(0, 0, canInputId);
  CAN.init_Filt(1, 0, canInputId);       
  CAN.init_Filt(2, 0, canInputId);
  CAN.init_Filt(3, 0, canInputId);
  CAN.init_Filt(4, 0, canInputId);
  CAN.init_Filt(5, 0, canInputId);

  pinMode(engineStartPin, INPUT_PULLUP);
  pinMode(motorStartPin, INPUT_PULLUP);
  pinMode(cockpitShutdownPin, INPUT_PULLUP);
  
}

void loop(){
  
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
      
      // Double check that the filter got the right ID before saving the data
      if(incomingMessageId == canInputId){
        
        // Process the message into the global input variables
        rpm = incomingMessageData[1] << 8 | incomingMessageData[0];
        coolantTemperature = (incomingMessageData[3] << 8 | incomingMessageData[2])/10.0;
        stateOfCharge = incomingMessageData[4];
        engineRunning = incomingMessageData[5] & 0b00000001;
        readyToDrive = incomingMessageData[5] & 0b00000010;
        buzzerOn = incomingMessageData[5] & 0b00000100;
        amsStatus = incomingMessageData[5] & 0b00001000;
        imdStatus = incomingMessageData[5] & 0b00010000;
        
      }

    }
    
  
    // Update the analog inputs into the global input variables
    mode = round(analogRead(modePin)*5/1024.0);
    tractionControl = round(analogRead(tractionControlPin)*5/1024.0);

//    // Get the current button state
    int engineStartReading = digitalRead(engineStartPin);
    int motorStartReading = digitalRead(motorStartPin);
    int cockpitShutdownReading = digitalRead(cockpitShutdownPin);
    
    // If the reading has changed, reset the debounce timer
    if(engineStartReading != lastEngineStartReading)
      lastEngineStartReadingTime = millis();
    if(motorStartReading != lastMotorStartReading)
      lastMotorStartReadingTime = millis();
    if(cockpitShutdownReading != lastCockpitShutdownReading)
      lastCockpitShutdownReadingTime = millis();
    
    // If the reading has been steady for longer than the delay, save that to the global variables
    if ((millis() - lastEngineStartReadingTime) > debounceDelay)
      engineStart = engineStartReading;
    if ((millis() - lastMotorStartReadingTime) > debounceDelay)
      motorStart = motorStartReading;
    if ((millis() - lastCockpitShutdownReadingTime) > debounceDelay)
      cockpitShutdown = cockpitShutdownReading;

    // Update the last reading
    lastEngineStartReading = engineStartReading;
    lastMotorStartReading = motorStartReading;
    lastCockpitShutdownReading = cockpitShutdownReading;
  
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
    
    // Check if the bar should be flashing
    if(rpm > flashRpm)
      flashMode = millis();
    else
      flashMode = flashDelay;

    // Loop through the bar's LEDs and set their color as required
    int j;
    for(j = 0; j < rpmLength; j++){
      if(j < rpmRatio*rpmLength){
        int flash = (flashMode / flashDelay) % 2;
        leds.setPixelColor(j + rpmStart, HSBtoRGB(rpmColor*flash,1,1));
      }
      else
        leds.setPixelColor(j + rpmStart, 0);
    }
    
    // Update the led changes just made
    leds.show();
  
  // Update all CAN outputs
  
    // Check if it's been long enough since the last CAN message was sent
    if(millis()-canLastSent > canDelay){
      
      // Build a new message
      uint8_t canMessage[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      canMessage[0] = mode;
      canMessage[1] = tractionControl;
      canMessage[2] = !engineStart;
      canMessage[3] = !motorStart;
      canMessage[4] = !cockpitShutdown;
      
      // Send the message
      CAN.sendMsgBuf(canOutputId, 0, 8, canMessage);
      
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
