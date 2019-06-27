#include "hardware.h"
#include "hardware_definition.h"
#include "Arduino.h"
#include "external_VL6180X.h"

VL6180X left;
VL6180X right;

using namespace hardware;
using namespace hardware::pins;

template<pin_t pin>
auto digital_pin<pin>::config_io_mode(io_mode mode)->void {
  switch(mode){
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

template<pin_t pin>
auto digital_pin<pin>::read()->logic_level {
  logic_level hl;
  int input = digitalRead(pin_number);
  if(input == LOW){
    hl = logic_level::high;  
  }else if(input == HIGH){
    hl = logic_level::low;  
  }else{
    Serial.println("No input level");  
  }
	return hl;
}

template<pin_t pin>
auto digital_pin<pin>::write(logic_level level) -> void {
  if(level == logic_level::high){
    digitalWrite(pin_number,HIGH);
  }else if(level == logic_level::low){
    digitalWrite(pin_number,LOW);  
  }else{
    Serial.println("No write level");  
  }
}

template<pin_t pin>
auto digital_pin<pin>::high() -> void {
	digitalWrite(pin_number, HIGH);
}

template<pin_t pin>
auto digital_pin<pin>::low() -> void {
	digitalWrite(pin_number, LOW);
}

template<pin_t pin>
auto digital_pin<pin>::pwm_write(units::percentage duty_cycle) -> void {
	double percent = units::percentage(duty_cycle).count() / 0.392;
	analogWrite(pin_number,percent);
}

template<pin_t pin>
auto digital_pin<pin>::pulse_length(logic_level state = logic_level::high,
	units::microseconds timeout = 1000000_us)->units::microseconds {
  long duration = pulseIn(pin_number,HIGH,1000000);
	return units::microseconds(duration);
}

template <class trigger_pin, class echo_pin>
auto sonar<trigger_pin,echo_pin>::enable () -> void{
  trigger_pin::config_io_mode(io_mode::output);
  echo_pin::config_io_mode(io_mode::input); 
  Serial.println("Ultrasonic pins set finished!"); 
}

template <class trigger_pin, class echo_pin>
auto sonar<trigger_pin,echo_pin>:: distance () -> units::millimeters{
  double duration, distance;
  trigger_pin::low();
  delayMicroseconds(2);
  trigger_pin::high();
  delayMicroseconds(10);
  trigger_pin::low();
  duration = pulseIn(echo_pin::pin_number,HIGH);
  distance = duration / 58.3 * 10;
  if(distance > 1000){
    Serial.println("Ultrasonic out of range!");
  }else{
    return units::millimeters(distance);  
  } 
}

template <typename tag>
auto lidar<tag>::enable()->void{
  if(is_same<tag,lidar_tag<0>>::value){
    digital_pin<48>::write(logic_level::high);
    delay(50);
    left.init();
    left.configureDefault();
    left.setTimeout(500);
    left.setAddress(left_lidar_address);  
  }else if(is_same<tag,lidar_tag<1>>::value){
    digital_pin<49>::write(logic_level::high);
    delay(50);
    right.init();
    right.configureDefault();
    right.setTimeout(500);
    right.setAddress(right_lidar_address);  
  }
}

template <typename tag>
auto lidar<tag>::distance()->units::millimeters{
  if(is_same<tag,lidar_tag<0>>::value){
    return units::millimeters(left.readRangeSingleMillimeters());
  }else if(is_same<tag,lidar_tag<1>>::value){
    return units::millimeters(right.readRangeSingleMillimeters());
  }else{
    Serial.println("Error reading lidars");
    return units::millimeters(0);  
  }
}

template class lidar<lidar_tag<0>>;
template class lidar<lidar_tag<1>>;
template class digital_pin<48>;
template class digital_pin<49>;
template class digital_pin<18>;
template class analog_pin<digital_pin<38>>;
template class analog_pin<digital_pin<39>>;
template class sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>;
