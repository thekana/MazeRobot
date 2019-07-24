// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "cppQueue.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#include "Sensors.h"
#include "locomotion.h"
#include "exploration.h"
#define bluetooth Serial3
using namespace hardware;
using namespace hardware::pins;

bool map_ready=false;

float Yaw;
//Variable that indicate the car to move , stop(0), front(1), left_turning(2), right_turning(3), back_turning(4)
int motion_mode = MOTION_STOP;
static Queue motion_queue(sizeof(int));

int count=2;

int start=0;


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    bluetooth.begin(115200);
    bluetooth.setTimeout(100);

    bluetooth.println("Hello world!");

    while (! Serial);
    Serial.println("Serial setup finish!");

    // setup leds
    leds_setup();
    Serial.println("Leds setup finish!");

    // setup locomotion
    locomotion_setup();
    Serial.println("Locomotion setup finish!");

    //setup Sensors
    sensorSetup();
    // setup map

    // Wait 5 seconds and main loop start
    delay(1000);
}

void loop() {
    if(bluetooth.available() > 0)
    {
        String c = bluetooth.readString();
        if(c.equals("12\n")) start=1;
        Serial.print(c);
        Serial.print(start);
    }


    // put your main code here, to run repeatedly:
    double distance_f, distance_l, distance_r;
    //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise
    int lfr[3];
    //Varianle that indicate which direction (East(1), South(2), West(3),North(4)) the car is facing
    int face_dir = 0;
    //Array be used to determine whether there is al wall at East, South, West and North
    int ESWN[4] = {0, 0, 0, 0};


    // 0. Read from bluetooth? Any command?
    if(motion_mode != MOTION_STOP && motion_mode != MOTION_FORWARD) return;

    int current_mode = motion_mode;

    // 1. perception


    // 1.1.Read from wheel


    // 1.2.Read from sensors
    //function that dectect the left, front and right wall near the car
    //front distance (ultrasonic distance)
    distance_f = sonar<pins::sonar_trigger, pins::sonar_echo>::distance().count();        //convert the CM to MM
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
    ESWNWall(Yaw, lfr, ESWN, &face_dir);


    // 2. Localisation and planning

    if(map_ready)
    {
        // planning();
    }
    else
    {
      if(startStep==0){
         ledR::high();
         ledG::low();
         startStep = 1;
         current = millis();
         timeIntervel = 0;
      }
      else if(startStep==1){
        if(timeIntervel < 2000){
          if(distance_f <= 50){
            timeIntervel = millis() - current;
          }else{
            current = millis();
            timeIntervel = 0;
          }
          bluetooth.print("Sonic: ");
          bluetooth.println(distance_f);
        }else if(distance_f > 50){
            bluetooth.println("Car start");
            ledR::low();
            ledG::high();
            delay(1000);
            startStep = 2;
        }else{
          timeIntervel = millis() - current;
        }
      }else if(startStep == 2){

        if(keyword.equals("s\n")){
          if(lfr[2] == 0){
            curCoord.x = X - 1;
            startStep = 3;
          }else if(lfr[0]==1){
            cellCount++;
            bluetooth.print("CellCount: "); bluetooth.println(cellCount);
            delay(7000);
          }else{
            startStep = 3;
          }
        }else if(keyword.equals("l\n")){
          if(lfr[0]==0){
            curCoord.x = X-1;
            heading = 2;
            startStep = 3;
          }else if(lfr[2]==1){
            cellCount++;
            bluetooth.print("CellCount: "); bluetooth.println(cellCount);
            delay(7000);
          }else{
            heading = 8;
            startStep = 3;
          }
        }
      }else if(startStep == 3){
        if(keyword.equals("s\n")){
          if(cellCount!=0){
            curCoord.y = cellCount;
            for(int i=0;i<cellCount;i++){
              maze[i][curCoord.x].walls = 11;
            }
            cellCount = 0;
    //        keyword = "";
          }
        }else if(keyword.equals("l\n")){
          if(cellCount!=0){
            if(heading==8){
              curCoord.x = cellCount;
    //          maze[curCoord.y][0].walls = 4;
              for(int i=1;i<cellCount;i++){
                maze[curCoord.y][i].walls = 7;
              }
            }else if(heading == 2){
              curCoord.x = X - 1 - cellCount;
    //          maze[curCoord.y][X-1].walls = 8;
              for(int i=cellCount-1;i>=0;i--){
                maze[curCoord.y][X-i].walls = 13;
              }
            }
    //        maze[0][curCoord.x].walls = 2;
            cellCount = 0;
    //        keyword = "";
          }

        }
        if(maze[curCoord.y][curCoord.x].distance != 0){
          bluetooth.print("Current cell distance: ");  bluetooth.println(maze[curCoord.y][curCoord.x].distance);
          bluetooth.print("front: "); bluetooth.println(distance_f);
          bluetooth.print("left: "); bluetooth.println(distance_l);
          bluetooth.print("right: ");bluetooth.println(distance_r);
          bluetooth.print("Wall: ");bluetooth.print(lfr[0]);bluetooth.print(lfr[1]);bluetooth.println(lfr[2]);
          bluetooth.print("Current cell: [");bluetooth.print(curCoord.y);
          bluetooth.print(" "); bluetooth.print(curCoord.x);bluetooth.println("] ");
          floodFillUpdate(curCoord, heading, lfr);
          maze[curCoord.y][curCoord.x].marked = true;
          mazePrint->fillCells(curCoord.y,curCoord.x,maze[curCoord.y][curCoord.x].walls);
          int nextHeading = orient(curCoord, heading);
          coord nextCoord = bearingCoord(curCoord, nextHeading);

          bluetooth.print("next cell: ["); bluetooth.print(nextCoord.y);
          bluetooth.print(" "); bluetooth.print(nextCoord.x);bluetooth.println("] ");
          bluetooth.print("Current heading: "); bluetooth.println(heading);
          curCoord = nextCoord;
          heading = nextHeading;

          bluetooth.print("next heading: "); bluetooth.println(heading);
          bluetooth.println();

          delay(7000);
        }else{
          bluetooth.println("end of maze. ready to print.");
          ledG::low();
          ledR::high();
          while(bluetooth.available()==0){}

          //GOAL FINDED, STOP CAR, end execution
          startStep = 4;
        }

      }else if(startStep == 4){
        mazePrint->print();
        startStep = 5;
        delay(200000);
      }else if(startStep == 5){
        delay(100);
      }
        // exploration(motion_queue, lfr);
    }


    // 3. Give command to locomotion
    // bluetooth.print(lfr[0]);
    // bluetooth.print("******");
    // bluetooth.print(lfr[1]);
    // bluetooth.print("******");
    // bluetooth.println(lfr[2]);

    if(!start) return;

    int ncells = 0;
    if (motion_mode == MOTION_STOP)
    {
        // Assign other values here
        motion_mode = MOTION_FORWARD;
    }

    // A simple obstacle avoidance behaviour
    // if (motion_mode == MOTION_STOP)
    // {
    //     delay(500);
    //     if(lfr[1]==0)
    //     {
    //         bluetooth.println("Forward");
    //         motion_mode=MOTION_FORWARD;
    //     }
    //     else if(lfr[0]==0)
    //     {
    //         bluetooth.println("Left");
    //         motion_mode=MOTION_LEFT;
    //     }
    //     else if(lfr[2]==0)
    //     {
    //         bluetooth.println("Right");
    //         motion_mode=MOTION_RIGHT;
    //     }
    // }

    // motion_mode = MOTION_FORWARD;
    // delay(5000);
    // motion_mode = count;
    // Serial.print(count);
    // Serial.println("**********************");
    // count++;
    // delay(5000);
    if (motion_mode >= MOTION_LEFT)
    {
        turning();
    }
    else if (current_mode == MOTION_STOP && motion_mode == MOTION_FORWARD)
    {
        forward(ncells, distance_f, distance_l, distance_r);
    }
    else if (current_mode == MOTION_FORWARD && motion_mode == MOTION_FORWARD)
    {
        forward_updating(distance_f, distance_l, distance_r);
    }
    else
    {
        Serial.println("Motion status not recognized!");
    }
    // Test code
    // bluetooth.print("Front: ");
    // bluetooth.print(distance_f);
    // bluetooth.print(" Left: ");
    // bluetooth.print(distance_l);
    // bluetooth.print(" Right: ");
    // bluetooth.println(distance_r);
    // Serial.print(" Yaw: ");
    // Serial.println(Yaw);
}
