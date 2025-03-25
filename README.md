# Fluid Bed Coffee Roaster


<img src="https://github.com/Quilliams85/FluidBedRoaster/blob/main/Documentation/Photos/IMG_2929.jpeg?raw=true" alt="drawing" width="200"/>

## Video Demo on Youtube
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/LB3SWqTu2bo/0.jpg)](https://youtu.be/LB3SWqTu2bo)

## Instructions

### Materials
[Documentation/build instructions/Bill of Materials
](https://github.com/Quilliams85/FluidBedRoaster/blob/main/Documentation/build%20instructions/Bill%20of%20Materiasl.csv)
### Build
(need to write detailed instructions) <br/>
<br/>
CAD Files:
[Onshape Link(not totally up-to-date
](
https://cad.onshape.com/documents/d441bf229b7e9ce5ff684fb0/w/b98e2fd99638b01268ae092b/e/72b5818c1e97701c577703ba?renderMode=0&uiState=671aff36163ea77b529e66e2)
<br/>
<br/>
I'd probably only build this if you are comfortable working with AC Power and electronics, there are a lot of safety considerations with the amount of power this roaster uses.
### Software
software used: 
1. Arduino IDE
2. Artisan
3. Onshape

#### Artisan Setup:
Use the arduino code provided to set up MODBUS protocol on the arduino, then it's pretty easy to set up Artisan with MODBUS. Navigate to config/device, then set meter to MODBUS. Then set the port to the USB port connected to the arduino, make sure they are using the same baud rate, and configure input 1 to [Slave=1, Register=2, Divider=1/100, Mode=F]. For PID control, just tick "control" next to the meter in device config. I found kp=1, ki=0, and kd=20 to be good values, but it depends on the machine and probe placement/type. For a more detailed explanation of Artisan PID, there are tons of good videos on youtube([this one is good](https://www.youtube.com/watch?v=5OrrdhCTBPE&t=410s)).<br/>
<br/>
This blog post was also very helpful for setup, and inspired a lot of my design choices for the roaster:[link](http://www.sinobi.dk/henrik/coffeeroaster1/)
