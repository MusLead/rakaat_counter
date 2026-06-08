#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

int IRSensor = 9;
int LED = 13;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;

int sujudCount = 0;
int rakaatCount = 0;

int currentStatus = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(IRSensor, INPUT);
  pinMode(LED, OUTPUT);
  u8g2.begin();
  Serial.println("Rakaat Counter Ready");
}

void loop() {
  unsigned long startTime = millis();
  int lowCount = 0, highCount = 0;

  while ((millis() - startTime) <= debounceDelay) {
    int reading = digitalRead(IRSensor);
    if (reading == LOW) {
      lowCount++;
    } else {
      highCount++;
    }
    delay(100);
  }

  int sensorStatus = (lowCount > highCount) ? LOW : HIGH;

  if (sensorStatus == LOW && currentStatus != sensorStatus) {
    sujudCount++;
    Serial.print("Sujud Count: ");
    Serial.println(sujudCount);

    if (sujudCount == 2) {
      
      // Blink before updating the rakaat count
      blinkRakaatCount();

      sujudCount = 0;
      rakaatCount++;
      Serial.print("Rakaat Count: ");
      Serial.println(rakaatCount);
      digitalWrite(LED, HIGH);
      delay(250);
      digitalWrite(LED, LOW);
    }

    lastDebounceTime = millis();
  }
  
  currentStatus = sensorStatus;

  // Display the current rakaat count and sujud circles without blinking
  displayRakaat(false);
}

void blinkRakaatCount() {
  for (int i = 0; i < 4; i++) {
    u8g2.clearBuffer();
    displayRakaat(false); // Temporarily display without blinking effect
    u8g2.sendBuffer();
    delay(250);

    u8g2.clearBuffer();
    u8g2.sendBuffer();
    delay(250);
  }
}

void displayRakaat(bool blink) {
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
