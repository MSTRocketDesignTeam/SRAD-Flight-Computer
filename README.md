<p align="center">
    <img src="assets/images/RDT_LOGO.png" width="60%" alt="Missouri S&T Rocket Design Team">
</p>

<p align="center"><sup><em>*This repo is intended for educational purposes only*</em></sup></p>

# SRAD Flight Computer
A student researched and designed (SRAD) rocket flight computer. 

## About 
This project is the spiritual sucessor of the Rocket Design Team's (RDT) modular SRAD altimeter system.
It aims to reduce complexity and overhead by doing away with the modular design and instead focusing on a single board. This will eliminate the difficulty in interfacing independent microcontrollers. By minimizing the area of the board it is anticipated that it may be flown on many more nonspecialized rockets, in an effort to build up flight data for verification. 

<em><strong>Once this platform has adequately matured, it is intended to be a viable parachute deployment system for S&T's Rockets.</strong></em>

## Documentation
*placeholder*

## Development 
*placeholder* 

## File Structure
This repo contains three main sections, hardware, firmware, and software.

### Hardware:
The hardware directory will contain all of the files related to the design and assembly of the PCBs. This includes KiCad projects as well as BOMs. 

### Firmware:
The firmware directory will contain all of the code (C++) necessary to operate the above hardware. Once the boards have been designed and assembled this is where significant effort will be made.

### Software: 
The software directory will contain the code (Python) for the flight computer's computer interface. This program will be responsible for configuring the flight computer prior to launch as well as for displaying live flight data as received over the telemetry radio. 

Future developments may allow for preflight simulations (generic / CFD drag) as well as for a higher fidelity landing estimate during the flight. 



