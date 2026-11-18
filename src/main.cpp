#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int LED_PIN = 4; // d4 led
const int BUTTON_PIN = 27; //d27 button
const int POT_PIN = 34; //potentiometer
int brightness = 0;
int step = 1;// +1 fading in, -1 fading out
unsigned long lastUpdate=0; 

unsigned long lastButtonPrint = 0; //button state

bool isBreathing = true;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastPotPrint = 0;

int raw_potentiometer = 0;
int converted_brightness = 0;

void updateDisplay(bool active){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 16);
  display.println(active ? "AUTO" : "MANUAL");
  display.setCursor(40, 45);
  display.println((converted_brightness*100)/255);
  display.println("%");
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("SSD1306 allocation fail, Check SDA(21)/SCL(22)");
    for(;;);
  }
  updateDisplay(isBreathing);
}

void loop() {
  unsigned long now = millis();
  unsigned long interval = (step>0) ? 5 : 10; //5ms interval bright, 10ms dim
  if(now - lastPotPrint >= 200){
    lastPotPrint = now;
    Serial.println(analogRead(POT_PIN));
    updateDisplay(isBreathing);
  }
  if(digitalRead(BUTTON_PIN) == LOW && (now - lastButtonPrint >= 200)){
    lastButtonPrint = now;
    Serial.println("Button PRESSED");
  }
  int reading = digitalRead(BUTTON_PIN);
  if(reading == LOW && lastButtonState == HIGH && (now - lastDebounceTime >= 50)){
    lastDebounceTime = now;
    isBreathing = !isBreathing;
    Serial.println(isBreathing ? "Auto" : "Manual");
    updateDisplay(isBreathing);
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
  if(!isBreathing && (now - lastUpdate >= interval)){
    raw_potentiometer = analogRead(POT_PIN);
    converted_brightness = raw_potentiometer / 16;
    lastUpdate = now;
    analogWrite(LED_PIN, converted_brightness);
  }
}