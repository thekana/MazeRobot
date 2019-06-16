# MazeRobot
The major project for MTRN4110 is a robot maze race based on the Micromouse competition.
# MTRN4110 Project Phase A (week 1-4)

# (Updated 15/6/2019)

The purpose of this phase is to get students familiarised with the hardware and electronics they will use.
Students will also gain hands-on design and prototyping experience.

The tasks for phase A are divided into 4 roles, one for each of the students in the group. Group of 3 does
not need to complete Role 4.

**Note: this is only an initial prototyping stage; students can make changes to the design in later phases.
The students are allowed to use the lab kit for assessment if their own platform is not ready or using
different components from the ones required in the assessment.**

## Student 1: Locomotion

This student will be responsible for making the robot move. The student needs to research and select
mechanical and electrical components to come with a suitable design for the vehicle to race within the
maze.

The student will be assessed in the following way and have a maximum of 10 minutes for assessment.
Only one trial is allowed.

The student needs to connect the Arduino with two sets of motors, encoders, and wheels, upload a
program to Arduino, and keep it running. **The demonstrator will give a random value for task 4 and
task 5 as specified in the tasks before the assessment. The demonstrator will input a NUMBER (the
number in the front of each task as listed below) to the serial monitor of Arduino and the Arduino
should demonstrate the corresponding functions.** (e.g., If the demonstrator inputs 1 to the serial
monitor and clicks “Enter”, the Arduino should toggle an LED.) **Any implementation (including using 3rd
party libraries) is allowed.**

- 1. Toggle an LED – on for 2 seconds, then off ( 2 )
- 2. Run motor 1 at full speed in the forward direction for 5 seconds ( 2 ), display the readings of
    the associated encoder on the serial monitor of Arduino ( 2 ), and display the distance travelled
    by the wheel ( 1 ) – (Total 5 )
- 3. Run motor 2 at half speed in the backward direction for 5 seconds ( 2 ), display the readings of
    the associated encoder on the serial monitor of Arduino ( 2 ), and display the distance travelled
    by the wheel ( 1 ) – (Total 5 )
- 4. Control the left wheel to run for an angle ( **X deg** ) given by the demonstrator before the test
    and display the readings of the associated encoder on the serial monitor. **The student should**
    **define a variable in their program to store this value and allow the demonstrator to change**
    **the value of this variable before the assessment.** (Positive value means the wheel should move
    forward if mounted on the left side of the robot) ( 3 )
- 5. Control the right wheel to run for an angle ( **Y deg** ) given by the demonstrator before the test
    and display the readings of the associated encoder on the serial monitor. **The student should**


```
define a variable in their program to store this value and allow the demonstrator to change
the value of the variable before the assessment. (Positive value means the wheel should move
forward if mounted on the right side of the robot) ( 3 )
```
- Provide power and drive module schematics and the hardware correctly connected ( 2 )
- _2 bonus points for demonstrating the above using_ **_hardware::digital_pin, hardware::motor,_**
    **_hardware::encoder, hardware::wheel_**_. If not all the functions are implemented, 0.5 bonus point_
    _for each implemented function (maximum 2)._
- ------------------------------- **The following criteria are cancelled.** ------------------------------------------------
- Perform digital IO using hardware::digital_pin, e.g. toggle led (2)
- Perform analog IO using hardware:: analog_pin (1)
- Control a motor using hardware::motor (4)
- Read a wheel encoder using hardware::encoder (4)
- Return the distance travelled by a wheel using hardware::wheel (1)
- Implement everything in “units.h” (6)
- Provide power and drive module schematics (2)

## Student 2: Sensing

This student will be responsible for sensing the environment when the robot autonomously explores the
maze. The student needs to select the appropriate sensors required and implement software to
interface with the sensors.

The student will be assessed in the following way and have a maximum of 10 minutes for assessment.
Only one trial is allowed.

The student needs to connect the Arduino with an IMU, an Ultrasound sensor, and two lidar sensors,
upload a program to Arduino, and keep it running. **The demonstrator will input a NUMBER (the number
in the front of each task as listed below) to the serial monitor of Arduino and the Arduino should
demonstrate the corresponding functions.** (e.g., If the demonstrator inputs 1 to the serial monitor and
click “Enter”, the Arduino should display the required readings of the sensors on the serial monitor.) **Any
implementation (including using 3rd party libraries) is allowed.**

- 1. Simultaneously display the roll ( 1 ), pitch ( 1 ), yaw ( 1 ) angles and the acceleration in X ( 1 ), Y ( 1 ),
    Z ( 1 ) directions of the IMU and the distances detected by the ultrasound sensor ( 2 ) and the two
    lidar sensors ( 4 ) in a sequential order on the serial monitor (Total 12)
- 2. The demonstrator puts a hand in front of the ultrasound sensor at a distance of around 5cm
    for 2s, then moves the hand away. Display a message (e.g., “start”) on the serial monitor. ( 2 )
- 3. Assume the ultrasound sensor and the two lidar sensors are the front, the left, and the right
    sensor, respectively. The demonstrator randomly puts “walls” in front of the three sensors at a
    distance around 10cm. Display the existence of the **left, front, and right** walls on the serial
    monitor. (e.g., “1 0 0” means there is a wall in the left and no walls in the front or right) ( 2 )
- 4. Assume the ultrasound sensor and the two lidar sensors are the front, the left, and the right
    sensor, respectively. Also assume the robot is facing North. The demonstrator rotates the IMU
    sensor for an angle (90deg, 180deg, or 270deg) horizontally and randomly puts “walls” in front
    of the three sensors at a distance around 10cm. Display the existence of the **East, South, West,**
    **and North** walls. (e.g., “1 0 0 2” means there is a wall in the East, no walls in the South or West,
    and not sure about whether there is a wall or not in the North) ( 2 )


- Provide schematics on how to connect the sensors to the control unit and the hardware

### correctly connected ( 2 )

- _2 bonus points for demonstrating the above functions_ **_using hardware::i2c, hardware::interrupt,_**
    **_hardware::imu, hardware::lidar, hardware::sonar_**_. If not all the functions are implemented, 0._
    _points for each function (maximum 2)._
- ------------------------------- **The following criteria are cancelled.** ------------------------------------------------
- Implement hardware::i2c communication (2)
- Implement hardware::interrupt (2)
- Read orientation using hardware::imu (5)
- Detect distance using time of flight range module with hardware::lidar (5)
- Detect distance using ultrasound range module with hardware::sonar (4)
- Provide schematics on how to connect the sensors to the control unit (2)

## Student 3: Hardware design

This student will be responsible for designing the hardware platform of the robot. The design needs to
fit within the maximum allowed size of the robot.

To get full marks, the student needs to demonstrate a fully assembled vehicle. The student will be
assessed in the following way and have a maximum of 10 minutes for assessment.

- Drive System (Total: 7 )
    o Motors and wheels mounted to allow driving forward, backward and turning ( 3 )
    o Encoders mounted to allow the wheel position to be read ( 2 ) – Zero point for using off-
       the-shelf brackets. If encoders are not used, the student needs to demonstrate how the
       wheel/robot position can be measured
    o Stability. The platform remains stable when moving and does not fall over. ( 2 )
- Sensing (Total: 5 )
    o IMU mounted. ( 2 ) - If IMU is not used, the student needs to demonstrate how the
       orientation of the robot can be measured.
    o Wall detection sensors mounted. ( 3 ) - If wall detection sensors are not used, the student
       needs to demonstrate that the front, left, and right walls can be detected.
- Power and Control (Total 4 )
    o Battery holder mounted ( 1 )
    o Arduino Mega mounted ( 1 )
    o 2x LED clearly visible on top from outside the maze (1).
    o 1 power switch to enable/disable the robot mounted (1).
- Design and Build Quality (Total: 4 )
    o Size: Fit within the size limit ( 2 )
    o Quality ( 2 ).
       ▪ Poor: flimsy, parts don’t work, incomplete. Components are easy to break. No
          points
       ▪ Just works: functional but not robust, e.g. hold together by duct tape. 1 point
       ▪ Excellent: well designed, looks well engineered. Full 2 points.


## Student 4: Communication

This student will be responsible for communicating between a computer and the vehicle using Bluetooth
communication.

The student will be assessed in the following way and have a maximum of 10 minutes for assessment.
Only one trial is allowed.

The student needs to connect the Arduino with a Bluetooth, upload a program to Arduino, and keep it
running. The student should also prepare a device to be paired with the Bluetooth. The Arduino should
be able to demonstrate the following corresponding functions. **Any implementation (including using 3 rd
party libraries) is allowed.**

- 1. The demonstrator inputs “AT” in the serial monitor of Bluetooth and receives a response “OK”
    ( 2 )
- 2. The demonstrator inputs a string on an external device, send the string to the Arduino
    through Bluetooth and display the string on the serial monitor of Bluetooth ( 2 ) and the LCD
    display ( 2 ) – (Total 4)
- 3. The demonstrator inputs a string in the serial monitor of the Bluetooth, send the string to the
    external device and display on the external device ( 2 )
- 4. Devise an appropriate encoding method ( 2 ) and successfully encode and send a 2 by 3 maze
    layout given by the demonstrator to the Arduino from an external device through Bluetooth,
    and display the string on the serial monitor ( 3 ) – (Total 5 )
- 5. Decode the received maze layout and display it on the serial monitor of Arduino ( 5 )
- E.g.

#### •

- Provide schematics for connecting Bluetooth module to the controller and the hardware
    correctly connected ( 2 )
- _2 bonus points for demonstrating using_ **_hardware::serial_api, hardware::bluetooth,_**
    **_hardware::display, hardware::wheel. hardware::receive_maze_layout,_**
    **_hardware::parse_maze_layout_** _– If not all the functions are implemented, 0.5 point for each_
    _function (maximum 2)._
- ------------------------------- **The following criteria are cancelled.** ------------------------------------------------
- Implement UART serial communication using hardware::serial_api. Define a convenience symbol
    hardware::serial in “hardware_definition.h”. Note: this could just be “using serial =
    serial_api<serial_tag<0>>”. (3)
- Implement Bluetooth communication using hardware::bluetooth. Define a convenience symbol
    hardware::bluetooth in “hardware_definition.h”. Note: this could just be using “bluetooth =
    serial_api<serial_tag<1>>;”. (3)
- Implement hardware::display (2)
- Implement and demonstrate a way to send and receive Bluetooth message on the computer (4)
- Specify a protocol for transmitting the maze layout using Bluetooth.


```
o Generate a message encoding the maze layout on the computer side and transmit (3)
o Receive the maze layout massage on the Arduino and parse the message (3) See
hardware::receive_maze_layout and hardware::parse_maze_layout.
```
- Provide schematics for connecting Bluetooth module to the controller (2)

## Resources:

- Arduino references: https://www.arduino.cc/reference/en/
- Arduino examples: https://www.arduino.cc/en/Tutorial/BuiltInExamples
- C++ references: https://en.cppreference.com/w/
- C++ templates: [http://www.cplusplus.com/doc/oldtutorial/templates/](http://www.cplusplus.com/doc/oldtutorial/templates/)
- C++ namespaces: [http://www.cplusplus.com/doc/oldtutorial/namespaces/](http://www.cplusplus.com/doc/oldtutorial/namespaces/)
