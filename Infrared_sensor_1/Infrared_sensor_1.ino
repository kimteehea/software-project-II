#include <Servo.h>

#define PIN_IR A0
#define PIN_SERVO 8
#define PIN_LED 9

#define INTERVAL 1000
#define ALPHA 0.05

#define DUTY_MIN 1000
#define DUTY_MAX 2000

Servo myservo;

unsigned long last_time = 0;

float dist_raw = 0;
float dist_ema = 0;

int a_value = 0;
int angle = 0;
int duty_user = 1500;

bool ema_initialized = false;

void setup() {
  Serial.begin(1000000);

  myservo.attach(PIN_SERVO);

  pinMode(PIN_LED, OUTPUT);

  myservo.writeMicroseconds(1500);
}

void loop() {

  unsigned long current_time = millis();

  if (current_time - last_time >= INTERVAL) {

    last_time = current_time;

    a_value = analogRead(PIN_IR);

    if (a_value > 9) {

      dist_raw = (6762.0 / (a_value - 9) - 4.0) * 10.0 - 60.0;

      if (dist_raw >= 50 && dist_raw <= 800) {

        if (!ema_initialized) {
          dist_ema = dist_raw;
          ema_initialized = true;
        }
        else {
          dist_ema = ALPHA * dist_raw
                   + (1.0 - ALPHA) * dist_ema;
        }
      }
    }

    if (ema_initialized) {

      if (dist_ema <= 100) {
        angle = 0;
      }
      else if (dist_ema >= 250) {
        angle = 180;
      }
      else {
        angle = (int)((dist_ema - 100.0) * 180.0 / 150.0);
      }

      duty_user = DUTY_MIN
                + (int)(angle * (DUTY_MAX - DUTY_MIN) / 180.0);

      myservo.writeMicroseconds(duty_user);

      if (dist_ema >= 100 && dist_ema <= 250) {
        digitalWrite(PIN_LED, HIGH);
      }
      else {
        digitalWrite(PIN_LED, LOW);
      }
    }
    Serial.print("IR:");
    Serial.print(a_value);

    Serial.print("\tDIST:");
    Serial.print(dist_raw);

    Serial.print("\tEMA:");
    Serial.print(dist_ema);

    Serial.print("\tDUTY:");
    Serial.println(duty_user);
  }
}
