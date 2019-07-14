#include "hardware.h"
#include "Arduino.h"

#define FORWARD logic_level::low
#define BACKWARD logic_level::high

int counterA = 0;
int counterB = 0;

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::config_io_mode (hardware::io_mode mode) -> void
{
  	switch(mode)
  	{
	    case io_mode::input:
	        pinMode(pin_number,INPUT); 
	        break;
	    case io_mode::output:
	        pinMode(pin_number,OUTPUT);
	        break;
	    case io_mode::input_pullup:
	        pinMode(pin_number,INPUT_PULLUP);
	        break;
	    case io_mode::unset:
	        Serial.print("Pin unset");
	        break;
  	}
}

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::read()->logic_level {
	int reading = digitalRead(pin_number);
	if(reading == LOW)
	{
		return logic_level::low;  
	}
	else 
	{
		return logic_level::high;  
	}
}

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::write(logic_level level) -> void {
    if(level == logic_level::high){
    	digitalWrite(pin_number,HIGH);
    }else if(level == logic_level::low){
        digitalWrite(pin_number,LOW);  
    }else{
        Serial.println("No write level");  
    }
}

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::high() -> void {
	digitalWrite(pin_number, HIGH);
}

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::low() -> void {
	digitalWrite(pin_number, LOW);
}

template <hardware::pin_t pin>
auto hardware::digital_pin<pin>::pwm_write (units::percentage duty_cycle) -> void
{
	analogWrite(pin_number, (int)(duty_cycle.count()*2.55));
    //Serial.println((int)(duty_cycle.count()*2.55));
}

template <typename pin>
auto hardware::interrupt<pin>::attach_interrupt (void (*callback) (), interrupt_mode mode) -> void
{
    switch(mode)
    {
        case interrupt_mode::low:
            attachInterrupt(digitalPinToInterrupt(pin::pin_number), callback, LOW);
            break;
        case interrupt_mode::change:
            attachInterrupt(digitalPinToInterrupt(pin::pin_number), callback, CHANGE);
            break;
        case interrupt_mode::rising:
            attachInterrupt(digitalPinToInterrupt(pin::pin_number), callback, RISING);
            break;
        case interrupt_mode::falling:
            attachInterrupt(digitalPinToInterrupt(pin::pin_number), callback, FALLING);
            break;
        default:
            break;
    }
}

template <class pin_a, class pin_b>
auto hardware::motor<pin_a, pin_b>::enable () -> void
{
    pin_a::config_io_mode(io_mode::output);
}

template <class pin_a, class pin_b>
auto hardware::motor<pin_a, pin_b>::stop () -> void
{
    units::percentage zero_percent(0);
    pin_b::pwm_write(zero_percent);
}

template <class pin_a, class pin_b>
auto hardware::motor<pin_a, pin_b>::forward (units::percentage velocity) -> void
{
	pin_a::write(FORWARD);
    pin_b::pwm_write(velocity);
}

template <class pin_a, class pin_b>
auto hardware::motor<pin_a, pin_b>::backward (units::percentage velocity) -> void
{
	pin_a::write(FORWARD);
    pin_b::pwm_write(velocity);
}

template <typename pin_a, typename pin_b>
auto hardware::encoder<pin_a, pin_b>::enable () -> void
{
    encoder_pin_a::config_io_mode(io_mode::input_pullup);
    encoder_pin_b::config_io_mode(io_mode::input);
}

template <typename pin_a, typename pin_b>
auto hardware::encoder<pin_a, pin_b>::count () -> encoder_count
{
    if (pin_a::pin_number==2u)
        return counterA;
    else
        return counterB;
}

template <typename pin_a, typename pin_b>
auto hardware::wheel<pin_a, pin_b>::position () -> units::millimeters
{
    hardware::encoder_count count = hardware::encoder<pin_a, pin_b>::count();
    units::millimeters result(count*135/24);
    return result;
}

template class hardware::digital_pin<22U>;
template class hardware::motor<hardware::pins::M1, hardware::pins::E1>;
template class hardware::motor<hardware::pins::M2, hardware::pins::E2>;
template class hardware::digital_pin<2U>;
template class hardware::interrupt<hardware::digital_pin<2U>>;
template class hardware::digital_pin<8U>;
template class hardware::digital_pin<3U>;
template class hardware::interrupt<hardware::digital_pin<3U>>;
template class hardware::digital_pin<9U>;
template class hardware::encoder<hardware::pins::left_encoder_a, hardware::pins::left_encoder_b>;
template class hardware::encoder<hardware::pins::right_encoder_a, hardware::pins::right_encoder_b>;
template class hardware::wheel<hardware::pins::left_encoder_a, hardware::pins::left_encoder_b>;
template class hardware::wheel<hardware::pins::right_encoder_a, hardware::pins::right_encoder_b>;
