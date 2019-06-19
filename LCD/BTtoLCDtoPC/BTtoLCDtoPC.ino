#include <SoftwareSerial.h>
SoftwareSerial BT(2, 3);
// #define BT Serial1
// creates a "virtual" serial port/UART
// connect BT module TX to D2
// connect BT module RX to D3
// connect BT Vcc to 5V, GND to GND
//////////////////////////////////////////////
// Include Wire Library for I2C
#include <Wire.h>
// Include NewLiquidCrystal Library for I2C
#include <LiquidCrystal_I2C.h>

// Define LCD pinout
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

// Define I2C Address - change if reqiuired
const int i2c_addr = 0x3F;

LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);
//////////////////////////////////////////////
void setup()  
{
  // set digital pin to control as an output
  pinMode(LED_BUILTIN, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");
  Serial.begin(9600);
  Serial.println("Waiting");
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
}
char a; // stores incoming character from other device
char b;
String c;
void loop() 
{
  if (BT.available())
  // if text arrived in from BT serial...
  {
    if (BT.peek()=='1'){
      digitalWrite(LED_BUILTIN, HIGH);
      BT.println("LED on");
    }else if (BT.peek()=='2'){
      digitalWrite(LED_BUILTIN, LOW);
      BT.println("LED off");
    }else if (BT.peek()=='?'){
      BT.println("Send '1' to turn LED on");
      BT.println("Send '2' to turn LED on");
    } else {
      c = BT.readString();
      Serial.println(c);
      delay(10);
      lcdPrintRoutine(c);
    }
    c = "";
    clearBTbuffer();
    // you can add more "if" statements with other characters to add more commands
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
  lcd.print(str);
  delay(1500);
  lcd.autoscroll();
  // print from 0 to 9:
  for (int thisChar = 0; thisChar < 10; thisChar++) {
    lcd.print(thisChar+1);
    delay(500);
    }
  // turn off automatic scrolling
  lcd.noAutoscroll();
}
