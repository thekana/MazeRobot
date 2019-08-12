#include "Wire.h"
#include "Sensor.h"
#include "MazeExp.h"
#include "Maze.h"
#include "LiquidCrystal_I2C.h"
#include "locomotion.h"
//#include <hd44780.h>                       // main hd44780 header
//#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
#define bluetooth Serial3
// Define LCD pinout
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

// Define I2C Address - change if reqiuired
const int i2c_addr = 0x3F;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//
//hd44780_I2Cexp lcd;
//String inMSG = " ";
//const int LCD_COLS = 16;
//const int LCD_ROWS = 2;

bool nextCellflag = true;
float Yaw;
long current, timeIntervel;
String keyword;
int motion_mode = MOTION_STOP;
//Variable that indicate the car to move , stop(0), front(1), left_turning(2), right_turning(3), back_turning(4)


extern int counterA;
extern int counterB;
int enCounter = 0;

using namespace hardware::pins;

//store the start corner of the maze
int start[2];  
int move_dir;
int face_dir;
byte heading = 4;
int startStep = 0;
int cellCount = 0;
//MazeCell maze[5][9];
Maze *mazePrint = new Maze("");
coord curCoord = {0,0};
bool cellflag = true;
int celldis = 0;
byte mazeType = 0;
MazeExp *newMaze = new MazeExp();
bool mazeFound = false;

void setup() {
  // put your setup code here, to run once:

  // join I2C bus (I2Cdev library doesn't do this automatically)
//  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//      Wire.begin();
//      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
//  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//      Fastwire::setup(400, true);
//  #endif
  bluetooth.begin(115200);
  bluetooth.setTimeout(100);
  lcd.begin(16, 2);
  bluetooth.println();
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); 
  sensorSetup();
  locomotion_setup();
//  initLCD();
//  Serial.println("LCD   Connect!");
//  lcd.print("Ready");
  current = millis();
  timeIntervel = 0;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HELLO");
  lcd.setCursor(0, 1);
  lcd.print("WORLD");
  
//  bluetooth.println("Enter short or long side...");
//  while (bluetooth.available()==0){delay(100);} // empty buffer    
////  while (bluetooth.available()==0){delay(100);} // empty buffer
//  if(bluetooth.available() > 0){
//      keyword = bluetooth.readString();
//      if(keyword.equals("s\n"))
//        heading = 4;
//      bluetooth.print("Heading = ");bluetooth.println(heading);
//  }
//  bluetooth.println("End setup!");
  
  delay(1000);
  // wait for ready
  Serial.println(F("\nSend any question number to start demo: "));  
//  while (Serial.available() && Serial.read()); // empty buffer
//  while (!Serial.available());                 // wait for data
////  while (Serial.available() && Serial.read()); // empty buffer again
//
//  keyword = Serial.read();  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  double distance_f, distance_l, distance_r;      
  //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise
  int lfr[3];    
  //Variable that indicate the car to move front(0), left(1), right(2), back(3)                       
  move_dir = 0;
  //Varianle that indicate which direction (East(1), South(2), West(3),North(4)) the car is facing
  face_dir = 0;
  //Array be used to determine whether there is al wall at East, South, West and North
  int ESWN[4] = {0, 0, 0, 0};  
  //front distance (ultrasonic distance)                                
  distance_f = sonar<analog_pin<digital_pin<38>>,analog_pin<digital_pin<39>>>::distance().count();         //convert the CM to MM
  //left distance (left lidar)
  distance_l = lidar<lidar_tag<0>>::distance().count();
  //right distance (right lidar)
  distance_r = lidar<lidar_tag<1>>::distance().count();

  
  //if can reach here, motion_mode = stop
  
  //function that dectect the left, front and right wall near the car
  carWall(distance_l, distance_f, distance_r, lfr); 
  //Set IMU parameters
//  imu::update();
  Yaw = imu::yaw();
  //function that records the walls near car (Ease, South, West, North)
//  ESWNWall(Yaw, lfr, ESWN,&face_dir);
  
   if(startStep==0){
     ledR::high();
     ledG::low();
     startStep = 1;
     current = millis();
     timeIntervel = 0; 
  }
  else if(startStep==1){
    if(timeIntervel < 2000){
      if(distance_f <= 50){
        timeIntervel = millis() - current;  
      }else{
        current = millis();
        timeIntervel = 0;
      }
      Serial.print("Sonic: ");
      Serial.println(distance_f);
    }else if(distance_f > 50){
        bluetooth.println("Car start");
        ledR::low();
        ledG::high();
        delay(1000);
        startStep = 2;
    }else{
      timeIntervel = millis() - current;
    }
  }else if(startStep == 2){
    if(lfr[2]==0){
      mazeType = 2;
      curCoord.x = 8;
      newMaze->setDestX(0);
      newMaze->setDestY(8);
      newMaze->instantiate(); 
      startStep = 3;
    }else if(lfr[0]==1){
        forward();
        cellCount++;
        Serial.print("CellCount: "); Serial.println(cellCount);
//        bluetooth.println("Stop");
        delay(7000);
    }else{
      mazeType = 1; 
      newMaze->setDestX(8);
      newMaze->setDestY(8);
      newMaze->instantiate();   
      startStep = 3;
    }
  }else if(startStep == 3){
    Serial.print("MazeType: ");Serial.println(mazeType);
    if(cellCount!=0){
      curCoord.y = cellCount;
      for(int i=0;i<cellCount;i++){
        newMaze->maze[i][curCoord.x].walls = 10;  
      } 
      cellCount = 0;
    }
    if(newMaze->maze[curCoord.y][curCoord.x].distance != 0){
      for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
          Serial.print(newMaze->maze[i][j].distance);
          Serial.print("\t");
        }  
        Serial.println("");
      }
      Serial.print("Current cell distance: ");  Serial.println(newMaze->maze[curCoord.y][curCoord.x].distance);  
      Serial.print("front: "); Serial.println(distance_f);
      Serial.print("left: "); Serial.println(distance_l);
      Serial.print("right: ");Serial.println(distance_r);
      Serial.print("Wall: ");Serial.print(lfr[0]);Serial.print(lfr[1]);Serial.println(lfr[2]);
      Serial.print("Current cell: [");Serial.print(curCoord.y);
      Serial.print(" "); Serial.print(curCoord.x);Serial.println("] ");
      newMaze->floodFillUpdate(curCoord, heading, lfr);
//      mazePrint->fillCells(curCoord.y,curCoord.x,maze[curCoord.y][curCoord.x].walls);
      int nextHeading = newMaze->orient(curCoord, heading);
      coord nextCoord = newMaze->bearingCoord(curCoord, nextHeading);
      
      
      Serial.print("next cell: ["); Serial.print(nextCoord.y);
      Serial.print(" "); Serial.print(nextCoord.x);Serial.println("] "); 
      Serial.print("Current heading: "); Serial.println(heading);
      curCoord = nextCoord;
      heading = nextHeading;
      
      Serial.print("next heading: "); Serial.println(heading);
      Serial.println();  
      if(mazeType == 1&&mazeFound==false){
        Serial.println("calc distance!");
        if(curCoord.x > 4){
            newMaze->setDestX(4);
            newMaze->setDestY(2);
            newMaze->reloadDist();
            mazeFound = true;
            Serial.print("DestX: ");Serial.println(newMaze->getDestX());
            Serial.print("DestY: ");Serial.println(newMaze->getDestY());
        }else if(curCoord.y > 4){
            newMaze->setDestX(2);
            newMaze->setDestY(4);
            newMaze->reloadDist();
            mazeFound = true;
            Serial.print("DestX: ");Serial.println(newMaze->getDestX());
            Serial.print("DestY: ");Serial.println(newMaze->getDestY());
        }
      }else if(mazeType == 2&&mazeFound==false){
        Serial.println("calc distance!");
        if(curCoord.x < 4){
            newMaze->setDestX(4);
            newMaze->setDestY(2);
            newMaze->reloadDist();
            mazeFound = true;
            Serial.print("DestX: ");Serial.println(newMaze->getDestX());
            Serial.print("DestY: ");Serial.println(newMaze->getDestY());
        }else if(curCoord.y > 4){
            newMaze->setDestX(6);
            newMaze->setDestY(4);
            newMaze->reloadDist();
            mazeFound = true;
            Serial.print("DestX: ");Serial.println(newMaze->getDestX());
            Serial.print("DestY: ");Serial.println(newMaze->getDestY());
        }
      }
      delay(10000);
    }else{
      Serial.println("end of maze. ready to print.");
      newMaze->floodFillUpdate(curCoord, heading, lfr);
//      mazePrint->fillCells(curCoord.y,curCoord.x,maze[curCoord.y][curCoord.x].walls);
      ledG::low();
      ledR::high();
      while(bluetooth.available()==0){}
      
      //GOAL FINDED, STOP CAR, end execution
      startStep = 4;
    }
  }else if(startStep==4){
    Serial.println("End");
    delay(100);  
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void forward(){
  counterA = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Move");
  lcd.setCursor(0, 1);
  lcd.print("Forward");
  while(counterA < 38){
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    delay(500);
  }
//  bluetooth.println("Forward");
}

void turnR(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Right");
  counterA = 0;
  counterB = 0;
  while(true){
    if((counterA >= 12) || (counterB <= -12)){
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}

void turnL(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Left");
  counterA = 0;
  counterB = 0;
  while(true){
    if((counterA <= -12) || (counterB >= 12)){
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}
void turnB(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Back");
  counterA = 0;
  counterB = 0;
  while(true){
    if((counterA <= -24) || (counterB >= 24)){
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}
