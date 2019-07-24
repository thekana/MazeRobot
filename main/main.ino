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

int startStep = 0;
int cellCount = 0;
MazeCell maze[5][9];
Maze *mazePrint = new Maze("");
coord curCoord = {0,0};
long current = 0;
long timeIntervel = 0;
byte heading = 0;

int start = 0;

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
    bluetooth.println("Enter short or long side...");
    while (bluetooth.available()==0){delay(100);} // empty buffer    
//  while (bluetooth.available()==0){delay(100);} // empty buffer
    if(bluetooth.available() > 0){
      keyword = bluetooth.readString();
      if(keyword.equals("s\n"))
        heading = 4;
      bluetooth.print("Heading = ");bluetooth.println(heading);
    }
    bluetooth.println("End setup!");
//    delay(1000);
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
        if(c.equals("s\n") || c.equals("l\n")) 
        {
            String keyword;
            keyword = c;
        }
    }


    // put your main code here, to run repeatedly:
    double distance_f, distance_l, distance_r;
    //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise
    int lfr[3];
    //Varianle that indicate which direction (East(1), South(2), West(3),North(4)) the car is facing
    int face_dir = 0;
    //Array be used to determine whether there is al wall at East, South, West and North
    int ESWN[4] = {0, 0, 0, 0};

    if(startStep==0)
    {
        ledR::high();
        ledG::low();
        startStep = 1;
        current = millis();
        timeIntervel = 0;
    }
    else if(startStep==1)
    {
        if(timeIntervel < 2000)
        {
            if(distance_f <= 50)
            {
                timeIntervel = millis() - current;
            }
            else
            {
                current = millis();
                timeIntervel = 0;
            }
            bluetooth.print("Sonic: ");
            bluetooth.println(distance_f);
        }
        else if(distance_f > 50)
        {
            bluetooth.println("Car start");
            ledR::low();
            ledG::high();
            delay(1000);
            startStep = 2;
        }
        else
        {
            timeIntervel = millis() - current;
        }
    }

    // 0. Read from bluetooth? Any command?
    if(motion_mode >= MOTION_LEFT || startStep < 2) return;

    int current_mode = motion_mode;

    // 1. perception
    // Read from sensors
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
         Serial.print("");
    }
    else
    {
         exploration(motion_queue, lfr, &startStep, keyword, &cellCount, &heading);
    }
     
    if(startStep == 4)
    {
      bluetooth.println("end of maze. ready to print.");
      ledG::low();
      ledR::high();
      mazePrint->print();
      startStep = 5;
      delay(200000);
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
