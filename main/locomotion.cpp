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
    if(goalA!=0 && counterA==goalA/135*24)
    {
        hardware::left_motor::stop();
        goalA=0;
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
  	Serial.print("CounterB is: ");
  	Serial.print(counterB);
    Serial.print(" A is: ");
    Serial.print((unsigned int)hardware::pins::right_encoder_a::read());
    Serial.print(" B is: ");
    Serial.println((unsigned int)hardware::pins::right_encoder_b::read());
    if(goalB!=0 && counterB==goalB/135*24)
    {
        hardware::right_motor::stop();
        goalB=0;
    }
}
