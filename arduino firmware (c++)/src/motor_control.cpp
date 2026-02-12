#include <Arduino.h>

//Defining pins

//Motor 1
#define step_pin_1 2
#define dir_pin_1 3
#define en_pin_1 4
//Motor 2
#define step_pin_2 5
#define dir_pin_2 6
#define en_pin_2 7
//LEDs
#define red_led 8
#define green_led 9



//Functions
void move_motor_1_clockwise(int steps, int pulse_delay) {
    digitalWrite(en_pin_1, LOW); //Powers motor
    digitalWrite(dir_pin_1, HIGH); //Sets to clockwise
    digitalWrite(green_led, LOW); //Motor LED

    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin_1, HIGH);
        delayMicroseconds(pulse_delay);
        digitalWrite(step_pin_1, LOW);
        delayMicroseconds(pulse_delay);
      }

      digitalWrite(en_pin_1, HIGH); //Cuts motor power
      digitalWrite(green_led, HIGH); 
}


void move_motor_1_anti_clockwise(int steps, int pulse_delay) {
    digitalWrite(en_pin_1, LOW); //Powers motor
    digitalWrite(dir_pin_1, LOW); //Sets to anti-clockwise
    digitalWrite(green_led, LOW); //Motor LED

    for (int i = 0; i < steps; i++) { 
        digitalWrite(step_pin_1, HIGH);
        delayMicroseconds(pulse_delay);
        digitalWrite(step_pin_1, LOW);
        delayMicroseconds(pulse_delay); 
      }

      digitalWrite(en_pin_1, HIGH); //Cuts motor power
      digitalWrite(green_led, HIGH); 
}


void move_motor_2_clockwise(int steps, int pulse_delay) {
    digitalWrite(en_pin_2, LOW); //Powers motor
    digitalWrite(dir_pin_2, HIGH); //Sets to clockwise
    digitalWrite(green_led, LOW); //Motor LED

    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin_2, HIGH);
        delayMicroseconds(pulse_delay);
        digitalWrite(step_pin_2, LOW);
        delayMicroseconds(pulse_delay);
      }

      digitalWrite(en_pin_2, HIGH); //Cuts motor power
      digitalWrite(green_led, HIGH); 
}


void move_motor_2_anti_clockwise(int steps, int pulse_delay) {
    digitalWrite(en_pin_2, LOW); //Powers motor
    digitalWrite(dir_pin_2, LOW); //Sets to anti-clockwise
    digitalWrite(green_led, LOW); //Motor LED

    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin_2, HIGH);
        delayMicroseconds(pulse_delay);
        digitalWrite(step_pin_2, LOW);
        delayMicroseconds(pulse_delay);
      }

    digitalWrite(en_pin_2, HIGH); //Cuts motor power
    digitalWrite(green_led, HIGH); 
}



// Setup code (runs once)
void setup() {
  digitalWrite(en_pin_1, HIGH); //Motor power (off)
  digitalWrite(en_pin_2, HIGH);
  pinMode(step_pin_1, OUTPUT);
  pinMode(dir_pin_1, OUTPUT);
  pinMode(en_pin_1, OUTPUT);
  pinMode(step_pin_2, OUTPUT);
  pinMode(dir_pin_2, OUTPUT);
  pinMode(en_pin_2, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);

  Serial.begin(9600);
}


// Main script
void loop() {
  digitalWrite(red_led, LOW); //Power indicator

  move_motor_1_clockwise(3200, 800); //Motor 1 Test
  delay(1000);
  move_motor_1_anti_clockwise(3200, 800);
  delay(1000);
  move_motor_2_clockwise(3200, 800); //Motor 2 Test
  delay(1000);
  move_motor_2_anti_clockwise(3200, 800);
  delay(1000);
}
