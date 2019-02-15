// This is the tester file written for the PDU
// Can be modified for use with the dash
#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 9;
int ID = 0x90;

MCP_CAN CAN(SPI_CS_PIN);

void setup()
{
    Serial.begin(9600);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

// Sends a 3 byte long array of data to the system for two seconds
void two_second_send(byte * data){
  
  for(int i = 0; i < 20; i++){
    CAN.sendMsgBuf(0x50, 0, 3, data);
    // Serial.println("Message Sent");
    delay(100);
  }
  
}

void loop()
{
  // Create a placeholder for the message to be sent
  byte message[3] = {0,0,0};
//
//  Serial.println("Blank Message Initialized!");
//  
  // Cycle power to each shifter, the starter, the spare, the ready to go, the microsquirt, the fuel pump, and brake light for 2 seconds
  message[0] = 0b10000000;
  Serial.println("Shift Down!");
  two_second_send(message);

  message[0] >>= 1;
  Serial.println("Shift Up!");
  two_second_send(message);

  message[0] >>= 1;
  Serial.println("Starter!");
  two_second_send(message);

  message[0] >>= 1;
  Serial.println("Spare1!");
  two_second_send(message);

  message[0] >>= 2;
  Serial.println("MicroSquirt!");
  two_second_send(message);

  message[0] >>= 1;
  Serial.println("Fuel Pump!");
  two_second_send(message);

  message[0] >>= 1;
  Serial.println("Brake Light!");
  two_second_send(message);
//  
//  // Turn everything off
//  message[0] = 0b0;
//  two_second_send(message);
//
  // Ramp up the engine fan and bring it back down again in two seconds
//  Serial.println("Fan Up!");
//  for(int i = 0; i < 160; i++){
//    message[2] = i;
//    CAN.sendMsgBuf(ID, 0, 3, message);
//    delay(16);
//  }
//  Serial.println("Fan Down!");
//  for(int i = 160; i >=0; i--){
//    message[2] = i;
//    CAN.sendMsgBuf(ID, 0, 3, message);
//    delay(16);
//  }
//
//  message[2]=160;
//  Serial.println("Fan on");
//  two_second_send(message);
//
//  delay(10000);
//
//  message[2] = 0;
//  Serial.println("Fan off");
//  two_second_send(message);
//
//  delay(6000);
  

}
