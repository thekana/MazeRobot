# MazeRobot
The major project for MTRN4110 is a robot maze race based on the Micromouse competition.
# MTRN4110 Project Phase A (week 1-3)

The purpose of this phase is to get students familiarised with the hardware and electronics they will use.
Students will also gain hands-on design and prototyping experience.

The tasks for phase A are divided into 4 roles, one for each of the students in the group. Group of 3 does
not need to complete role 4.

To make marking easier and more consistent, all groups must implement a set of predefined APIs in the
provided headers. The API is designed to work with many common hardware variations the student may
choose from. The API also allows each student to work independently. Please refer to the provided
headers for the additional functionality of each function and classes.

Students should define their hardware configuration in “hardware_definition.h”. **Do not make changes
to other provided headers**. For this phase, students will need to implement functions and classes
defined in “hardware.h” and “units.h”. Each student put their code into one or more separate
implementation .cpp files.

Libraries required to use for the lab hardware are provided in files starting with “external_”. Third-party
libraries can be used in the implementation file. Make sure the correct headers are included and third-
party libraries do not interfere with provided libraries.

Completion of the tasks for the role will be assessed in **week 4** as an individual assessment and make up
**20% course mark** for each individual student. Phase A will be assessed based on the correctness and
quality. Students need to demonstrate their software works with hardware. Works that are difficult to
mark due to messiness, bad formatting, inconsistent style, lack of commenting, difficult to understand,
etc will be penalised.

Note: this is only an initial prototyping stage, students can make a change to the design in later phases.

## How student code will be assessed

Each student should keep their code in one or more implementation files.

## Student 1: Locomotion

This student will be responsible for making the robot move. The student needs to research and select
mechanical and electrical components to come with a suitable design for the vehicle to race within the
maze. The student will also implement some miscellaneous functionalities.

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

- Implement hardware::i2c communication (2)
- Implement hardware::interrupt (2)
- Read orientation using hardware::imu (5)
- Detect distance using time of flight range module with hardware::lidar (5)
- Detect distance using ultrasound range module with hardware::sonar (4)
- Provide schematics on how to connect the sensors to the control unit (2)

## Student 3: Hardware design

This student will be responsible for designing the hardware platform of the robot. The design needs to
fit within the maximum allowed size of the robot.

To get full marks, the student needs to demonstrate a fully assembled vehicle.

- Drive System (Total: 7)
    o Motors and wheels mounted to allow driving forward, backward and turning (3)
    o Encoders mounted to allow the wheel position to be read (2) – Zero point for using off-
       the-shelf brackets. If encoders are not used, the student needs to demonstrate how the
       wheel/robot position can be measured
    o Stability. The platform remains stable when moving and does not fall over. (2)
- Sensing (Total: 5)
    o IMU mounted. (2) - If IMU is not used, the student needs to demonstrate how the
       orientation of the robot can be measured.
    o Wall detection sensors mounted. (3) - If wall detection sensors are not used, the student
       needs to demonstrate that the front, left, and right walls can be detected.
- Power and Control (Total 4)
    o Battery holder mounted (1)
    o Arduino Mega mounted (1)
    o 2x LED clearly visible on top from outside the maze (1).
    o 1 power switch to enable/disable the robot mounted (1).
- Design and Build Quality (Total: 4)
    o Size: Fit within the size limit (2)
    o Quality (2).
        Poor: flimsy, parts don’t work, incomplete. Components are easy to break. No
          points
        Just works: functional but not robust, e.g. hold together by duct tape. 1 point
        Excellent: well designed, looks well engineered. Full 2 points.


## Student 4: Communication

This student will be responsible for communicating between a computer and the vehicle using Bluetooth
communication.

- Implement UART serial communication using hardware::serial_api. Define a convenience symbol
    hardware::serial in “hardware_definition.h”. Note: this could just be “using serial =
    serial_api<serial_tag<0>>”. (3)
- Implement Bluetooth communication using hardware::bluetooth. Define a convenience symbol
    hardware::bluetooth in “hardware_definition.h”. Note: this could just be using “bluetooth =
    serial_api<serial_tag<1>>;”. (3)
- Implement hardware::display (2)
- Implement and demonstrate a way to send and receive Bluetooth message on the computer (4)
- Specify a protocol for transmitting the maze layout using Bluetooth.
    o Generate a message encoding the maze layout on the computer side and transmit (3)
    o Receive the maze layout massage on the Arduino and parse the message (3) See
       hardware::receive_maze_layout and hardware::parse_maze_layout.
- Provide schematics for connecting Bluetooth module to the controller (2)

## Grading Rubric:

Students will be assessed on the quality of completion of the designated tasks. The mark allocated with
each task is shown in the brackets.

## Resources:

- Arduino references: https://www.arduino.cc/reference/en/
- Arduino examples: https://www.arduino.cc/en/Tutorial/BuiltInExamples
- C++ references: https://en.cppreference.com/w/


