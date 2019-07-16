#include "Sensors.h"
using namespace hardware;
using namespace hardware::pins;
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

float Yaw;
long current, timeIntervel;
char keyword;

void setup() {
  // put your setup code here, to run once:
  // setup pins
  // setup encoders
  // setup motors
  //setup Sensors
  sensorSetup();
  // setup map
}

void loop() {
  // put your main code here, to run repeatedly:
  double distance_f, distance_l, distance_r;      
  //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise
  int lfr[3];    
  //Variable that indicate the car to move front(0), left(1), right(2), back(3)                       
  int move_dir = 0;
  //Varianle that indicate which direction (East(1), South(2), West(3),North(4)) the car is facing
  int face_dir = 0;
  //Array be used to determine whether there is al wall at East, South, West and North
  int ESWN[4] = {0, 0, 0, 0}; 


  // 0. Read from bluetooth? Any command?

  // 1. perception
  
    // 1.1.Read from wheel
  
    // 1.2.Read from sensors
  //function that dectect the left, front and right wall near the car
  //front distance (ultrasonic distance)                                
  distance_f = sonar<pins::sonar_trigger,pins::sonar_echo>::distance().count();         //convert the CM to MM
  //left distance (left lidar)
  distance_l = lidar<lidar_tag<0>>::distance().count();
  //right distance (right lidar)
  distance_r = lidar<lidar_tag<1>>::distance().count();
  carWall(distance_l, distance_f, distance_r, lfr); 
  //update IMU parameters
  imu::update();
  //Get IMU yaw value
  Yaw = imu::yaw();
  //function that records the walls near car (Ease, South, West, North)
  ESWNWall(Yaw, lfr, ESWN,&face_dir);
  // 2. Localisation and planning

  // 3. Give command to locomotion
  
}
