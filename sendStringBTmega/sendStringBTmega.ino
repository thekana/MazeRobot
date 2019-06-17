#include <stdio.h>
#define BT Serial1
// connect BT module TX to 19
// connect BT module RX to 18
// connect BT Vcc to 5V, GND to GND
void setup()  
{
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  BT.setTimeout(5000); //5secs
  // Send test message to other device
  BT.println("Hello from Arduino");
  Serial.begin(9600);
  Serial.println("Waiting Bluetooth Message");
}
char a; // stores incoming character from other device
String c;
String f = "Construct";
void loop() 
{
  if (BT.available())
  // if text arrived in from BT serial...
  // read every character
  {
    while(BT.available()){
      a = BT.read();
      c = String(c+a);
      delay(10);
      BT.flush();
    }
    Serial.print(c);
    if ( c.equals("MAZE\n") ){
      Serial.println("In Maze Constructor Mode");
      boolean quit = 0;
      while(!quit){
        BT.println("Input Maze Width:");
        int w = BT.parseInt();
        BT.println("Input Maze Height:");
        int h = BT.parseInt();
        BT.println("Input Maze Name:");
        String s = BT.readString();
        Serial.println("Input Data are");
        char buff[100];
        sprintf(buff, "Width is %d, Height is %d, %s is Name", w, h, s.c_str());
        Serial.println(buff);
        BT.println("Do you want to quit Maze Constructor Mode? Y/N");
        waitBTInput();
        if(BT.peek() == 'Y' || BT.peek() == 'y'){
          BT.println("Quiting Clearing Receive Buffer");
          clearBTInBuffer();
          Serial.println("In Message Mode");
          quit = 1;
        }
      }
    }
    delay(10);
    c = "";
  }
  if (Serial.available()){
    while(Serial.available()){
      a = Serial.read();
      c = String(c+a);
      delay(10);
      Serial.flush();
    }
    BT.println(c);
    delay(10);
    c = "";
  }
}

int getIntSerialBT(){
  int a;
}

String getStringSerialBT()
{
  String str = String("");
  while(BT.available()){
    str = BT.readString();
  }
  return str;
}

void waitBTInput()
{
  while(!BT.available()){}
}

void clearBTInBuffer(){
  while(BT.read() >= 0){}
}
