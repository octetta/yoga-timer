// yoga timer running on octetta's bench.
// turn knob to adjust time in seconds
// push knob to start timer
// tone when time reaches zero
// stop when time reaches zero

#include <Encoder.h>
#include <SoftwareSerial.h>

#define PIN_TURN_LEFT 2
#define PIN_TURN_RIGHT 3
#define PIN_PUSH 5
#define PIN_BUZZER 9

#define LED_SERIAL_X1 11
#define LED_SERIAL_X2 12
#define LED_BAUD 9600

#define DEBUG_BAUD 9600

#define LED_CLEAR 0x76
#define LED_BRIGHTNESS 0x7a
#define LED_GOTO 0x79

SoftwareSerial led(LED_SERIAL_X1, LED_SERIAL_X2);
Encoder knob(PIN_TURN_LEFT, PIN_TURN_RIGHT);

long runTimer = 10;

void setup() {
  Serial.begin(DEBUG_BAUD);
  Serial.println("Encoder Test:");
  led.begin(LED_BAUD);
  led.write(LED_CLEAR); // clear display
  led.write(LED_BRIGHTNESS); // brightness
  led.write(192);  // 50%
  pinMode(PIN_PUSH, INPUT);
  digitalWrite(PIN_PUSH, HIGH); //turn pullup resistor on
  knob.write(runTimer * 4);
  pinMode(PIN_BUZZER, OUTPUT);
}

long position  = 100;

void show(int x) {
  int n = x;
  int m = 0;
  char d = 0;
  char p = 3;
  if (x < 0) return;
  // show digits
  while (n) {
    m = n;
    n /= 10;
    d = m-n*10;
    led.write(LED_GOTO);
    led.write(p);
    led.write(d);
    p--;
  }
  // show padding 0's
  while (p > 0) {
    led.write(LED_GOTO);
    led.write(p);
    led.write((char)0);    
    p--;
  }
}

#define MODE_SET 0
#define MODE_RUN 1

char lastPush = 0;
char thisPush = 0;
char debouncePush = 0;
char mode = MODE_SET;

#define PUSH_FILTER 5 // how many loops to filter debounce

int pushCount = 0;
long newTimer;
long markMs = 0;
long nowMs;

#define INTERVAL_MS 1000

void loop() {
  nowMs = millis();
  
  // handle knob push
  thisPush = digitalRead(PIN_PUSH);
  if (thisPush == 0) {
    if (lastPush == thisPush) {
      pushCount++;
      delay(5);
    } else {
      pushCount = 0;
    }
  }
  lastPush = thisPush;
  if (pushCount == PUSH_FILTER) {
    if (mode == MODE_SET) {
      Serial.print("SET -> RUN nowMs:");
      Serial.print(nowMs);
      Serial.print(", runTimer:");
      mode = MODE_RUN;
      runTimer = newTimer;
      Serial.print(runTimer);
      Serial.print(", markMs:");
      markMs = nowMs + INTERVAL_MS;
      Serial.println(markMs);
      show(runTimer);
    } else if (mode == MODE_RUN) {
      Serial.println("RUN -> SET");
      mode = MODE_SET;
    }
  }

  if (mode == MODE_SET) {
    // handle knob turns
    newTimer = knob.read() / 4;
    if (newTimer < 0) newTimer = 0;
    if (newTimer != position) {
      position = newTimer;
      show(newTimer);
    }
  } else if (mode == MODE_RUN) {
    if (nowMs >= markMs) {
      runTimer--;
      markMs = nowMs + INTERVAL_MS;
      Serial.println(runTimer);
      if (runTimer < 0) {
        Serial.print("SET -> RUN : ");
        Serial.println(newTimer);
        show(newTimer);
        mode = MODE_SET;
        knob.write(newTimer * 4);
      } else {
        show(runTimer);
        if (runTimer == 0) {
          tone(PIN_BUZZER, 10);
          delay(10);
          noTone(PIN_BUZZER);
        }
      }
    }
  } else {
    Serial.println("?#1");
    mode = MODE_SET;
  }

#if 0
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset knob to zero");
    knob.write(0);
  }
#endif
}
