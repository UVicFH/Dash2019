/* 

Here are where pins and message information is defined for the dashboard. 
More information can be found in README.md.

An excel file mirrors this information and has the same title. 

Author: Chad McColm
Date:   12-Mar-2019

*/

// Input CAN message
#define canInputId 0x100

// Output CAN message
#define canOutputId 0x90
#define canDelay 50

// Physical pins
#define spiCsPin 9
#define tractionControlPin A0
#define modePin A1
#define ledPin 3
#define engineStartPin 4
#define buzzerPin 5
#define motorStartPin 6
#define motorLedPin 7
#define engineLedPin 8
#define cockpitShutdownPin 10

// Physical pin debouncing delay
#define debounceDelay 50

// LED Specifications
#define ledCount 60
#define flashDelay 50

#define maxRpm 12300.0
#define flashRpm 11500.0
#define coolantCold 165
#define coolantHot 190

#define amsLed 0
#define imdLed 1
#define coolantTemperatureLed 2

#define stateOfChargeStart 3.0
#define stateOfChargeLength 10.0

#define rpmStart 3.0
#define rpmLength 15.0
