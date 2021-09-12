#include <Servo.h>
#include <RBD_Button.h>
#include <RBD_Timer.h>

/*

  SupraSwing
  2021

  Press btn B (middle) long to start auto test mode


  Jens Weber

  Arduino 1.8.9
  For Arduino Uno 

*/

int testMode = 0;

// LED
int ledPin = 9;

// Sensor
int sensorPin = 10;

// Buttons
RBD::Button btnA(2);
RBD::Button btnB(4);
RBD::Button btnC(7);

// Servos
int servoPinA = 3;
int servoPinB = 5;
int servoPinC = 6;
Servo servoA;
Servo servoB;
Servo servoC;

// Servo config
int posLow = 6; // servo 0..180
int _delay = 350; // ms 

int secondDelay = 720; // ms // second hit
boolean secondDelayEnabled = !true;

int posHigh = 174;  // servo 0..180


RBD::Timer everySecondTimer;
unsigned long offTime = 80000; // ms blocked
unsigned long lastHitTimer;
int everySecond = 0;
long randNumber;
boolean isBlocked = false;

void setup() {
  Serial.begin(9600);

  everySecondTimer.setTimeout(1000);
  everySecondTimer.restart();

  pinMode (sensorPin, INPUT_PULLUP);
  pinMode (ledPin, OUTPUT);

  // servos
  servoA.attach(servoPinA);
  servoB.attach(servoPinB);
  servoC.attach(servoPinC);

  // go to zero position
  servoA.write(posLow);
  servoB.write(posLow);
  servoC.write(posLow);

  // off
  delay(_delay + 500);
  servoA.detach();
  servoB.detach();
  servoC.detach();
}

void loop() {
  
  // test mode
  if (testMode > 0) {
    
    if (testMode == 2) {
      triggerServo("A");
      testMode = 3;
    } else if (testMode == 3) {
      triggerServo("B");
      testMode = 4;
    } else if (testMode == 4) {
      triggerServo("C");
      testMode = 2;
    }
    
    if (testMode == 1) {
      // start testmode if btn B long pressed
      if (btnB.isPressed()) {
        testMode = 2;
        Serial.println( "testmode" );
      } else {
        testMode = 0;  // reset longpress
      }
    }
  } else {
  
  
    // manual mode
    if (btnA.onPressed()) triggerServo2("A", "B"); //triggerServo("A");
    if (btnB.onPressed()) {
      triggerServo("B");
      testMode = 1; // start long press
      Serial.println( "longpress" );
    }
    if (btnC.onPressed()) triggerServo3(); //triggerServo("C");
  
    // auto mode
    isBlocked = lastHitTimer + offTime > millis();
    if (!isBlocked) {
      checkSensor();
    }
  
    if(everySecondTimer.onRestart()) {
//        Serial.print(millis());
//        Serial.print( " " );
//        Serial.print( isBlocked );
//        Serial.print( " " );
//        Serial.println( lastHitTimer );
  
      
      if (!isBlocked) {
        // wait for random
        shuffle();
        flash();
      } else {
        // blocked
        digitalWrite(ledPin, HIGH);
        Serial.print("blocked ");
        Serial.print( (offTime - millis() + lastHitTimer)/1000 );
        Serial.println( "s " );
      }
      
    }

  }
}



void triggerServo(String id) {
  Servo servo = getServo(id);
  int servoPin = getServoPin(id);
  
  // on
  servo.attach(servoPin);
  // up and down
  servo.write(posHigh); 
  delay(_delay);
  servo.write(posLow);

  // off
  delay(_delay + 100);
  servo.detach();

  Serial.print(">>> ");
  Serial.println(id);
  lastHitTimer = millis();
}

void triggerServo2(String id, String id2) {
  Servo servo = getServo(id);
  int servoPin = getServoPin(id);
  Servo servo2 = getServo(id2);
  int servoPin2 = getServoPin(id2);
  
  // on
  servo.attach(servoPin);
  servo2.attach(servoPin2);
  // up and down
  servo.write(posHigh); 
  servo2.write(posHigh); 
  delay(_delay);
  servo.write(posLow);
  servo2.write(posLow);

  // off
  delay(_delay + 100);
  servo.detach();
  servo2.detach();
  
  Serial.print(">>> ");
  Serial.print(id);
  Serial.println(id2);
  lastHitTimer = millis();
}

void triggerServo3() {
  // on
  servoA.attach(servoPinA);
  servoB.attach(servoPinB);
  servoC.attach(servoPinC);
  // up and down
  servoA.write(posHigh); 
  servoB.write(posHigh); 
  servoC.write(posHigh); 
  delay(_delay);
  servoA.write(posLow);
  servoB.write(posLow);
  servoC.write(posLow);

  // off
  delay(_delay + 100);
  servoA.detach();
  servoB.detach();
  servoC.detach();
  
  Serial.println(">>> 3x");
  lastHitTimer = millis();
}




void triggerRandom() {
  long r = random(7); 
  if (r == 0)      triggerServo("A");
  else if (r == 1) triggerServo("B");
  else if (r == 2) triggerServo("C");
  else if (r == 3) triggerServo2("A","B");
  else if (r == 4) triggerServo2("A","C");
  else if (r == 5) triggerServo2("B","C");
  else if (r == 6) triggerServo3();
}

// every 1 sec
void shuffle() {
  Serial.println("shuffle");
  randNumber = random(1000);
  if (randNumber < 10) {
    triggerRandom();
  }
}

void flash() {
  digitalWrite(ledPin, HIGH); 
  delay(50);
  digitalWrite(ledPin, LOW);
}

void checkSensor() {
  boolean sensor = digitalRead(sensorPin);
  if (sensor) {
    triggerRandom();
    Serial.println("sensor active");
  }
}

Servo getServo(String id) {
  return (id == "A") ? servoA : (id == "B") ? servoB : servoC;
}

int getServoPin(String id) {
  return (id == "A") ? servoPinA : (id == "B") ? servoPinB : servoPinC;
}
