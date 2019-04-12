/* 

Here are where pins and message information is defined for the dashboard. 
More information can be found in README.md.

An excel file mirrors this information and has the same title. 

Author: Chad McColm
Date:   12-Mar-2019

*/

// Input CAN message (from other CAN nodes)
#define megasquirt_gp0 1520   
#define megasquirt_gp2 1522
#define dash_input 100


// Output CAN message (to other CAN nodes)
#define dash_output 90
#define canOutputDelay 500

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
#define ledCount 53
#define flashDelay 50

#define maxRpm 12300.0
#define flashRpm 11500.0
#define coolantCold 165
#define coolantHot 190

#define amsLed 0
#define imdLed 1
#define coolantTemperatureLed 2

#define stateOfChargeStart 45
#define stateOfChargeLength 8

#define rpmStart 3
#define rpmLength 42
