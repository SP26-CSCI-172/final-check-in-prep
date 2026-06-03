#include <IRremote.hpp>
#include <LiquidCrystal.h>

// motor driver pins
#define ENA 9
#define IN1 12
#define IN2 11
#define ENB 6
#define IN3 7
#define IN4 8

// ultrasonic + LED pins
const int trigPin = 10;
const int echoPin = 13;
const int ledPin = 23;
const int ledPintwo = 24;

// IR receiver pin
const int irPin = 41;

// ir pins
const int ir1 = A11;  // far left
const int ir2 = A12;  // mid left
const int ir3 = A13;  // center
const int ir4 = A14;  // mid right
const int ir5 = A15;  // far right

//current state constants
const int STC = 0; //state center
const int STL = 1; //state turn left
const int SHL = 2; //state turn hard left
const int STR = 3; //state turn right
const int SHR = 4; //state turn hard right
const int STOP = 5; //stop

int currentState = STC;
int lastDirection = 0;  // -1 = left, 0 = center, +1 = right

const int TURN_SPEED       = 100; //turn speed constants for IR
const int SHARP_TURN_SPEED = 150;


// LCD pins: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(53, 51, 49, 47, 45, 43);

int pressMode = 3;  // holds values 1-8; start in avoid mode

// IR hex codes
const unsigned long BUTTON_1 = 0xBA45FF00;      // line following
const unsigned long BUTTON_2 = 0xB946FF00;      // obstacle avoidance
const unsigned long BUTTON_3 = 0xB847FF00;      // dance
const unsigned long BUTTON_4 = 0xBB44FF00;      // sleep / stop
const unsigned long BUTTON_UP = 0xE718FF00;     // forward
const unsigned long BUTTON_RIGHT = 0xA55AFF00;  // right
const unsigned long BUTTON_DOWN = 0xAD52FF00;   // backward
const unsigned long BUTTON_LEFT = 0xF708FF00;   // left

#define SPEED 190         // motor speed
#define TURN_TIME 800     // turn duration in ms
#define STOP_DISTANCE 60  // obstacle distance in cm

void setup() {
  // motor pins
  pinMode(ENA, OUTPUT); //sets ENA pin to output
  pinMode(IN1, OUTPUT); //sets IN1 pin to output
  pinMode(IN2, OUTPUT); //sets IN2 pin to output
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // sensor + LED pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPintwo, OUTPUT);

  // serial for distance debugging
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("    Power on    ");
  delay(2000);

  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);
  delay(500);
}

void moveForward() {  // drive forward  
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(200);
}

void moveBack() {  // drive backward
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(200);
}

void turnRight() {  // pivot right for TURN_TIME
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(TURN_TIME);
}

void turnLeft() {  // pivot left for TURN_TIME
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(TURN_TIME);
}

void moveStop() {  // stop all motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(200);
}

void warningLights() {  // flash the two LEDs
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
  digitalWrite(ledPintwo, HIGH);
  delay(200);
  digitalWrite(ledPintwo, LOW);
  delay(200);
}

void mad() {  // show mad face on LCD
  lcd.setCursor(0, 0);
  lcd.print("  >:# move!     ");
}

void happy() {  // show happy face on LCD
  lcd.setCursor(0, 0);
  lcd.print("    Hi!!! :3    ");
}

void danceMode() {
  for (int i = 0; i < 2; i++) {  // dance movement
    moveForward();
    delay(500);
    moveBack();
    delay(500);
    moveStop();
    turnRight();
    delay(1000);
    moveForward();
    delay(500);
    moveBack();
    delay(500);
    moveStop();
    turnLeft();
    delay(1000);
  }
  turnRight();  // final spin
  delay(8000);
  moveStop();

  lcd.setCursor(0, 0);
  lcd.print("   Tadaa!! :D   ");
  delay(2000);
}

int checkDistance() {  // measure distance in cm
  long duration;
  int distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);  // read echo pulse
  distanceCm = duration / 58;         // convert time to distance
  return distanceCm;
}

void avoidMode() {  // obstacle avoidance
  int distanceCm = checkDistance();

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  if (distanceCm <= STOP_DISTANCE) {  // obstacle ahead
    moveStop();
    warningLights();
    moveBack();
    mad();
    turnRight();  // already turns for TURN_TIME
    moveStop();
  }
  else {  // path is clear
    moveForward();
    happy();
  }
}

void lineFollow() {
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
    currentState = STOP;
  }

}

void loop() {
  // read the remote and update the mode
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

  // run the selected mode
  switch (pressMode) {
    case 1:  // line following (stub - not yet implemented)
      lcd.setCursor(0, 0);
      lcd.print("   Line Mode    ");
      moveStop();
      lineFollow();
      break;

    case 2:  // obstacle avoidance
      lcd.setCursor(0, 0);
      lcd.print("   Avoid Mode   ");
      avoidMode();
      break;

    case 3:  // dance once, then idle
      lcd.setCursor(0, 0);
      lcd.print("   Dance Mode   ");
      moveStop();
      danceMode();
      pressMode = 4;  // stop after one routine
      break;

    case 4:  // sleep / stop
      lcd.setCursor(0, 0);
      lcd.print("   Sleeping...  ");
      moveStop();
      break;

    case 5:  // forward
      lcd.setCursor(0, 0);
      lcd.print("    Forward     ");
      moveForward();
      break;

    case 6:  // right
      lcd.setCursor(0, 0);
      lcd.print("     Right      ");
      turnRight();
      break;

    case 7:  // backward
      lcd.setCursor(0, 0);
      lcd.print("    Backward    ");
      moveBack();
      break;

    case 8:  // left
      lcd.setCursor(0, 0);
      lcd.print("     Left       ");
      turnLeft();
      break;
  }

  delay(100);
}
