#define INTCAN_PIN 2
const int SPI_CS_PIN = 9;
#define CAN_DASH_OUT_ID 0x90   // Dash to controller
#define STATUS_ID 0x81    // IMD status, AMS status
#define RTD_ID 0x80    // rtds from main controller (only bit 3)
#define RPM_CHARGE_ID 0x257
#define TEMP_ID 0x258 // coolant temp in here


#define LED_COUNT 60

// Outputs to main controller (ID 0x90)
#define DRIVEMODE_BYTE 0            
#define DRIVEMODE_BIT 0             // 3 bits
#define DRIVEMODE_PIN C1

#define TRACTION_BYTE 0
#define TRACTION_BIT 3              // 3 bits
#define TRACTION_PIN C0

#define CPBRB_BYTE 1
#define CPBRB_BIT 0
#define CPBRB_PIN 10

#define MOTOR_BTN_BYTE 1
#define MOTOR_BTN_BIT 1
#define MOTOR_BTN_PIN 6

#define ENGINE_BTN_BYTE 1
#define ENGINE_BTN_BIT 2
#define ENGINE_BTN_PIN A4

// Internal dash outputs
#define MOTOR_LED_PIN 6

#define ENGINE_LED_PIN A4

#define LED_PIN 3

//Inputs from pdu (ID 0x81)

#define IMD_STAT_BYTE 0
#define IMD_STAT_BIT 0

#define AMS_STAT_BYTE 0
#define AMS_STAT_BIT 1

//Inputs from controller (ID 0x80)
#define RTDS_BYTE 0
#define RTDS_BIT 3
#define RTDS_PIN 5

//Inputs from controller  (ID 0x257)
#define RPM_BYTE 2 
#define RPM_LEN 16 //16 bits (2 bytes) 
#define CHARGE_BYTE 6
#define CHARGE_LEN 8 //8 bits (1 byte)

//Inputs from controller  (ID 0x258)
#define TEMP_BYTE 7
#define TEMP_LEN 8 //8 bits (1 byte)

#define MASK_1 0x1
#define BIT_2_MASK 0x2
#define BIT_3_MASK 0x4
#define BIT_4_MASK 0x8
#define BIT_5_MASK 0x10
#define BIT_6_MASK 0x20
#define BIT_7_MASK 0x40
#define BIT_8_MASK 0x80
