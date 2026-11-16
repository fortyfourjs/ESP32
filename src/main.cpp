#include <Arduino.h>

const int LED_PIN = 4; // d4 pin
const int BUTTON_PIN = 27; //d27 pin
int brightness = 0;
int step = 1;// +1 fading in, -1 fading out
unsigned long lastUpdate=0; 

unsigned long lastButtonPrint = 0; //button state

bool isBreathing = true;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();
  unsigned long interval = (step>0) ? 5 : 10; //5ms interval bright, 10ms dim

  if(digitalRead(BUTTON_PIN) == LOW && (now - lastButtonPrint >= 200)){
    lastButtonPrint = now;
    Serial.println("Button PRESSED");
  }
  int reading = digitalRead(BUTTON_PIN);
  if(reading == LOW && lastButtonState == HIGH && (now - lastDebounceTime >= 50)){
    lastDebounceTime = now;
    isBreathing = !isBreathing;
    Serial.println(isBreathing ? "Resumed" : "Paused");
  }
  lastButtonState = reading;
  
  if(isBreathing && (now - lastUpdate >= interval)){
    lastUpdate = now;
    brightness += step;
    analogWrite(LED_PIN, brightness);

    if(brightness >= 255){
      step = -1;
    }else if(brightness <= 0){
      step = 1;
    }
  }
}