int start = 1;
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(22, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  
  if(start==1)
  {
    Serial.println("Start!");
    digitalWrite(22, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(2000);                       // wait for a second
    digitalWrite(22, LOW);    // turn the LED off by making the voltage LOW
    start=0;
    Serial.println("Finish!"); 
  }       
               
}
