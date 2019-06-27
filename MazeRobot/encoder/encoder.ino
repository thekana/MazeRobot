int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;

int En1A = 2;
int En1B = 8;
int En2A = 3;
int En2B = 9;

int counterA = 0;
int counterB = 0;

#define M1_FORWARD LOW
#define M1_BACKWARD HIGH
#define M2_FORWARD LOW
#define M2_BACKWARD HIGH

void setup() {
  Serial.begin(19200);
  while (! Serial);
  Serial.println("hello");
  // put your setup code here, to run once:
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(En1A, INPUT_PULLUP);
  pinMode(En1B, INPUT);
  pinMode(En2B, INPUT);
  attachInterrupt(digitalPinToInterrupt(En1A), callback1, CHANGE);
  pinMode(En2A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(En2A), callback2, CHANGE);
  digitalWrite(M1, M1_FORWARD);
  analogWrite(E1, 100);
  digitalWrite(M2, M2_FORWARD);
  analogWrite(E2, 100);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void callback1()
{
	counterA++;
  Serial.print("CounterA is: ");
  Serial.print(counterA);
  Serial.print(", A is ");
  Serial.print(digitalRead(En1A));
  Serial.print(", B is ");
  Serial.println(digitalRead(En1B));
}

void callback2()
{
  counterB++;
  Serial.print("CounterB is:");
  Serial.print(counterB);
  Serial.print(", A is ");
  Serial.print(digitalRead(En2A));
  Serial.print(", B is ");
  Serial.println(digitalRead(En2B));
  if(counterB==120)
  {
    analogWrite(E2, 00);
  }
}
