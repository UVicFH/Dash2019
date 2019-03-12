# Dashboard

This code runs the dashboard of the UVH19 vehicle. It takes the following message over CAN (bytes are in order starting at 0):

| Message                | Bytes |
|------------------------|-------|
| RPM                    | 2     |
| 10*Coolant Temperature | 2     |
| SOC                    | 1     |
| Engine Running         | 0.5   |
| RTD                    | 0.5   |
| Buzzer On              | 1     |
| AMS Status             | 0.5   |
| IMD Status             | 0.5   |

This message should be sent at 5Hz from the supervisory controller and the message ID is defined in the defines.h file. 

It takes input from three buttons (engine start, motor start, and cockpit shutdown) and two analogs (Traction Control and Mode). These are indexed analog inputs.

It outputs to WS2812 addressable LED lights in 5 parts as shown below: 

| Name               | Length |
|--------------------|--------|
| AMS Status         | 1      |
| IMD Status         | 1      |
| Engine Temperature | 1      |
| RPM	               |        |
| Charge             | 	      |


It also outputs the physical inputs over CAN in the following format at 5Hz:

| Message            | Bytes | Format      |
|--------------------|-------|-------------|
| Mode               | 1     | Indexed 0-5 |
| Traction   Control | 1     | 0-100       |
| Engine Start       | 1     | 0/1         |
| Motor Start        | 1     | 0/1         |
| CPBRB Status       | 1     | 0/1         |