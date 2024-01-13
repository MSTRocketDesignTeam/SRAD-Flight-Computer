# Serial Speed Test
This is a basic python program designed to test the throughput of the serial connection
of a connection microcontroller and do basic packet error checking. 

## Testing TX (uC -> PC):
### uC: 
Microcontroller should continously send bytes, each one incrementing one from the last. 
When the max 8 bit byte value of 255 is reached, roll over to 0. 