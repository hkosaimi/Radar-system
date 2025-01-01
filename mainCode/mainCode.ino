#include <Servo.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.hpp>

#define SCREEN_WIDTH 128   
#define SCREEN_HEIGHT 32   
#define OLED_RESET    -1   
#define SCREEN_ADDRESS 0x3C  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int trigPin = 2;
const int echoPin = 5;
const int IR_RECEIVE_PIN = 12;  
const int SERVO_PIN = 9; 
const int BUZZER_PIN = 7;
const int LED_RED_PIN = 3;
const int LED_GREEN_PIN = 4;

Servo myservo;
//int pos;
int speedDelay = 15;             
const int minDelay = 5;         
const int maxDelay = 100;       

const uint32_t arrowUpBtn = 0xE718FF00;  
const uint32_t arrowDownBtn = 0xAD52FF00;  
const uint32_t turnOnBtn = 0xBA45FF00;  
const uint32_t turnOffBtn = 0xE619FF00; 

unsigned long decoded;
bool isDeviceActive = false;  
long duration;
int distance;

unsigned long previousMillis = 0;
const long intervalFar = 250;
const long intervalClose = 50;

bool buzzerState = LOW; 

void setup() {
  Serial.begin(250000);         
  while (!Serial);              
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  myservo.attach(SERVO_PIN);  

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (true);
  }
  
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);
  display.println(F("CPEG 330 project"));
  display.display();

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
}

void loop() {
  processIRRemote();

  if (isDeviceActive) {
    startScan();  

  } else {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(0, 10);
    display.println(F("OFF"));
    display.display();
  }
}

void processIRRemote() {
  if (IrReceiver.decode()) {  
    decoded = IrReceiver.decodedIRData.decodedRawData;
    if (decoded == arrowUpBtn) {
      increaseSpeed();  
    }
    if (decoded == arrowDownBtn) {
      decreaseSpeed();  
    }
    if (decoded == turnOnBtn) {
      isDeviceActive = true;
    }
    if (decoded == turnOffBtn) {
      isDeviceActive = false;
    }
    IrReceiver.resume();  
  }
}

void startScan() {
  scanRightToLeft();
  scanLeftToRight();
}

void scanRightToLeft() {
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    delay(speedDelay);
    distance = calculateDistance();
    handleDistanceEvents(distance);
    displayDistance(distance);   
  }
}

void scanLeftToRight() {
  for (int pos = 180; pos >= 0; pos--) {
    myservo.write(pos);
    delay(speedDelay);
    distance = calculateDistance();
    handleDistanceEvents(distance);
    displayDistance(distance);   
  }
}

int calculateDistance() {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); 
  return duration * 0.034 / 2; 
}

void handleDistanceEvents(int distance) {
  unsigned long currentMillis = millis();
  if (distance <= 40 && distance > 20 && currentMillis - previousMillis >= intervalFar) {
    previousMillis = currentMillis;
    toggleBuzzerAndLED(); 
  } 
  else if (distance <= 20 && distance > 0 && currentMillis - previousMillis >= intervalClose) {
    previousMillis = currentMillis;
    toggleBuzzerAndLED(); 
  } 
  else if (distance > 40) {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, HIGH);
  }
}

void toggleBuzzerAndLED() {
  static bool buzzState = LOW;
  buzzState = !buzzState;
  digitalWrite(BUZZER_PIN, buzzState);
  digitalWrite(LED_RED_PIN, buzzState);
  digitalWrite(LED_GREEN_PIN, HIGH);
}

void increaseSpeed() {
  if (speedDelay > minDelay) {
    speedDelay -= 10;  
  }
}

void decreaseSpeed() {
  if (speedDelay < maxDelay) {
    speedDelay += 10;  
  }
}

void displayDistance(int distance) {
  display.clearDisplay();  
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 10);  
  display.print(distance);
  display.print(F(" cm"));
  display.display();  
}
