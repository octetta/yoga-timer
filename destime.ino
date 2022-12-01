#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial led(11,12);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  led.begin(9600);
  led.write(0x76); // clear display
  led.write(0x7a); // brightness
  led.write(127);  // 50%
}

unsigned int i = 0;
int n = 0;
int m = 0;
char d = 0;
char p = 0;


void loop() {
  n = i;
  p = 3;
  if (n > 999) n = 999;
  //led.write(0x76);
  while (n) {
    m = n;
    n /= 10;
    d = m-n*10;
    led.write(0x79);
    led.write(p);
    led.write(d);
    p--;
  }
  while (p > 0) {
    led.write(0x79);
    led.write(p);
    led.write((char)0);    
    p--;
  }
  delay(1000);
  i++;
  if (i > 999) i = 0;

}
