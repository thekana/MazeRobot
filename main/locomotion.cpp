/**
 * @file locomotion.cpp
 * @author Liangde Li
 * @date 16 July 2019
 * @brief The functions for leds and locomotion system
 *
 * @note 
 * */
#include "locomotion.h"

extern int counterA;
extern int counterB;
extern int motion_mode;

int goalA=0;
int goalB=0;

void callback1(void);
void callback2(void);

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
        goalA=-68;
        counterA=0;
        goalB=68;
        counterB=0;
        units::percentage test_speed(50);
        hardware::left_motor::backward(test_speed);
        hardware::right_motor::forward(test_speed);
    }
    else if (motion_mode == MOTION_RIGHT)
    {
        goalA=68;
        counterA=0;
        goalB=-68;
        counterB=0;
        units::percentage test_speed(50);
        hardware::left_motor::forward(test_speed);
        hardware::right_motor::backward(test_speed);
    }
    else if (motion_mode == MOTION_BACK)
    {
        goalA=-141;
        counterA=0;
        goalB=141;
        counterB=0;
        units::percentage test_speed(50);
        hardware::left_motor::backward(test_speed);
        hardware::right_motor::forward(test_speed);
    }
    else
    {
        Serial.println("Warning: Motion mode not properly setup in turning!");
    }
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
    Serial.print("CounterA is: ");
    Serial.print(counterA);
    Serial.print(" A is: ");
    Serial.print((unsigned int)hardware::pins::left_encoder_a::read());
    Serial.print(" B is: ");
    Serial.println((unsigned int)hardware::pins::left_encoder_b::read());
    if(goalA!=0 && counterA==(int)(goalA/5.625)) // 135/24=5.625
    {
        hardware::left_motor::stop();
        goalA=0;
        if (goalB==0) motion_mode = MOTION_STOP;
    }
}

void forward(int ncells, double distance_f, double distance_l, double distance_r)
{
    goalA=250;
    counterA=0;
    goalB=250;
    counterB=0;
    units::percentage test_speed(50);
    hardware::left_motor::forward(test_speed);
    hardware::right_motor::forward(test_speed);
}

void forward_updating(double distance_f, double distance_l, double distance_r)
{
    
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
  	Serial.print("CounterB is: ");
  	Serial.print(counterB);
    Serial.print(" A is: ");
    Serial.print((unsigned int)hardware::pins::right_encoder_a::read());
    Serial.print(" B is: ");
    Serial.println((unsigned int)hardware::pins::right_encoder_b::read());
    if(goalB!=0 && counterB==(int)(goalB/5.625))
    {
        hardware::right_motor::stop();
        goalB=0;
        if (goalA==0) motion_mode = MOTION_STOP;
    }
}
