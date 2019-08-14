#include "Wire.h"
#include "Sensor.h"
#include "MazeExp.h"
//#include "Maze.h"
#include "LiquidCrystal_I2C.h"
#include "locomotion.h"
#include "cppQueue.h"
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
static Queue motion_queue(sizeof(int));
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
//Maze *mazePrint = new Maze();
coord curCoord = {0,0};
//coord initCoord;
bool cellflag = true;
int celldis = 0;
byte mazeType = 0;
MazeExp *newMaze = new MazeExp();
bool mazeFound = false;
int lfr[3];
double distance_f, distance_l, distance_r; 
int motion_decision = 0;

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
//  bluetooth.print((unsigned int)hardware::pins::left_encoder_a::read());
  //Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise 
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
  motion_decision = 0;
  int ncells;
  ncells = 0;
  int current_mode;
  current_mode = motion_mode;

  if(motion_mode >= MOTION_LEFT) return;
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
      bluetooth.print("Sonic: ");
      bluetooth.println(distance_f);
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
//      initCoord = curCoord;
      startStep = 3;
    }else if(lfr[0]==1){
//        forward();

        if(motion_mode==MOTION_STOP)
        {
          if(motion_queue.isEmpty())
          {
            bluetooth.println("Queue empty!");
            motion_decision = MOTION_FORWARD;
            if (!motion_queue.isEmpty())
            {
              bluetooth.println("Warning! Queue not empty!");
            }

            motion_queue.push(&motion_decision);
            bluetooth.print("Push: ");
            bluetooth.println(motion_decision);
          
            if(motion_decision>=2) 
            {
              motion_decision = 1;
              motion_queue.push(&motion_decision);
            }
         
            cellCount++;
            bluetooth.print("CellCount: "); bluetooth.println(cellCount);
          }
          
          if(!motion_queue.isEmpty())
          {
              bluetooth.println("Delay start!");
              delay(1000);
              bluetooth.println("Delay end!");
              motion_queue.pop(&motion_mode);
              if (motion_mode==MOTION_FORWARD) ncells=1;
              bluetooth.print("Pop: ");
              bluetooth.println(motion_mode);
              bluetooth.println("Queue not empty");
          }
        }
        
//        bluetooth.println("Stop");
//        delay(7000);
    }else{
      mazeType = 1; 
      newMaze->setDestX(8);
      newMaze->setDestY(8);
      newMaze->instantiate();   
//      initCoord = curCoord;
      startStep = 3;
    }
  }else if(startStep == 3){
//    motion_decision = 0;
    if(motion_mode==MOTION_STOP)
    {
        if(motion_queue.isEmpty())
        {
          bluetooth.println("Queue empty!");
          exploration();
        }
        if(!motion_queue.isEmpty())
        {
            bluetooth.println("Delay start!");
            delay(1000);
            bluetooth.println("Delay end!");
            motion_queue.pop(&motion_mode);
            if (motion_mode==MOTION_FORWARD) ncells=1;

            bluetooth.print("Pop: ");
            bluetooth.println(motion_mode);
            bluetooth.println("Queue not empty");
        }
      }
    }else if(startStep==4){
//      for(byte i=0;i<9;i++){
//        for(byte j=0;j<9;j++){
//          if(newMaze->maze[i][j].detected==true)
//           mazePrint->fillCells_steven(i,j,newMaze->maze[i][j].walls);
//        }
//      }
  //    mazePrint->updateStatusCells(initCoord.y, initCoord.x, "S", newMaze->getDestY(), newMaze->getDestX());
//      mazePrint->print();
      bluetooth.println("End");
      startStep = 5;
//      delay(100);  
    }

    if (motion_mode >= MOTION_LEFT)
    {
  //         bluetooth.println("Turning");
        turning();
    }
    else if (current_mode == MOTION_STOP && motion_mode == MOTION_FORWARD)
    {
//        bluetooth.println("Forward");
        forward(ncells, distance_f, distance_l, distance_r);
    }
    else if (current_mode == MOTION_FORWARD && motion_mode == MOTION_FORWARD)
    {
        forward_updating(distance_f, distance_l, distance_r);
    }
    else
    {
        Serial.println("Motion status not recognized!");
    }
    
//    delay(1000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void exploration(){
//  bluetooth.print("MazeType: ");bluetooth.println(mazeType);
    if(cellCount!=0){
      curCoord.y = cellCount;
      for(int i=0;i<cellCount;i++){
        newMaze->maze[i][curCoord.x].walls = 10;  
      } 
      cellCount = 0;
    }
    if(newMaze->maze[curCoord.y][curCoord.x].distance != 0){
//      bluetooth.print("Current cell distance: ");  bluetooth.println(newMaze->maze[curCoord.y][curCoord.x].distance);  
      bluetooth.print("front: "); bluetooth.println(distance_f);
      bluetooth.print("left: "); bluetooth.println(distance_l);
      bluetooth.print("right: ");bluetooth.println(distance_r);
      bluetooth.print("Wall: ");bluetooth.print(lfr[0]);bluetooth.print(lfr[1]);bluetooth.println(lfr[2]);
      bluetooth.print("Current cell: [");bluetooth.print(curCoord.y);
      bluetooth.print(" "); bluetooth.print(curCoord.x);bluetooth.println("] ");
      newMaze->floodFillUpdate(curCoord, heading, lfr);
      newMaze->maze[curCoord.y][curCoord.x].detected = true;
//      mazePrint->fillCells_steven(curCoord.y,curCoord.x,newMaze->maze[curCoord.y][curCoord.x].walls);
      int nextHeading = newMaze->orient(curCoord, heading);
      coord nextCoord = newMaze->bearingCoord(curCoord, nextHeading);
      
      
      bluetooth.print("next cell: ["); bluetooth.print(nextCoord.y);
      bluetooth.print(" "); bluetooth.print(nextCoord.x);bluetooth.println("] "); 
      bluetooth.print("Current heading: "); bluetooth.println(heading);
      curCoord = nextCoord;

      if(heading == nextHeading){
        //Move forward one cell
        motion_decision = MOTION_FORWARD;
      }else{
        switch (heading) {
          case 1: //N
            if(nextHeading==2){
                motion_decision = MOTION_LEFT;
            }else if(nextHeading == 4){
                motion_decision = MOTION_BACK;  
            }else if(nextHeading == 8){
                motion_decision = MOTION_RIGHT;
            }
            break; 
          case 4: //S
            if(nextHeading==2){
                motion_decision = MOTION_RIGHT;
            }else if(nextHeading == 8){
                motion_decision = MOTION_LEFT;  
            }else if(nextHeading == 1){
                motion_decision = MOTION_BACK;
            }
 
            break;
          case 8: //E
            if(nextHeading==4){
                motion_decision = MOTION_RIGHT;
            }else if(nextHeading == 1){
                motion_decision = MOTION_LEFT;  
            }else if(nextHeading == 2){
                motion_decision = MOTION_BACK;
            }

            break;
          case 2: //W
            if(nextHeading==1){
                motion_decision = MOTION_RIGHT;
            }else if(nextHeading == 4){
                motion_decision = MOTION_LEFT;  
            }else if(nextHeading == 8){
                motion_decision = MOTION_BACK;
            }
            //turn then move forward one cell
            break;
        }
//              nextCellflag = false;
      }
      heading = nextHeading;
      bluetooth.print("next heading: "); bluetooth.println(heading);
      bluetooth.println();  
      if(mazeType == 1&&mazeFound==false){
        bluetooth.println("calc distance!");
        if(curCoord.x > 4){
            newMaze->setDestX(4);
            newMaze->setDestY(2);
            newMaze->reloadDist();
            newMaze->floodFillUpdate(curCoord, heading, lfr);
            mazeFound = true;
            bluetooth.print("DestX: ");bluetooth.println(newMaze->getDestX());
            bluetooth.print("DestY: ");bluetooth.println(newMaze->getDestY());
        }else if(curCoord.y > 4){
            newMaze->setDestX(2);
            newMaze->setDestY(4);
            newMaze->reloadDist();
            newMaze->floodFillUpdate(curCoord, heading, lfr);
            mazeFound = true;
            bluetooth.print("DestX: ");bluetooth.println(newMaze->getDestX());
            bluetooth.print("DestY: ");bluetooth.println(newMaze->getDestY());
        }
      }else if(mazeType == 2&&mazeFound==false){
        bluetooth.println("calc distance!");
        if(curCoord.x < 4){
            newMaze->setDestX(4);
            newMaze->setDestY(2);
            newMaze->reloadDist();
            mazeFound = true;
            bluetooth.print("DestX: ");bluetooth.println(newMaze->getDestX());
            bluetooth.print("DestY: ");bluetooth.println(newMaze->getDestY());
        }else if(curCoord.y > 4){
            newMaze->setDestX(6);
            newMaze->setDestY(4);
            newMaze->reloadDist();
            mazeFound = true;
            bluetooth.print("DestX: ");bluetooth.println(newMaze->getDestX());
            bluetooth.print("DestY: ");bluetooth.println(newMaze->getDestY());
        }
      }
//      delay(6000);
    }else{
      bluetooth.println("end of maze. ready to print.");
      motion_mode = MOTION_STOP;
      newMaze->floodFillUpdate(curCoord, heading, lfr);
//      mazePrint->fillCells_steven(curCoord.y,curCoord.x,newMaze->maze[curCoord.y][curCoord.x].walls);
      ledG::low();
      ledR::high();
      while(bluetooth.available()==0){}
      
      //GOAL FINDED, STOP CAR, end execution
      startStep = 4;
    }  

    if (!motion_queue.isEmpty())
    {
      bluetooth.println("Warning! Queue not empty!");
      delay(100000);
    }
    
    motion_queue.push(&motion_decision);
    bluetooth.print("Push: ");
    bluetooth.println(motion_decision);
  
    if(motion_decision>=2) 
    {
      motion_decision = 1;
      motion_queue.push(&motion_decision);
    }
}

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
