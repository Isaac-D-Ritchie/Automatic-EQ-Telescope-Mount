#include <Arduino.h>

//Defining pins

//Motor engage pin
#define en_pin 4

//Motor 1
#define step_pin_1 2
#define dir_pin_1 3

//Motor 2
#define step_pin_2 5
#define dir_pin_2 6

//LEDs
#define red_led 7
#define green_led 9


//Functions
void move_motor_1_clockwise(int steps, int delay)
{
    for (int i = 0; i < steps; i++)
    {
        digitalWrite(en_pin, LOW);
        digitalWrite(red_led, LOW);
        digitalWrite(dir_pin_1, HIGH)
        digitalWrite(step_pin_1, HIGH);
        delayMicroseconds(delay);
        digitalWrite(step_pin_1, LOW);
        delayMicroseconds(delay);
        digitalWrite(red_led, HIGH);
    }
}

// Setup code (runs once)
void setup() {
  digitalWrite(en_pin, HIGH); //HIGH = no power
  pinMode(step_pin_1, OUTPUT);
  pinMode(dir_pin_1, OUTPUT);
  pinMode(en_pin, OUTPUT);
  pinMode(step_pin_2, OUTPUT);
  pinMode(dir_pin_2, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
}



// Main script
void loop() {
  move_motor_1_clockwise(3200, 800);
  delay(1000);
}
