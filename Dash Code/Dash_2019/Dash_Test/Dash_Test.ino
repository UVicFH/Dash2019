// This is the tester file written for the Dash
// Original author: Chad J. McColm
// Editor: Tylynn Haddow
#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 9;
int ID = 100;

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
    CAN.sendMsgBuf(ID, 0, 8, data);
    // Serial.println("Message Sent");
    delay(100);
  }
  
}

void loop()
{
  // Create a placeholder for the message to be sent
  byte message[8] = {0,0,0,0,0,0,0,0};
 
  // Cycle power to buzzer, motor LED, and engine Led for 2 seconds
  message[5] = 0b00000001;
  Serial.println("engine LED");
  two_second_send(message);

  message[5] = 0b0;
  Serial.println("off");
  two_second_send(message);

  message[5] = 0b00000010;
  Serial.println("motor LED");
  two_second_send(message);
//
//  message[5] = 0b00000100;
//  Serial.println("buzzer");
//  two_second_send(message);


  message[1] = 0b01111111;
  message[2] = 0b00011111;
  message[3] = 0b00011111;
  message[4] = 0b00011111;
  message[5] = 0b00011000;
  Serial.println("clt and rpm");
  two_second_send(message);

  message[5] = 0b0;
  message[1] = 0b0;
  message[2] = 0b0;
  message[3] = 0b0;
  Serial.println("off");
  two_second_send(message);


}
