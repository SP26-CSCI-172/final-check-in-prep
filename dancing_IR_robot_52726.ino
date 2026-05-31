#include <IRremote.hpp>
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

//IR pin setup
const int irPin = 53;

const int rs = 25, en = 2, d4 = 3, d5 = 26, d6 = 4, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool pressMode; //different integer values for each press

//IR Hex codes
const unsigned long BUTTON_1 = 0xBA45FF00;  // line following
const unsigned long BUTTON_2 = 0xB946FF00;  // obstacle avoidance
const unsigned long BUTTON_3 = 0xB847FF00;  // dance button
const unsigned long BUTTON_4 = 0xBB44FF00;  // sleep
const unsigned long BUTTON_UP = 0xE718FF00; // forward
const unsigned long BUTTON_RIGHT = 0xA55AFF00; // right
const unsigned long BUTTON_DOWN = 0xAD52FF00; // backward
const unsigned long BUTTON_LEFT = 0xF708FF00; // left

#define SPEED 190 //set motor speed
#define TURN_TIME 800
#define STOP_DISTANCE 40

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

int avoidMode() {
  int distanceCm = checkDistance(); 
    Serial.begin(9600);
    Serial.print("Distance: ");
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

    else{
      moveForward();
      analogWrite(buzzerPin, 0);
      happy();
    }
  
}

void loop(){
  
  //ir receiver buttons
  if (IrReceiver.decode()) {
    unsigned long buttonCode = IrReceiver.decodedIRData.decodedRawData;

    switch (buttonCode) {
      case BUTTON_1:
        pressMode = 1;
        break;
      case BUTTON_2:
        pressMode = 2;
        break;
      case BUTTON_3:
        pressMode = 3;
        break;
      case BUTTON_4:
        pressMode = 4;
        break;
      case BUTTON_UP:
        pressMode = 5;
        break;
      case BUTTON_RIGHT:
        pressMode = 6;
        break;
      case BUTTON_DOWN:
        pressMode = 7;
        break;
      case BUTTON_LEFT:
        pressMode = 8;
        break;
    }
    
    IrReceiver.resume();
  }

  switch (pressMode) { // button function switch
    case 1:
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(" Line Mode  ");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(" Avoid Mode  ");
      moveStop();
      avoidMode();
      break;
    case 3:
      moveStop();
      for (int i = 0; i < 4; i++) { //dance movement
        moveForward();
        delay(1000);
        moveBack();
        delay(1000);
        moveStop();
        turnRight();
        delay(1500);
        moveForward();
        delay(1000);
        moveBack();
        delay(1000);
        moveStop();
        turnLeft();
        delay(1500);
      }
      turnRight(); //final spin
      delay(8000);
      moveStop();
      
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("   Tadaa!! :D   ");
      delay(1500);

      break;
    case 5: // up
      lcd.clear();
      moveForward();
      break;
    
    case 6: // right
      lcd.clear();
      turnRight();
      break;
    
    case 7: // down
      lcd.clear();
      moveStop();
      break;
    
    case 8: // left
      lcd.clear();
      turnLeft();
      break;

  }
}
