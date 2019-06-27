#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
using namespace hardware;
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
 * ========================================================================= */

/* =========================================================================
   NOTE: Arduino v1.0.1 with the Leonardo board generates a compile error
   when using Serial.write(buf, len). The Teapot output uses this method.
   The solution requires a modification to the Arduino USBAPI.h file, which
   is fortunately simple, but annoying. This will be fixed in the next IDE
   release. For more info, see these links:
   http://arduino.cc/forum/index.php/topic,109987.0.html
   http://code.google.com/p/arduino/issues/detail?id=958
 * ========================================================================= */



// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
//#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

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

#define INTERRUPT_PIN 18  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

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
float Yaw = 0;

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

#define trigPin 38
#define echoPin 39

VL6180X sensor1;
VL6180X sensor2;
long current, timeIntervel;
char keyword;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>::enable();
  digital_pin<48>::config_io_mode(io_mode::output);
  digital_pin<49>::config_io_mode(io_mode::output);
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  digital_pin<18>::config_io_mode(io_mode::input);
//  pinMode(INTERRUPT_PIN, INPUT);

  digital_pin<48>::write(logic_level::low);
  digital_pin<49>::write(logic_level::low);

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  mpu.setXGyroOffset(-152);   //Roll offset
  mpu.setYGyroOffset(97);    //pitch offset
  mpu.setZGyroOffset(-85);      //Yaw offset
  mpu.setXAccelOffset(-2928); // 1688 factory default for my test chip
  mpu.setYAccelOffset(-1502); // 1688 factory default for my test chip
  mpu.setZAccelOffset(1811); // 1688 factory default for my test chip


  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
      Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
      Serial.println(F(")..."));
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }
  lidar<lidar_tag<0>>::enable();

  delay(100);
  
  lidar<lidar_tag<1>>::enable();
  
  current = millis();
  timeIntervel = 0;
  
  delay(1000);
  // wait for ready
  Serial.println(F("\nSend any question number to start demo: "));
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available());                 // wait for data
//  while (Serial.available() && Serial.read()); // empty buffer again

  keyword = Serial.read();  
}

void loop() {
  // put your main code here, to run repeatedly:
  //
  double distance_f, distance_l, distance_r;      //Variables to recored the distance values that obtained by sensors
  int lfr[3];                            //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively
  int ESWN[4] = {0, 0, 0, 0};                                  //Array be used to determine whether there is al wall at East, South, West and North
  distance_f = sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>::distance().count();         //convert the CM to MM
//  distance_f = distance_f*10;
  distance_l = lidar<lidar_tag<0>>::distance().count();
  distance_r = lidar<lidar_tag<1>>::distance().count();
  if(sensor1.timeoutOccurred()){
    Serial.println("SENSOR LEFT TIMEOUT");  
  }
  if(sensor2.timeoutOccurred()){
    Serial.println("SENSOR RIGHT TIMEOUT");  
  }
  if(distance_f < 150){
    lfr[1] = 1;
  }else{
    lfr[1] = 0;
  }

  if(distance_l < 150){
    lfr[0] = 1;
  }else{
    lfr[0] = 0;  
  }
//  delay(20);
 
  if(distance_r < 150){
    lfr[2] = 1;
  }else{
    lfr[2] = 0;  
  }
  setIMU();
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  Yaw = ypr[0] * 180/M_PI;
   if(Yaw > -22.5 && Yaw < 22.5){
      if(lfr[1]!=0){
        ESWN[3] = 1;
      }else{
        ESWN[3] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[2] = 1;  
      }else{
        ESWN[2] = 0;  
      }
      if(lfr[2]!=0){
        ESWN[0] = 1;  
      }else{
        ESWN[0] = 0;  
      }
      ESWN[1] = 0;
//      Serial.println("C0");
  }else if(Yaw > 67.5 && Yaw < 112.5){
      if(lfr[1]!=0){
        ESWN[0] = 1;
      }else{
        ESWN[0] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[3] = 1;  
      }else{
        ESWN[3] = 0;  
      }
      if(lfr[2]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0;  
      }
      ESWN[2] = 0;
//      Serial.println("C1");
  }else if(Yaw < -67.5 && Yaw > -112.5){
      if(lfr[1]!=0){
        ESWN[2] = 1;
      }else{
        ESWN[2] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0; 
      }
      if(lfr[2]!=0){
        ESWN[3] = 1;  
      }else{
        ESWN[3] = 0;  
      }
//      Serial.println("C2");
      ESWN[0] = 0;
  }else if(Yaw > 157.5 && Yaw < -157.5){
      if(lfr[1]!=0){
        ESWN[1] = 1;
      }else{
        ESWN[1] = 0;  
      }
      if(lfr[0]!=0){
        ESWN[0] = 1;  
      }else{
        ESWN[0] = 0;
      }
      if(lfr[2]!=0){
        ESWN[1] = 1;  
      }else{
        ESWN[1] = 0;  
      }
      ESWN[3] = 0;
//      Serial.println("C3");
  }
    
  if(keyword == '1'){
    //Ultrasonic
    Serial.print("Front: ");
    Serial.print(distance_f);
    Serial.print(" mm\t");
    //Lidar left
    Serial.print("Left: ");
    Serial.print(distance_l);
    Serial.print(" mm\t");
    //Lidar right
    Serial.print("Right: ");
    Serial.print(distance_r);
    Serial.println(" mm");
    displayIMUValues();
  }else if(keyword == '2'){
    if(timeIntervel < 2000){                //Detect 2s
      if(distance_f < 50){
        timeIntervel = millis() - current;  //detect the timeIntervel that object is put within 5cm distance at the front of ultrasonic
      }else{
        current = millis();
        timeIntervel = 0;
      }
    }else if(distance_f > 50){
      Serial.println("Start!");
      current = millis();
      timeIntervel = 0;
    }else{
      timeIntervel = millis() - current;  
    }
    Serial.print("Ultrasonic: ");
    Serial.print(distance_f);
    Serial.println(" mm\t");
  }else if(keyword == '3'){
    Serial.print(" "); 
    Serial.print("Wall around: ");
    for(int i=0;i<3;i++){
      Serial.print(lfr[i]);
      Serial.print(" ");  
    }
    Serial.println("");
  }else if(keyword == '4'){
    Serial.print("Yaw: ");
    Serial.println(Yaw);
    Serial.print("Wall in ESWN: ");
    for(int i=0;i<4;i++){
      Serial.print(ESWN[i]);
      Serial.print("\t");
    }
  
    Serial.println("");
  }
  
  delay(500);
}

void displayIMUValues(){
   #ifdef OUTPUT_READABLE_QUATERNION
        // display quaternion values in easy matrix form: w x y z
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        Serial.print("quat\t");
        Serial.print(q.w);
        Serial.print("\t");
        Serial.print(q.x);
        Serial.print("\t");
        Serial.print(q.y);
        Serial.print("\t");
        Serial.println(q.z);
    #endif
  
    #ifdef OUTPUT_READABLE_EULER
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        Serial.print("euler\t");
        Serial.print(euler[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(euler[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(euler[2] * 180/M_PI);
    #endif
  
    #ifdef OUTPUT_READABLE_YAWPITCHROLL
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        Serial.print("Yaw:");
        Serial.print(ypr[0] * 180/M_PI);
        Serial.print("\tPitch: ");
        Serial.print(ypr[1] * 180/M_PI);
        Serial.print("\tRoll: ");
        Serial.println(ypr[2] * 180/M_PI);
    #endif
  
    #ifdef OUTPUT_READABLE_REALACCEL
        // display real acceleration, adjusted to remove gravity
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        Serial.print("arealX:");
        Serial.print(aaReal.x);
        Serial.print("\tarealY: ");
        Serial.print(aaReal.y);
        Serial.print("\tarealZ: ");
        Serial.println(aaReal.z);
    #endif
  
    #ifdef OUTPUT_READABLE_WORLDACCEL
        // display initial world-frame acceleration, adjusted to remove gravity
        // and rotated based on known orientation from quaternion
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
        Serial.print("aworldX: ");
        Serial.print(aaWorld.x);
        Serial.print("\taworldY: ");
        Serial.print(aaWorld.y);
        Serial.print("\taworldZ: ");
        Serial.println(aaWorld.z);
    #endif
}

void setIMU(){
   // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {
      if (mpuInterrupt && fifoCount < packetSize) {
        // try to get out of the infinite loop 
        fifoCount = mpu.getFIFOCount();
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
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      fifoCount = mpu.getFIFOCount();
//      Serial.println(F("FIFO overflow!"));

  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;
  }
}
