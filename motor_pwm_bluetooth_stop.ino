#include <SoftwareSerial.h>

// Bluetooth: Arduino RX = D2, TX = D3.
SoftwareSerial HM10(2, 3);

// Left motor direction pins.
int in1 = 9;
int in2 = 8;

// Right motor direction pins.
int in3 = 11;
int in4 = 10;

// Remove ENA and ENB jumpers; connect ENA to D5 and ENB to D6.
int ena = 5;
int enb = 6;

// PWM duty cycles: level 1 = 25%, 2 = 50%, 3 = 75%, 4 = 100%.
// These are power settings, not measured wheel speeds.
const int speedLevels[5] = {0, 64, 128, 191, 255};
int leftLevel = 3;
int rightLevel = 3;
bool motorsRunning = false;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);

  Stop(); // Keep both motors stopped at startup.
  Serial.begin(9600);
  HM10.begin(9600);
  Serial.println("AutoNova-car ready for input.");
}

void loop() {
  if (HM10.available()) {
    char c = HM10.read();
    Serial.print("RECEIVED: ");
    Serial.println(c);

    if (c == 'A' || c == 'a') {
      Forward();
    }
    else if (c == 'C' || c == 'c') {
      Backward();
    }
    else if (c == 'B' || c == 'b') {
      TurnRight();
    }
    else if (c == 'D' || c == 'd') {
      TurnLeft();
    }
    // Triangle: increase both speed levels, up to 4.
    else if (c == 'E' || c == 'e') {
      if (leftLevel < 4) leftLevel++;
      if (rightLevel < 4) rightLevel++;
      ApplySpeed();
    }
    // X: decrease both speed levels, down to 1.
    else if (c == 'G' || c == 'g') {
      if (leftLevel > 1) leftLevel--;
      if (rightLevel > 1) rightLevel--;
      ApplySpeed();
    }
    // Circle: stop both motors, keeping the selected speed levels.
    else if (c == 'F' || c == 'f') {
      Stop();
    }
    // Square H is unassigned. Other characters are ignored.
  }
}

// Speed changes take effect immediately while moving.
// While stopped, speed buttons change the level without restarting motors.
void ApplySpeed() {
  analogWrite(ena, motorsRunning ? speedLevels[leftLevel] : 0);
  analogWrite(enb, motorsRunning ? speedLevels[rightLevel] : 0);
}

void Forward() {
  // Both motors forward.
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  motorsRunning = true;
  ApplySpeed();
}

void Backward() {
  // Both motors backward.
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  motorsRunning = true;
  ApplySpeed();
}

void TurnRight() {
  // Left motor forward; right motor backward.
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  motorsRunning = true;
  ApplySpeed();
}

void TurnLeft() {
  // Left motor backward; right motor forward.
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  motorsRunning = true;
  ApplySpeed();
}

void Stop() {
  // Disable motor power and clear all direction signals.
  motorsRunning = false;
  ApplySpeed();
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}