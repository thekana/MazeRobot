int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;

int EnA = 2;
int EnB = 3;

int counterA = 0;
int counterB = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(EnA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EnA), callbackA, RISING);
  // Start

  while (! Serial);

  // M1 run forward for 5 seconds
  counterA=0;
  digitalWrite(M1, HIGH);
  analogWrite(E1, 255);
  analogWrite(E2, 0);
  delay(50000);

  // M2 run backward for 5 seconds
  digitalWrite(M2, LOW);
  analogWrite(E1, 0);
  analogWrite(E2, 128);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value;
  for (value=0; value<=255; value+=5)
  {
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, value);
    analogWrite(E2, 0);
    delay(30);
  }
}

void callbackA()
{
  counterA++;
  Serial.print("CounterA is: ");
  Serial.print(counterA);
  Serial.print(", degree is: ");
  Serial.print(counterA*30);
  Serial.println("deg.");
  if (counterA==836/30)
  {
    analogWrite(E1, 0);
  }
}
