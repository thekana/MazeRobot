#include "hardware.h"

extern int counterA;
extern int counterB;

using namespace hardware::pins;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(19200);
    while (! Serial);
    Serial.println("hello");
	  
	hardware::led::config_io_mode(hardware::io_mode::output);
    hardware::led::high();
    delay(2000);
    hardware::led::low();
    Serial.println("Finish!");

    hardware::left_encoder::enable();
    hardware::pins::left_encoder_a::attach_interrupt(&callback1, hardware::interrupt_mode::change);
    hardware::left_motor::enable();
    units::percentage full_speed(100);
    hardware::left_motor::forward(full_speed);
    delay(5000);
    hardware::left_motor::stop();
    units::millimeters distance = hardware::left_wheel::position();
    Serial.println(distance.count());

    hardware::right_encoder::enable();
    hardware::pins::right_encoder_a::attach_interrupt(&callback2, hardware::interrupt_mode::change);
    hardware::right_motor::enable();
    units::percentage half_speed(50);
    hardware::right_motor::backward(half_speed);
    delay(5000);
    hardware::right_motor::stop();
    distance = hardware::right_wheel::position();
    Serial.println(distance.count());
    
}

void loop() {
    // put your main code here, to run repeatedly:
    
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
}
