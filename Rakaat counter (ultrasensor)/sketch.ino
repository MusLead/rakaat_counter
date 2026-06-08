#include <U8g2lib.h>
#include <Wire.h>

// Initialize Ultrasonic Sensor pins
const int trigPin = 11; // Assign a pin for TRIG
const int echoPin = 12; // Assign a pin for ECHO

// SH1106 OLED display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long debounceDelay = 1000; // Measurement interval
int rakaatCount = 0; // Rakaat counter
int sujudCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  u8g2.begin();
  Serial.println("Setup Complete");
}

boolean currentStatus = false;
void loop() {
  boolean isSujud = detectSujud();
  if (isSujud && isSujud != currentStatus) {
    sujudCount++;
    Serial.print("Sujud Count: ");
    Serial.println(sujudCount);
    
    if (sujudCount == 2) {
      
      // Blink before updating the rakaat count
      blinkRakaat();
      
      sujudCount = 0;
      rakaatCount++;
      Serial.print("Rakaat Count: ");
      Serial.println(rakaatCount);
    }
  } 
  if(isSujud != currentStatus){
    currentStatus = isSujud;
  }

  // Display rakaat 
  displayRakaat();
}

bool detectSujud() {
  unsigned long startTime = millis();
  const int sampleSize = 5; // Number of measurements to take
  int sujudThreshold = 20; // Threshold distance in cm for sujud detection
  int closeCount = 0, farCount = 0;

  while ((millis() - startTime) <= debounceDelay) {
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and return)
    //Serial.println(distance);
    if (distance <= sujudThreshold) closeCount++;
    else farCount++;

    delay(50); // Short delay between measurements
  }

  return closeCount > farCount; // True if sujud, false otherwise
}

void blinkRakaat() {
  for (int i = 0; i < 4; i++) {
    u8g2.clearBuffer();
    displayRakaat(); // Temporarily display without blinking effect
    u8g2.sendBuffer();
    delay(250);

    u8g2.clearBuffer();
    u8g2.sendBuffer();
    delay(250);
  }
}

void displayRakaat() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB08_tr);
  char rakaatStr[] = "Rakaat: ";
  u8g2.drawStr(2, 24, rakaatStr);

  int staticTextWidth = u8g2.getStrWidth(rakaatStr);

  if (sujudCount > 0) {
    for (int i = 0; i < sujudCount; i++) {
      u8g2.drawDisc(10 + i * 12, 46, 4);
    }
  }

  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(staticTextWidth + 10, 48);
  u8g2.print(rakaatCount);
  u8g2.sendBuffer();
}
