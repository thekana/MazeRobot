#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
#include "Maze.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
using namespace hardware;
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

float Yaw;
long current, timeIntervel;
char keyword;

Maze *maze = new Maze("");

extern int counterA;
extern int counterB;

using namespace hardware::pins;

void setup() {
  // put your setup code here, to run once:
  // setup pins
  // setup encoders
  // setup motors
  //setup sonar
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  sonar<sonar_trigger,sonar_echo>::enable();
  // setup lidar
  left_lidar_enable::config_io_mode(io_mode::output);
  right_lidar_enable::config_io_mode(io_mode::output);
  
  left_lidar_enable::write(logic_level::low);
  right_lidar_enable::write(logic_level::low);
  lidar<lidar_tag<0>>::enable(); //left lidar
  delay(100); 
  lidar<lidar_tag<1>>::enable();  //right lidar
  //setup imu
  if(imu::enable()){
    Serial.println("IMU success");  
  }else{
    Serial.println("IMu failed");  
  }
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
  distance_f = sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>::distance().count();         //convert the CM to MM
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

int orientation(int lfr[]){
  int dir = 1;  //direction number indicate the should move front(1), turn right(2), turn left(3),turn back(4)
  if(lfr[0]==1 && lfr[2]==1){
    dir = 1;  
  }
  if(lfr[0]==1 && lfr[1]==1){
    dir = 2;  
  }
  if(lfr[1]==1 && lfr[2]==1){
    dir = 3;  
  }
  if(lfr[0]==1 && lfr[1]==1 && lfr[2]==1){
    dir = 4;  
  }
  return dir;
}

void carWall(double distance_l, double distance_f, double distance_r,int *lfr){
  if(distance_f < 100){
    lfr[1] = 1;
  }else{
    lfr[1] = 0;
  }

  if(distance_l < 100){
    lfr[0] = 1;
  }else{
    lfr[0] = 0;  
  }
//  delay(20);
 
  if(distance_r < 100){
    lfr[2] = 1;
  }else{
    lfr[2] = 0;  
  }
}

void ESWNWall(double Yaw, int *lfr, int *ESWN, int *face_dir){
  if(Yaw > -22.5 && Yaw < 22.5){
//      Serial.println("North");
      *face_dir = 4;
      if(lfr[1]!=0){
        ESWN[3] = 1;
      }else{
        ESWN[3] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[2] = 1;  
      }else{
        ESWN[2] = 0;  
      }
      if(lfr[2]!=0){
        ESWN[0] = 1;  
      }else{
        ESWN[0] = 0;  
      }
      ESWN[1] = 0;
  }else if(Yaw > 67.5 && Yaw < 112.5){
//      Serial.println("East");
      *face_dir = 1;
      if(lfr[1]!=0){
        ESWN[0] = 1;
      }else{
        ESWN[0] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[3] = 1;  
      }else{
        ESWN[3] = 0;  
      }
      if(lfr[2]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0;  
      }
      ESWN[2] = 0;
  }else if(Yaw < -67.5 && Yaw > -112.5){
//      Serial.println("West");
      *face_dir = 3;
      if(lfr[1]!=0){
        ESWN[2] = 1;
      }else{
        ESWN[2] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0; 
      }
      if(lfr[2]!=0){
        ESWN[3] = 1;  
      }else{
        ESWN[3] = 0;  
      }
      ESWN[0] = 0;
  }else if(Yaw > 157.5 || Yaw < -157.5){
//      Serial.println("South");
      *face_dir = 2;
      if(lfr[1]!=0){
        ESWN[1] = 1;
      }else{
        ESWN[1] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[0] = 1;  
      }else{
        ESWN[0] = 0;
      }
      if(lfr[2]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0;  
      }
      ESWN[3] = 0;
  }
}
