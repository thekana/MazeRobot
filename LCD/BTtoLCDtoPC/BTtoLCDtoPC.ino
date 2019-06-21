//#include <SoftwareSerial.h>
//SoftwareSerial BT(2, 3);
#define BT Serial1
// creates a "virtual" serial port/UART
// connect BT module TX to D2
// connect BT module RX to D3
// connect BT Vcc to 5V, GND to GND
//////////////////////////////////////////////
// Include Wire Library for I2C
#include <Wire.h>
// Include NewLiquidCrystal Library for I2C
#include "LiquidCrystal_I2C.h"

// Include personal library
#include "hardware.h"
#include "hardware_definition.h"

// Define LCD pinout
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

// Define I2C Address - change if reqiuired
const int i2c_addr = 0x3F;
hardware::cell_location cell(6, 9, 1, 1, 1, 1);
hardware::maze_layout_message mazeMesg;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//////////////////////////////////////////////
void setup()  
{
  // set the data rate for the SoftwareSerial port
  BT.begin(115200);
  BT.setTimeout(1000);
  // Send test message to other device
  BT.println("Hello from Arduino");
  Serial.begin(115200);
  Serial.println("Waiting");
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HELLO");
  lcd.setCursor(0,1);
  lcd.print("WORLD");
}
char a; // stores incoming character from other device
char b;
String c;
void loop() 
{
  if (BT.available())
  {
    c = BT.readString();
    Serial.print(c);    //if not println must send string with LF from phone
    //Serial.println(cell.toString());
    delay(10);
    lcdPrintRoutine(c);
    if ( c.equals("MAZE\n")){
      Serial.println("In Maze Constructor Mode");
      waitBTInput();
      c = "";
      c = BT.readString();
      c.remove(c.length()-1);
      mazeMesg.setMessage(c);
      Serial.println(mazeMesg.toString());
	    hardware::maze_layout layout = hardware::parse_maze_layout(mazeMesg);
      layout.print();
    }
    c = "";
    clearBTbuffer();
  }
  if (Serial.available()){
    b = Serial.read();
    BT.write(b);
    Serial.flush();
  }
}

void clearBTbuffer(){
  while(BT.available()){
    BT.read();
  }
}

void lcdPrintRoutine(String str){
  lcd.clear();
  lcd.setCursor(0,0);
  str.remove(str.length()-1);
  if (str.length() <= 15){
    lcd.print(str);
    delay(500);
    return;
  }
  ///////////////////////////////
   /* Split into two lines
  lcd.print(str.substring(0,16));
  lcd.setCursor(0,1);
  lcd.print(str.substring(16));
  */
  //////////////////////////////
  /*lcd.print(str.substring(0,16));
  delay(1500);
  String sub1 = str.substring(16);
  Serial.println(sub1);
  lcd.autoscroll();
  for(int i = 0; i < sub1.length();i++){
    lcd.print(sub1.charAt(i));
    Serial.print(sub1.charAt(i));
    delay(400);
  }
  lcd.noAutoscroll();
  ///////////////////////////////*/
  lcd.setCursor(16,0);
  lcd.autoscroll();
  for(int i = 0; i < str.length();i++){
    lcd.print(str.charAt(i));
    Serial.print(str.charAt(i));
    delay(400);
  }
  Serial.println("");
  lcd.noAutoscroll();
}
void waitBTInput()
{
  while(!BT.available()){}
}
