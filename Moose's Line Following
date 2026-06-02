#include <LiquidCrystal.h>

//define pins 
#define ENA 9
#define IN1 12 
#define IN2 11
#define ENB 6
#define IN3 7
#define IN4 8

const int trigPin = 10;
const int echoPin = 13;
const int buzzerPin = A0;
const int ledPin = 23;
const int ledPintwo = 24;

const int ir1 = A0;  // far left
const int ir2 = A1;  // mid left
const int ir3 = A2;  // center
const int ir4 = A3;  // mid right
const int ir5 = A4;  // far right

const int STC = 0;
const int STL = 1;
const int SHL = 2;
const int STR = 3;
const int SHR = 4;
const int STLO = 5;

int currentState = STC;
int lastDirection = 0;  // -1 = left, 0 = center, +1 = right

const int TURN_SPEED       = 100;
const int SHARP_TURN_SPEED = 150;

const int rs = 25, en = 2, d4 = 3, d5 = 26, d6 = 4, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define SPEED 190 //set motor speed
#define TURN_TIME 800
#define STOP_DISTANCE 20

void moveForward(){ //declare forward function 
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW); 
  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW); 
  delay(200); // 1 second delay
}

void moveBack(){ //declare forward function
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, HIGH);
  delay(200);
}

void turnRight(){ //declare turn right function 
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(TURN_TIME); 
}

void turnLeft(){ // declare turn left funtion
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(TURN_TIME); 
}

void sharpTurnLeft() {
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  //TODO: set motor direction for sharp left turn (pivot left)
  //   IN1=LOW  IN2=HIGH  IN3=HIGH IN4=LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void sharpTurnRight() {
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  //TODO: set motor direction for sharp right turn (pivot right)
  //   IN1=HIGH IN2=LOW   IN3=LOW  IN4=HIGH
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveStop(){ // declare stop function
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(200); 
}

void buzzerAlarm(){ //function in charge of buzzing the alarm.
  analogWrite(buzzerPin, 300);
  delay(500);
  analogWrite(buzzerPin, 0);
}

void warningLights(){ //function in charge of flashing the lights
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPintwo, HIGH);
  delay(200);
  digitalWrite(ledPintwo, LOW);
  delay(200);
}

void mad(){ //prints mad expression on LCD
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("  >:# movee!  ");
}

void happy(){ // prints happy expression on LCD
  lcd.setCursor(0, 0);
  lcd.print("    Hi!!! :3");
  delay(1500);
}

int checkDistance(){ // fucntion used to measure and check distance 
  long duration;
  int distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //read echo pulse 
  duration = pulseIn(echoPin, HIGH);

  //convert time to distance 
  distanceCm = duration / 58;

  return distanceCm;
}

void setup() {
  pinMode(ENA, OUTPUT); //sets ENA pin to output
  pinMode(IN1, OUTPUT); //sets IN1 pin to output
  pinMode(IN2, OUTPUT); //sets IN2 pin to output
  pinMode(ENB, OUTPUT); //sets ENB pin to output
  pinMode(IN3, OUTPUT); //sets IN3 pin to output
  pinMode(IN4, OUTPUT); //sets IN4 pin to output

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPintwo, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);

  delay(500);
}

void loop(){

  int distanceCm = checkDistance();
  Serial.begin(9600);
  Serial.print("Distance: ");0
  Serial.println(distanceCm);
  Serial.println(" cm");
  delay(200);
  warningLights();

  if(distanceCm <= STOP_DISTANCE){
    moveStop();
    delay(100);
    moveBack();
    delay(100);
    buzzerAlarm();
    delay(100);
    mad();
    buzzerAlarm(); 
    delay(100);

    turnRight();
    delay(TURN_TIME);

    moveStop();
    delay(200);
  }
// Start of Moose's edits
  else if(distanceCm >= STOP_DISTANCE) {
    int s1 = digitalRead(ir1);
    int s2 = digitalRead(ir2);
    int s3 = digitalRead(ir3);
    int s4 = digitalRead(ir4);
    int s5 = digitalRead(ir5);

    Serial.print(s1);
    Serial.print(s2);
    Serial.print(s3);
    Serial.print(s4);
    Serial.println(s5);

    if (s3 == HIGH) {
      currentState = STC;
      lastDirection = 0;
    }
    else if (s2 == HIGH) {
      currentState = STL;
      lastDirection = -1;
    }
    else if (s1 == HIGH) {
      currentState = SHL;
      lastDirection = -1;
    }
    else if (s4 == HIGH) {
      currentState = STR;
      lastDirection = +1;
    }
    else if (s5 == HIGH) {
      currentState = SHR;
      lastDirection = +1;
    }
    else {
      currentState = STLO;
    }

    switch(currentState) {
      
      case STC:
        moveForward();
        break;
        
      case STL:
        turnLeft();
        break;
        
      case SHL:
        sharpTurnLeft();
        break;
        
      case STR:
        turnRight();
        break;
        
      case SHR:
        sharpTurnRight();
        break;
        
      case STLO:
        moveStop();
        break;
      
    }
  }
  // End of Moose's edit
  else{
    moveForward();
    analogWrite(buzzerPin, 0);
    happy();
  }
}



