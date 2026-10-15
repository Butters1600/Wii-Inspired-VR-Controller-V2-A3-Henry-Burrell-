/*
  Arduino Nano 33 BLE Sense Rev2
  Wii-Inspired Motion Controller for Hollow Knight

  CONTROLS:
  Tilt Left  -> LEFT
  Tilt Right -> RIGHT
  Tilt Up    -> UP / Jump
  Tilt Down  -> DOWN
  D2 Button  -> ATTACK

  Shake attack has been removed.
*/

#include "Arduino_BMI270_BMM150.h"

// TILT SETTINGS
const int TRIGGER_DEGREES = 20;
const int RELEASE_DEGREES = 10;

// ATTACK BUTTON
const int ATTACK_BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 25;

// MOVEMENT STATES
bool isLeftPressed = false;
bool isRightPressed = false;
bool isUpPressed = false;
bool isDownPressed = false;

// BUTTON STATES
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(ATTACK_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);

  if (!IMU.begin()) {
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {

  // =========================================
  // PHYSICAL ATTACK BUTTON
  // =========================================

  bool reading = digitalRead(ATTACK_BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_MS) {

    if (reading != stableButtonState) {

      stableButtonState = reading;

      if (stableButtonState == LOW) {
        Serial.println("PRESS ATTACK");
      }
      else {
        Serial.println("RELEASE ATTACK");
      }
    }
  }

  lastButtonReading = reading;


  // =========================================
  // MOTION CONTROLS
  // =========================================

  float x, y, z;

  if (IMU.accelerationAvailable()) {

    IMU.readAcceleration(x, y, z);

    int degreesPitch = 0;
    int degreesRoll = 0;

    // =======================================
    // PITCH - UP / DOWN
    // =======================================

    if (x > 0.1f) {
      degreesPitch = map((int)(x * 100), 0, 97, 0, 90);
    }
    else if (x < -0.1f) {
      degreesPitch = -map((int)(x * 100), 0, -100, 0, 90);
    }

    // UP / JUMP

    if (!isUpPressed &&
        degreesPitch > TRIGGER_DEGREES) {

      Serial.println("PRESS UP");
      isUpPressed = true;
    }
    else if (isUpPressed &&
             degreesPitch < RELEASE_DEGREES) {

      Serial.println("RELEASE UP");
      isUpPressed = false;
    }

    // DOWN

    if (!isDownPressed &&
        degreesPitch < -TRIGGER_DEGREES) {

      Serial.println("PRESS DOWN");
      isDownPressed = true;
    }
    else if (isDownPressed &&
             degreesPitch > -RELEASE_DEGREES) {

      Serial.println("RELEASE DOWN");
      isDownPressed = false;
    }


    // =======================================
    // ROLL - LEFT / RIGHT
    // =======================================

    if (y > 0.1f) {
      degreesRoll = map((int)(y * 100), 0, 97, 0, 90);
    }
    else if (y < -0.1f) {
      degreesRoll = -map((int)(y * 100), 0, -100, 0, 90);
    }

    // LEFT

    if (!isLeftPressed &&
        degreesRoll < -TRIGGER_DEGREES) {

      Serial.println("PRESS LEFT");
      isLeftPressed = true;
    }
    else if (isLeftPressed &&
             degreesRoll > -RELEASE_DEGREES) {

      Serial.println("RELEASE LEFT");
      isLeftPressed = false;
    }

    // RIGHT

    if (!isRightPressed &&
        degreesRoll > TRIGGER_DEGREES) {

      Serial.println("PRESS RIGHT");
      isRightPressed = true;
    }
    else if (isRightPressed &&
             degreesRoll < RELEASE_DEGREES) {

      Serial.println("RELEASE RIGHT");
      isRightPressed = false;
    }
  }

  delay(10);
}