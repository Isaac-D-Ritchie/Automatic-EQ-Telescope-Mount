//Librarys
#include <Arduino.h>
#include <AccelStepper.h> //Motor step pulse control
#include <U8g2lib.h> //Display library
#include <Wire.h> //I2C Comunication library (SDA + SCL)


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


//Hardware Objects
//128 x 64 OLED Screen (Defined as 'display') - Using refresh page system to save SRAM 
U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
//Assigning AccelStepper pins for motor drivers
AccelStepper motor1(AccelStepper::DRIVER, step_pin_1, dir_pin_1);
AccelStepper motor2(AccelStepper::DRIVER, step_pin_2, dir_pin_2);


//Enums and variables
//Display state options
enum screen_state {
  Logo,
  Menu,
  Manual,
  Track,
  Settings
};
screen_state current_display_status = Logo; //Sets starting screen to Logo
//Telescope status for device monitoring
enum telescope_status {
  Idle,
  Tracking_Object,
  Targeting_Go_To,
  Manual_Movement,
  Error
};
telescope_status current_telescope_status =  Idle;
//Menu Options
enum menu_items {
  select_manual,
  select_track,
  select_settings
};
menu_items selected_menu_item = select_manual;
//Joystick variables - global
int joystick_x = 0;
int joystick_y = 0;
bool joystick_btn = HIGH;
bool joystick_stop = false; //Stops endless scrolling in menu


//Functions
//Motor power control functions
void enable_motor(int en_pin) {
  digitalWrite(en_pin, LOW); //Powers TMC 2209
  digitalWrite(blue_led, LOW); //Motor LED
}
void disable_motor(int en_pin) {
  digitalWrite(en_pin, HIGH); //Cuts TMC 2209 power
  digitalWrite(blue_led, HIGH); //Cuts motor LED
}
//Function to update joystick in loop
void update_joystick() {
  joystick_btn = digitalRead(stick_sw);
  joystick_x = analogRead(stick_x);
  joystick_x -= 524; //Resting value adjustment x
  if (abs(joystick_x) <25) {
    joystick_x = 0;
  }
  joystick_y = analogRead(stick_y);
  joystick_y -= 509; //Resting value adjustment y
  if (abs(joystick_y) <25) {
    joystick_y = 0;
  }
}
//Joystick Option Selection function 
void option_selection() {
  if (!joystick_stop) {
    if (joystick_y < -300) { //Move up
      joystick_stop = true;
      switch (selected_menu_item)
      {
      case select_settings:
        selected_menu_item = select_track;
        break;
    
      case select_track:
        selected_menu_item = select_manual;
        break;
    
      case select_manual:
        break;
      }
    }
    else if (joystick_y > 300) { //Move down
      joystick_stop = true;
      switch (selected_menu_item)
      {
      case select_manual:
        selected_menu_item = select_track;
        break;
    
      case select_track:
        selected_menu_item = select_settings;
        break;

      case select_settings:
        break;
      }
    }
  }
  if (abs(joystick_y) < 100) {
    joystick_stop = false;
  }
}
//Display Screen function
void draw_logo() { //Logo Screen
  {
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_ncenB18_tr);
      display.drawStr(10, 30, "ASTEPS");
      display.setFont(u8g2_font_6x10_tr);
      display.drawStr(20, 50, "Press Joystick");
      display.drawStr(28, 60, "to continue");
    }
    while (display.nextPage());
    {
    if (digitalRead(stick_sw) == LOW) { //Joystick switch detection
    current_display_status = Menu; }
    }
  }
}
void draw_menu() { //Menu control screen

  display.firstPage();
  do
  {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(50, 10, "Menu");

    if (selected_menu_item == select_manual)
      display.drawStr(0,25,"> Manual <");
    else display.drawStr(0,25,"  Manual");

    if (selected_menu_item == select_track)
      display.drawStr(0,40,"> Tracking <");
    else display.drawStr(0,40,"  Tracking");

    if (selected_menu_item == select_settings) 
      display.drawStr(0,55,"> Settings <");
    else display.drawStr(0,55,"  Settings");

  } while (display.nextPage());
}

void draw_manual() { //Manual control screen
 //Plans to make joystick tracking and current position <-----------------------------
}
void draw_track() { //Track control screen
 //Plans to make integration with python for object search / Go-To  <-----------------
}
void draw_settings() { //Settings control screen
 //Plans to make settings options with info about device <----------------------------
}


// Arduino setup code
void setup() {
  // Pin setup
  pinMode(stick_sw, INPUT_PULLUP);
  pinMode(step_pin_1, OUTPUT);
  pinMode(dir_pin_1, OUTPUT);
  pinMode(en_pin_1, OUTPUT);
  pinMode(step_pin_2, OUTPUT);
  pinMode(dir_pin_2, OUTPUT);
  pinMode(en_pin_2, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  // Display setup
  display.begin();
  display.setDisplayRotation(U8G2_R2); //Flips display orientation
  //Motor setup
  motor1.setMaxSpeed(4500); //Motor 1 max speed
  motor1.setAcceleration(1000);
  enable_motor(en_pin_1);
  motor2.setMaxSpeed(4500); //Motor 2 max speed
  motor2.setAcceleration(1000);
  enable_motor(en_pin_2);

  Serial.begin(115200);
}


// Main script
void loop() {
  digitalWrite(red_led, LOW); //Power indicator
  update_joystick(); //Updates joystick position

  //Prints joystick reading to serial output
  Serial.print("X: ");
  Serial.print(joystick_x);
  Serial.print("  Y: ");
  Serial.print(joystick_y);
  Serial.print("  Button: ");
  Serial.println(joystick_btn);


  //Switch cases for display
  switch (current_display_status)
  {
    case Logo:
      draw_logo();
      break;

    case Menu:
      option_selection();
      draw_menu();
      break;

    case Manual:
      draw_manual();
      break;

    case Track:
      draw_track();
      break;

    case Settings:
      draw_settings();
      break;

    default:
      break;
  }

  //Variable motor control from joystick
  float variable_x = constrain(joystick_x / 500.0, -1.0, 1.0); //Convert value to between 1 and -1
  float variable_y = constrain(joystick_y / 500.0, -1.0, 1.0);
  float control_expo = 2.2; //Joystick response curve and calculation
  float control_x = (variable_x >= 0 ? 1 : -1) * pow(abs(variable_x), control_expo);
  float control_y = (variable_y >= 0 ? 1 : -1) * pow(abs(variable_y), control_expo);
  motor1.setSpeed(control_x * 3000); //Motor manual speed control
  motor2.setSpeed(control_y * 3000);
  motor1.runSpeed();
  motor2.runSpeed();
}
