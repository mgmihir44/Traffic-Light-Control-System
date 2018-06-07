# Traffic-Light-Control-System

- Copyright - UT Austin, Jonathan Valvano, Daniel Valvano, Ramesh Yerraballi
- Date: January 15, 2016

Lab 10 of UT Austin's course on edx.org taught by Dr. Jonatha Valvano and Dr. Ramesh Yerraballi.
File provided: tm4c123gh6pm.h, startup.S

- Author: Mihir Garude
- Date: June 7th, 2018

This is a simple system that miminc a traffic control system at an intersection. Apart from EnableInterrupts(),
DisableInterrupts(), and TExaS_Init() all other functions and code is implemented by me. You are free
to use the code as long as you mention the copyrights and my name as one of the authors.

Assumption:
- Two one way sections intersect each other at one junction, east/west and south/norht.
- Senors(Switches in this Lab) are used to detect traffic from either direction.
- There is additional pedestrian sensor(Switch) which is used for pedestrian crossing.

Working:
A Moore FSM is used for transition between different states. In the Initial State, Cars going in South/North
direction are allowed to pass. Based on the input, the System will go to another state or will remain in the current state.
The System will wait in a particular state for a predefined period. The predefined delay is executed
using the NVIC SysTick Timer
Cars from only one direction can pass the intersection - either from North/South or East/West.
Transition from Green to Redd follows the Pattern - Green - Yellow - Red.
Whenever, the walk pedestrian light is Green, traffic from both directions will come to a halt.

Limitation:
- The System in no interrupt driven.
- The System does not use BITs to Set and Clear Signals and get input from Switches, uses the entire Port.

[Video](https://youtu.be/gMDjTUCRp6U)
