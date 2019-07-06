#include <SoftwareSerial.h>
SoftwareSerial BT(2, 3); 
// #define BT Serial1
// creates a "virtual" serial port/UART
// connect BT module TX to D2
// connect BT module RX to D3
// connect BT Vcc to 5V, GND to GND
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
