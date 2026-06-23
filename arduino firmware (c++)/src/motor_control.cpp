#include <Arduino.h>
#include <AccelStepper.h>

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

AccelStepper motor1(AccelStepper::DRIVER, step_pin_1, dir_pin_1);
AccelStepper motor2(AccelStepper::DRIVER, step_pin_2, dir_pin_2);

//Functions
//Custom motor control (NON AccelStepper)
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

  motor1.setMaxSpeed(4500);
  motor1.setAcceleration(1000);
  enable_motor(en_pin_1);

  motor2.setMaxSpeed(4500);
  motor2.setAcceleration(1000);
  enable_motor(en_pin_2);

  Serial.begin(115200);
}


// Main script
void loop() {
  digitalWrite(red_led, LOW); //Power indicator

  //Joystick controls
  bool button = digitalRead(stick_sw);
  
  int x = analogRead(stick_x);
  x -= 524; //Resting value x
  if (abs(x) <50) {
    x = 0;
  }

  int y = analogRead(stick_y);
  y -= 509; //Resting value y
  if (abs(y) <50) {
    y = 0;
  }

  //Prints joystick reading to serial output
  Serial.print("X: ");
  Serial.print(x);
  Serial.print("  Y: ");
  Serial.print(y);
  Serial.print("  Button: ");
  Serial.println(button);

  //Variable motor control
  float variable_x = constrain(x / 500.0, -1.0, 1.0); //Convert value to between 1 and -1
  float variable_y = constrain(y / 500.0, -1.0, 1.0);
  float control_exponential = 2.2;
  float control_x = (variable_x >= 0 ? 1 : -1) * pow(abs(variable_x), control_exponential);
  float control_y = (variable_y >= 0 ? 1 : -1) * pow(abs(variable_y), control_exponential);

  motor1.setSpeed(control_x * 3000);
  motor2.setSpeed(control_y * 3000);

  motor1.run();
  motor2.run();
}
