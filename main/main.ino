// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#include "Sensors.h"
#include "locomotion.h"
using namespace hardware;
using namespace hardware::pins;

float Yaw;
//Variable that indicate the car to move , stop(0), front(1), left_turning(2), right_turning(3), back_turning(4)
int motion_mode = MOTION_STOP;

int count=2;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
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


    // 3. Give command to locomotion
    
    motion_mode = MOTION_FORWARD;
    delay(5000);
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
        forward(1, distance_f, distance_l, distance_r);
    }
    else if (current_mode == MOTION_FORWARD && motion_mode == MOTION_FORWARD)
    {
        // pass the sensors' data to locomotion
    }
    else
    {
        Serial.println("Motion status not recognized!");
    }


    // Test code
    // Serial.print("Front: ");
    // Serial.print(distance_f);
    // Serial.print(" Left: ");
    // Serial.print(distance_l);
    // Serial.print(" Right: ");
    // Serial.print(distance_r);
    // Serial.print(" Yaw: ");
    // Serial.println(Yaw);
}
