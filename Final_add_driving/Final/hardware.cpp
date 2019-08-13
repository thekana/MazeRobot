#include "external_VL6180X.h"
#include "external_I2CIO.h"

#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
#define FORWARD logic_level::low
#define BACKWARD logic_level::high
#define INTERRUPT_PIN 18

int counterA = 0;
int counterB = 0;

VL6180X *left = new VL6180X();
VL6180X *right = new VL6180X();
MPU6050 *mpu = new MPU6050();

using namespace hardware;
using namespace hardware::pins;

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
//#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
#define OUTPUT_READABLE_WORLDACCEL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}


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
//  if(distance > 1000){
//    Serial.println("Ultrasonic out of range!");
//  }else{
    return units::millimeters(distance);  
//  } 
}

template <typename tag>
auto lidar<tag>::enable()->void{
  if(is_same<tag,lidar_tag<0>>::value){
    digital_pin<48>::write(logic_level::high);
    delay(50);
    left->init();
    left->configureDefault();
    left->setTimeout(500);
    left->setAddress(left_lidar_address);  
  }else if(is_same<tag,lidar_tag<1>>::value){
    digital_pin<49>::write(logic_level::high);
    delay(50);
    right->init();
    right->configureDefault();
    right->setTimeout(500);
    right->setAddress(right_lidar_address);  
  }
}

template <typename tag>
auto lidar<tag>::distance()->units::millimeters{
  if(is_same<tag,lidar_tag<0>>::value){
    return units::millimeters(left->readRangeSingleMillimeters());
  }else if(is_same<tag,lidar_tag<1>>::value){
    return units::millimeters(right->readRangeSingleMillimeters());
  }else{
    Serial.println("Error reading lidars");
    return units::millimeters(0);  
  }
}

auto imu::enable()->bool{
   // initialize device
//  Serial.println(F("Initializing I2C devices..."));
  mpu->initialize();
  imu_interrupt::config_io_mode(io_mode::input);
//  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
//  Serial.println(F("Testing device connections..."));
//  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
  // load and configure the DMP
//  Serial.println(F("Initializing DMP..."));
  devStatus = mpu->dmpInitialize();

  mpu->setXGyroOffset(-131);   //Roll offset
  mpu->setYGyroOffset(104);    //pitch offset
  mpu->setZGyroOffset(145);      //Yaw offset
  mpu->setXAccelOffset(-2838); // 1688 factory default for my test chip
  mpu->setYAccelOffset(-1500); // 1688 factory default for my test chip
  mpu->setZAccelOffset(1736); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu->setDMPEnabled(true);

      // enable Arduino interrupt detection
      Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
//      Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
//      Serial.println(F(")..."));
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu->getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
//      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu->dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      dmpReady = false;
  }  
  return dmpReady;
}

auto imu::update()->bool{
   // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {
      if (mpuInterrupt && fifoCount < packetSize) {
        // try to get out of the infinite loop 
        fifoCount = mpu->getFIFOCount();
      }  
      
      // other program behavior stuff here
      // .
      // .
      // .
      // if you are really paranoid you can frequently test in between other
      // stuff to see if mpuInterrupt is true, and if so, "break;" from the
      // while() loop to immediately process the MPU data
      // .
      // .
      // .
  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu->getIntStatus();

  // get current FIFO count
  fifoCount = mpu->getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
      // reset so we can continue cleanly
      mpu->resetFIFO();
      fifoCount = mpu->getFIFOCount();
//      Serial.println(F("FIFO overflow!"));

  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu->getFIFOCount();

      // read a packet from FIFO
      mpu->getFIFOBytes(fifoBuffer, packetSize);
      
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;
  }
  return true;  
}

auto imu::yaw()->float{
  mpu->dmpGetQuaternion(&q, fifoBuffer);
  mpu->dmpGetGravity(&gravity, &q);
  mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);
  return  ypr[0] * 180/M_PI;
}

auto imu::pitch()->float{
  mpu->dmpGetQuaternion(&q, fifoBuffer);
  mpu->dmpGetGravity(&gravity, &q);
  mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);
  return  ypr[1] * 180/M_PI;
}

auto imu::roll()->float{
  mpu->dmpGetQuaternion(&q, fifoBuffer);
  mpu->dmpGetGravity(&gravity, &q);
  mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);
  return  ypr[2] * 180/M_PI;
}

auto imu::stabilize()->void{
  int update_count = 0;
  while(true){
    if(update_count == 400){
      break;  
    }else{
      imu::update();
      update_count++;
    }
  }
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
  pin_a::write(BACKWARD);
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
template class hardware::digital_pin<23U>;
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

template class lidar<lidar_tag<0>>;
template class lidar<lidar_tag<1>>;
template class digital_pin<48>;
template class digital_pin<49>;
template class digital_pin<18>;
template class digital_pin<52>;
template class digital_pin<53>;
template class analog_pin<digital_pin<38>>;
template class analog_pin<digital_pin<39>>;
template class sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>;
