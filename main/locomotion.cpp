/**
 * @file locomotion.cpp
 * @author Liangde Li
 * @date 16 July 2019
 * @brief The functions for leds and locomotion system
 *
 * @note 
 * */
#include <stdlib.h>

#include "locomotion.h"

#define bluetooth Serial3
#define MIN_THRESHOLD_LEFT  50.0
#define MAX_THRESHOLD_LEFT  70.0
#define MIN_THRESHOLD_RIGHT 50.0
#define MAX_THRESHOLD_RIGHT 70.0

extern int counterA;
extern int counterB;
extern int motion_mode;

static int counterA_history = 0;
static int counterB_history = 0;

int goalA=0;
int goalB=0;

static double vLeft=0;
static double vRight=0;
static double last_offset=0;

static void apply_speed(void);
static void motor_controller(double distance_f, double distance_l, double distance_r);
static void callback1(void);
static void callback2(void);

void leds_setup(void)
{
    hardware::led_red::config_io_mode(hardware::io_mode::output);
    hardware::led_green::config_io_mode(hardware::io_mode::output);
}

void locomotion_setup(void)
{
    goalA=0;
    counterA=0;
    goalB=0;
    counterB=0;

    hardware::left_encoder::enable();
    hardware::pins::left_encoder_a::attach_interrupt(&callback1, hardware::interrupt_mode::change);
    hardware::left_motor::enable();
    hardware::right_encoder::enable();
    hardware::pins::right_encoder_a::attach_interrupt(&callback2, hardware::interrupt_mode::change);
    hardware::right_motor::enable();

}

void turning(void)
{
    if(motion_mode == MOTION_LEFT)
    {
        goalA=-67;
        counterA=0;
        goalB=67;
        counterB=0;
        vLeft=-60.0;
        vRight=65.0;
        apply_speed();
    }
    else if (motion_mode == MOTION_RIGHT)
    {
        goalA=67;
        counterA=0;
        goalB=-67;
        counterB=0;
        vLeft=60.0;
        vRight=-65.0;
        apply_speed();
    }
    else if (motion_mode == MOTION_BACK)
    {
        goalA=-141;
        counterA=0;
        goalB=141;
        counterB=0;
        vLeft=-60.0;
        vRight=65.0;
        apply_speed();
    }
    else
    {
        Serial.println("Warning: Motion mode not properly setup in turning!");
    }
}

void forward(int ncells, double distance_f, double distance_l, double distance_r)
{
    goalA = ncells*253;
    counterA = 0;
    counterA_history = 0;
    goalB = ncells*253;
    counterB = 0;
    counterB_history = 0;
    vLeft=60.0;
    vRight=65.0;
    apply_speed();
}

void forward_updating(double distance_f, double distance_l, double distance_r)
{
    motor_controller(distance_f, distance_l, distance_r);
}

/**
 * @brief Send the distance data to locomotion module
 *
 * @param 
 *
 * @return 
 *
 * @note Send the distance data to locomotion module
 * */
void apply_speed(void)
{
    units::percentage left_speed(abs(vLeft));
    units::percentage right_speed(abs(vRight));
    
    if (vLeft >= 0)
        hardware::left_motor::forward(left_speed);
    else
        hardware::left_motor::backward(left_speed);
    
    if (vRight >= 0)
        hardware::right_motor::forward(right_speed);
    else
        hardware::right_motor::backward(right_speed);
}

/**
 * @brief Send the distance data to locomotion module
 *
 * @param 
 *
 * @return 
 *
 * @note Send the distance data to locomotion module
 * */
void motor_controller(double distance_f, double distance_l, double distance_r)
{
    // brake
    if (distance_f > 10 && distance_f < 60.0)
    {
        vLeft=0.0;
        vRight=0.0;
        apply_speed();
        goalA=0;
        goalB=0;
        motion_mode = MOTION_STOP;
        bluetooth.print("Brake!!! distance: ");
        bluetooth.println(distance_f);
        goto print_out;
    }
    // Goto mild controller depending on counters
    if( (distance_l < MAX_THRESHOLD_LEFT && distance_l > MIN_THRESHOLD_LEFT) 
        || (distance_r < MAX_THRESHOLD_RIGHT && distance_r > MIN_THRESHOLD_RIGHT))
    {
        if ((counterB-counterB_history) >=20)
        {
            vLeft += ((counterB-counterB_history)-(counterA-counterA_history))/2.0;
            bluetooth.print("Counter add up: ");
            bluetooth.println(((counterB-counterB_history)-(counterA-counterA_history))/2.0);
            counterA_history = counterA;
            counterB_history = counterB;
        }
    }

    double offset = 0.0;
    if(distance_l+distance_r < 140)
        offset = (distance_r-distance_l)/2.0;
    else if (distance_l < 140)
        offset = 60.0 - distance_l;
    else if (distance_r < 140)
        offset = distance_r - 60.0;

    double update;
    update = offset;
    vLeft = 60.0 + update;
//    bluetooth.print("Sensor add up: ");
//    bluetooth.println(update);

    last_offset = offset;

    vLeft = max(min(vLeft, 100.0), -100.0);

    apply_speed();

    // bluetooth.println("Here");

print_out:
  ;
//    bluetooth.print(distance_l);
//    bluetooth.print("**");
//    bluetooth.print(vLeft);
//    bluetooth.print("**");
//    bluetooth.print(distance_f);
//    bluetooth.print("**");
//    bluetooth.print(vRight);
//    bluetooth.print("**");
//    bluetooth.print(distance_r);
//    bluetooth.println("**");
}

void callback1(void)
{
    if(hardware::pins::left_encoder_a::read() != hardware::pins::left_encoder_b::read())
    {
        counterA++;
    }
    else
    {
         counterA--;
    }
    bluetooth.print("CounterA is: ");
    bluetooth.print(counterA);
    bluetooth.print(" A is: ");
    bluetooth.print((unsigned int)hardware::pins::left_encoder_a::read());
    bluetooth.print(" B is: ");
    bluetooth.println((unsigned int)hardware::pins::left_encoder_b::read());
    if(goalA!=0 && counterA==(int)(goalA/5.625)) // 135/24=5.625
    {
//        bluetooth.println("Left motor stop!");
        vLeft = 0.0;
        hardware::left_motor::stop();
        goalA=0;
//        if (goalB==0) 
//        {
            vRight = 0;
            goalB = 0;
            hardware::right_motor::stop();
            motion_mode = MOTION_STOP;
//            bluetooth.println("Set to motion stop in callback1");
//        }
    }
}

void callback2(void)
{
    if(hardware::pins::right_encoder_a::read() != hardware::pins::right_encoder_b::read())
    {
        counterB++;
    }
    else
    {
        counterB--;
    }
      bluetooth.print("CounterB is: ");
      bluetooth.print(counterB);
    bluetooth.print(" A is: ");
    bluetooth.print((unsigned int)hardware::pins::right_encoder_a::read());
    bluetooth.print(" B is: ");
    bluetooth.println((unsigned int)hardware::pins::right_encoder_b::read());
    if(goalB!=0 && counterB==(int)(goalB/5.625))
    {
//        bluetooth.println("Right motor stop!");
        vRight = 0.0;
        hardware::right_motor::stop();
        goalB=0;
//        if (goalA==0) 
//        {
            goalA = 0;
            vLeft = 0;
            hardware::left_motor::stop();
            motion_mode = MOTION_STOP;
//            bluetooth.println("Set to motion stop in callback2");
//        }
    }
}