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
#define blue_led 9
//Joy-Stick
#define stick_sw 10
#define stick_x A1
#define stick_y A2


//Functions
void move_motor(int steps, int pulse_delay, int step_pin, int dir_pin, bool dir) {
    digitalWrite(dir_pin, dir); //HIGH = clockwise LOW = anti-clockwise

    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin, HIGH);
        delayMicroseconds(pulse_delay);
        digitalWrite(step_pin, LOW);
        delayMicroseconds(pulse_delay);
      }
}

void enable_motor(int en_pin) {
  digitalWrite(en_pin, LOW); //Powers TMC 2209
  digitalWrite(blue_led, LOW); //Motor LED
}

void disable_motor(int en_pin) {
  digitalWrite(en_pin, HIGH); //Cuts TMC 2209 power
  digitalWrite(blue_led, HIGH); //Cuts motor LED
}


// Setup code (runs once)
void setup() {
  pinMode(stick_sw, INPUT_PULLUP); //Joystick switch
  pinMode(step_pin_1, OUTPUT);
  pinMode(dir_pin_1, OUTPUT);
  pinMode(en_pin_1, OUTPUT);
  pinMode(step_pin_2, OUTPUT);
  pinMode(dir_pin_2, OUTPUT);
  pinMode(en_pin_2, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  digitalWrite(en_pin_1, HIGH); //Motor 1 power (off)
  digitalWrite(en_pin_2, HIGH); //Motor 2 power (off)

  Serial.begin(9600);
}


// Main script
void loop() {
  digitalWrite(red_led, LOW); //Power indicator

  //Joystick controls
  int x = analogRead(stick_x);
  int y = analogRead(stick_y);
  bool button = digitalRead(stick_sw);

  //Prints joystick reading to serial output
  Serial.print("X: ");
  Serial.print(x);
  Serial.print("  Y: ");
  Serial.print(y);
  Serial.print("  Button: ");
  Serial.println(button);

  //Motor 1
  if (x > 600) {
    enable_motor(en_pin_1);
    move_motor(10, 650, step_pin_1, dir_pin_1, HIGH); //Clockwise
  }
  else {
    disable_motor(en_pin_1);
  }

  if (x < 400) {
    enable_motor(en_pin_1);
    move_motor(10, 650, step_pin_1, dir_pin_1, LOW); //Anti-clockwise
  }
  else {
    disable_motor(en_pin_1);
  }

  //Motor 2
  if (y > 600) {
    enable_motor(en_pin_2);
    move_motor(10, 650, step_pin_2, dir_pin_2, HIGH); //Clockwise
  }
  else {
    disable_motor(en_pin_2);
  }
  
  if (y < 400) {
    enable_motor(en_pin_2);
    move_motor(10, 650, step_pin_2, dir_pin_2, LOW); //Anti-clockwise
  }
  else {
    disable_motor(en_pin_2);
  }
}
