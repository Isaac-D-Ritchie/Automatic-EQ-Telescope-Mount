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


//Enums
//Display state options
enum screen_state {
  Logo,
  Menu,
  Manual,
  Polaris_sync,
  Success_screen,
  Track,
  Settings
};
screen_state current_display_status = Logo; //Sets starting screen to Logo
//Success screen types
enum success_type {
  Polaris_success,
  Data_import_success
};
success_type current_success_type = Polaris_success;
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
  select_polaris_sync,
  select_track,
  select_settings
};
menu_items selected_menu_item = select_manual;


//Global variables
//Telescope calibration and positioning
bool telescope_calibrated = false;
long polaris_ha_steps = 0; //To store telescope position in motor steps
long polaris_dec_steps = 0;
float polaris_ha = 0; //Current local polaris ha and dec to be sent by python code later
float polaris_dec = 0;
bool calibration_data_received = false; //For checking if local polaris is present for calibration
bool calibration_requested = false;
float current_ha = 0; //For current tracking position
float current_dec = 0;
float target_ha = 0; //Value to be received from serial port
float target_dec = 0;
bool target_data_received = false; //To trigger tracking events
//Motor and gear ratio variables
const uint16_t motor_steps = 200;
const uint8_t microsteps = 8;
const uint16_t ha_ratio = 96; //96:1 worm gear
const uint16_t dec_ratio = 5; //5:1 spur gear
const float steps_per_degree_ha = ((long)motor_steps * microsteps * ha_ratio) / 360.0f;
const float steps_per_degree_dec = ((long)motor_steps * microsteps * dec_ratio) / 360.0f;
//Joystick variables
int joystick_x = 0;
int joystick_y = 0;
bool joystick_btn = HIGH;
bool joystick_stop = false; //To stop menu over scrolling
bool last_joystick_state = HIGH; //To stop button spamming
bool joystick_btn_pressed = false; //Joystick button press detection
unsigned long first_press_time = 0; //Joystick double press detection
const unsigned long double_press_time = 500; //Double detection time frame ms
bool double_press = false; //Double press event triggers
bool second_press_waiting = false;
//Screen refresh limiter
unsigned long last_display_refresh = 0;
const unsigned long display_refresh = 250; //Screen refresh rate (ms)
bool display_needs_updating = true; //For only refreshing screen when needed
unsigned long success_time = 0; //Timer for success screen
const unsigned long success_delay = 3000; //ms


//Non display functions
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
  joystick_btn_pressed = (last_joystick_state == HIGH && joystick_btn == LOW);
  last_joystick_state = joystick_btn;
}
//Function for joystick double press
void update_double_press() {
  double_press = false;
  if (!joystick_btn_pressed) {
    return;
  }
  if (!second_press_waiting) {
    second_press_waiting = true;
    first_press_time = millis();
  }
  else {
    if (millis() - first_press_time <= double_press_time) {
      double_press = true;
    }
    second_press_waiting = false;
  }
  if (second_press_waiting && millis() - first_press_time > double_press_time) {
    second_press_waiting = false;
  }
}
//Joystick Option Selection function 
void option_selection() {
  if (!joystick_stop) {
    if (joystick_y < -300) { //Move up menu
      joystick_stop = true;
      switch (selected_menu_item)
      {
      case select_settings:
        selected_menu_item = select_track;
        display_needs_updating = true;
        break;
    
      case select_track:
        selected_menu_item = select_polaris_sync;
        display_needs_updating = true;
        break;

      case select_polaris_sync:
        selected_menu_item = select_manual;
        display_needs_updating = true;
        break;
    
      case select_manual:
        break;
      }
    }
    else if (joystick_y > 300) { //Move down menu
      joystick_stop = true;
      switch (selected_menu_item)
      {
      case select_manual:
        selected_menu_item = select_polaris_sync;
        display_needs_updating = true;
        break;

      case select_polaris_sync:
        selected_menu_item = select_track;
        display_needs_updating = true;
        break;
    
      case select_track:
        selected_menu_item = select_settings;
        display_needs_updating = true;
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
//Manual Motor control function
void manual_motor_control() {
  float variable_x = constrain(joystick_x / 500.0, -1.0, 1.0); //Convert value to between 1 and -1
  float variable_y = constrain(joystick_y / 500.0, -1.0, 1.0);
  float control_expo = 2.2; //Joystick response curve and calculation
  float control_x = (variable_x >= 0 ? 1 : -1) * pow(abs(variable_x), control_expo);
  float control_y = (variable_y >= 0 ? 1 : -1) * pow(abs(variable_y), control_expo);
  motor1.setSpeed(control_x * 2000); //Motor manual speed control
  motor2.setSpeed(control_y * 100);
}
//Motor power control
void stop_manual_control() {
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  disable_motor(en_pin_1);
  disable_motor(en_pin_2);
}
//Motor coordinate updater
void update_coordinates() {
  long current_ha_steps = motor1.currentPosition();
  long current_dec_steps = motor2.currentPosition();
  long ha_difference = current_ha_steps - polaris_ha_steps;
  long dec_difference = current_dec_steps - polaris_dec_steps;
  current_ha = polaris_ha + ((float)ha_difference / steps_per_degree_ha);
  current_dec = polaris_dec + ((float)dec_difference / steps_per_degree_dec);
}
//Function to calculate target go-to movement in motor steps
void goto_target() {
  if (!telescope_calibrated) { //Wont accept GOTO commands unless telescope is calibrated
    Serial.println("ERROR - Telescope Not Calibrated");
    return;
  }
  enable_motor(en_pin_1); //Enables motors
  enable_motor(en_pin_2);
  long target_ha_steps = polaris_ha_steps + ((target_ha - polaris_ha) * steps_per_degree_ha);
  long target_dec_steps = polaris_dec_steps + ((target_dec - polaris_dec) * steps_per_degree_dec);
  motor1.moveTo(target_ha_steps);
  motor2.moveTo(target_dec_steps);
  current_telescope_status = Targeting_Go_To;
}
//Function to retrieve incoming target data from serial
void receive_target_data() {
  if (!Serial.available()) {
    return;
  }
  String target_data = Serial.readStringUntil('\n');
  target_data.trim();

  int first_comma = target_data.indexOf(','); //Detecting gap between command, ha and dec coordinates
  String command;
  if (first_comma == -1) { //Extracts serial command
    command = target_data;
  }
  else {
    command = target_data.substring(0, first_comma);
  }
  if (command == "START") { //Tells python a connection has been made
    Serial.println("READY");
    return;
  }
  else if (command == "CALIBRATE") { //Sets calibration data for polaris variables
    int second_comma = target_data.indexOf(',', first_comma + 1);
    if (second_comma == -1) {
        Serial.println("ERROR - Calibration Data Formatting");
        return;
    }
    polaris_ha = target_data.substring(first_comma + 1, second_comma).toFloat(); //Saves received polaris data
    polaris_dec = target_data.substring(second_comma + 1).toFloat();
    calibration_data_received = true;
    Serial.println("CONFIRM");
    return;

}
  else if (command == "GOTO") { //Tells motors to move to target
    int second_comma = target_data.indexOf(',', first_comma + 1);
    if (second_comma == -1) { //Checking for data error
      Serial.println("ERROR - GOTO Data Formatting");
      return;
    }
    target_ha = target_data.substring(first_comma + 1, second_comma).toFloat();
    target_dec = target_data.substring(second_comma + 1).toFloat();
    target_data_received = true;
    goto_target();
    Serial.println("CONFIRM");
    current_success_type = Data_import_success;
    success_time = millis();
    current_display_status = Success_screen;
    display_needs_updating = true;
    return;
  }
  else if (command == "STOP") { //Tells motors to stop (Emergency stop)
    motor1.stop();
    motor2.stop();
    current_telescope_status = Idle;
    Serial.println("CONFIRM");
    return;
  }
  else if (command == "STATUS") { //Prints telescope current status to serial
    Serial.println("CONFIRM");
    Serial.print("LOCATION STATUS,");
    Serial.print(current_ha);
    Serial.print(",");
    Serial.println(current_dec);
    Serial.print("SYSTEM STATUS,");
    Serial.print(current_telescope_status);
    Serial.print(",");
    Serial.print(current_display_status);
    Serial.print("CALIBRATED,");
    Serial.println(telescope_calibrated);
    return;
  }
  else { //Issues readying junk bits as unknown error, commented out for development testing
   //Serial.println("ERROR - Unknown Command");
  }
}
//Function to track go-to movements
void update_goto() {
  if (current_telescope_status != Targeting_Go_To) {
      return;
  }
  if (motor1.distanceToGo() == 0 && motor2.distanceToGo() == 0) {
      Serial.println("Arrived."); //Announce target arrival to serial
      current_telescope_status = Tracking_Object;
      current_ha = target_ha;
      current_dec = target_dec;
  }
}
//Display page functions
//Logo Screen
void draw_logo() {
    if (!display_needs_updating) {
      return;
    }

    display_needs_updating = false;
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_ncenB08_tr);
      display.drawStr(10, 30, "ASTEPS");
      display.setFont(u8g2_font_6x10_tr);
      display.drawStr(20, 50, "Press Joystick");
      display.drawStr(28, 60, "to continue");
    }
    while (display.nextPage());
}
//Menu control screen
void draw_menu() {
  if (!display_needs_updating) {
    return;
  }
  display_needs_updating = false;
  display.firstPage();
  do
  {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(50, 8, "Menu");

    if (selected_menu_item == select_manual)
      display.drawStr(0,20,"> Manual <");
    else display.drawStr(0,20,"  Manual");

    if (selected_menu_item == select_polaris_sync)
      display.drawStr(0,34,"> Polaris Sync <");
    else display.drawStr(0,34,"  Polaris Sync");

    if (selected_menu_item == select_track)
      display.drawStr(0,48,"> Tracking <");
    else display.drawStr(0,48,"  Tracking");

    if (selected_menu_item == select_settings) 
      display.drawStr(0,62,"> Settings <");
    else display.drawStr(0,62,"  Settings");

  } while (display.nextPage());
}
//Manual control screen
void draw_manual() {
    if (!display_needs_updating) {
      return;
    }
    display_needs_updating = false;
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_7x13_tr);
      display.drawStr(20, 15, "Manual Control");
      display.setFont(u8g2_font_6x10_tr);
      display.drawStr(20, 30, "Current position");
      char buffer[20];
      char ha_buffer[10];
      char dec_buffer[10];
      dtostrf(current_ha, 6, 2,ha_buffer);
      dtostrf(current_dec, 6, 2, dec_buffer);
      sprintf(buffer, "HA = %s",ha_buffer);
      display.drawStr(20,45,buffer);
      sprintf(buffer, "DEC=%s", dec_buffer);
      display.drawStr(20,60,buffer);
    }
    while (display.nextPage());
}
//Sync telescope coordinates to polaris
void draw_polaris_sync() {
    if (!display_needs_updating) {
      return;
    }
    display_needs_updating = false;
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_7x13_tr);
      display.drawStr(5, 10, "Polaris Alignment");
      display.setFont(u8g2_font_6x10_tr);
      display.drawStr(0, 28, "1. Point to polaris");
      display.drawStr(0, 42, "2. Double press stick");
      display.drawStr(18, 52, "to calibrate");
    }
    while (display.nextPage());
}
//Success screen
void draw_success_screen(const char* title, const char* message, float ha, float dec) {
    if (!display_needs_updating) {
      return;
    }
    display_needs_updating = false;
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_7x13B_tr);
      display.drawStr(0, 15, title);
      display.setFont(u8g2_font_7x13_tr);
      display.drawStr(0, 30, message);
      char buffer[32];
      char ha_buffer[10];
      char dec_buffer[10];
      dtostrf(ha, 6, 2, ha_buffer);
      dtostrf(dec, 6, 2, dec_buffer);
      sprintf(buffer, "HA  %s", ha_buffer);
      display.drawStr(25, 45, buffer);
      sprintf(buffer, "DEC %s", dec_buffer);
      display.drawStr(25, 58, buffer);
    }
    while (display.nextPage());
}
//Track control screen
void draw_track() { 
    if (!display_needs_updating) {
      return;
    }
    display_needs_updating = false;
    display.firstPage();
    do
    {
    display.setFont(u8g2_font_7x13B_tr);
    char buffer[32];
    char ha_buffer[12];
    char dec_buffer[12];
    display.drawStr(10, 10, "Tracking System");
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 22, "Current Position:");
    dtostrf(current_ha, 6, 2,ha_buffer);
    dtostrf(current_dec, 6, 2, dec_buffer);
    sprintf(buffer, "HA:%s",ha_buffer);
    display.drawStr(0, 33, buffer);
    sprintf(buffer, "DEC:%s", dec_buffer);
    display.drawStr(65, 33, buffer);
    if (target_data_received) { //Ask for data for display data if available
      display.drawStr(0, 46, "Target Position:");
      dtostrf(target_ha, 6, 2,ha_buffer);
      dtostrf(target_dec, 6, 2, dec_buffer);
      sprintf(buffer, "HA:%s",ha_buffer);
      display.drawStr(0, 57, buffer);
      sprintf(buffer, "DEC:%s", dec_buffer);
      display.drawStr(65, 57, buffer);
    }
    else {
      display.setFont(u8g2_font_ncenB08_tr);
      display.drawStr(5, 46, "Waiting for tracking");
      display.drawStr(10, 58, "coordinate data...");
    }
    }
    while (display.nextPage());
}
//Settings control screen
void draw_settings() {
    if (!display_needs_updating) {
      return;
    }
    display_needs_updating = false;
    display.firstPage();
    do
    {
      display.setFont(u8g2_font_7x13_tr);
      display.drawStr(20, 15, "Settings");
      display.setFont(u8g2_font_6x10_tr);
      display.drawStr(20, 30, "Calibrate"); // <------------ Menu integration?
    }
    while (display.nextPage());
}


// Arduino setup
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
  Wire.setClock(400000); //Increases screen I2C speed
  display.setDisplayRotation(U8G2_R2); //Flips display orientation
  //Motor setup
  motor1.setMaxSpeed(3000); //Motor 1 max speed
  motor1.setAcceleration(1000);
  motor2.setMaxSpeed(3000); //Motor 2 max speed
  motor2.setAcceleration(1000);
  disable_motor(en_pin_1); //Disables motor power on start
  disable_motor(en_pin_2);
  //Booting sequence (serial port)
  Serial.begin(115200);
  delay(2000);
  Serial.println("ASTEPS BOOTED");
}


//Arduino main loop
void loop() {
  motor1.run(); //Sends motor steps every loop
  motor2.run();
  digitalWrite(red_led, LOW); //Power indicator
  update_joystick(); //Updates joystick position
  update_double_press(); //Checks for joystick double press
  receive_target_data(); //Looks for target data from serial port
  update_goto(); //Updates position to track go-to movement

  //Switch cases for display
  switch (current_display_status)
  {
    case Logo:
      if (joystick_btn_pressed) { //Joystick switch detection
        current_display_status = Menu; 
        display_needs_updating = true;
        return;
      }
      draw_logo();
      break;

    case Menu:
    option_selection();

    if (joystick_btn_pressed) {
      switch (selected_menu_item)
      {
      case select_manual:
        enable_motor(en_pin_1); //Enables motor power
        enable_motor(en_pin_2);
        current_display_status = Manual;
        display_needs_updating = true;
        break;

      case select_polaris_sync:
        current_display_status = Polaris_sync;
        display_needs_updating = true;
        break;
    
      case select_track:
        current_display_status = Track;
        display_needs_updating = true;
        break;

      case select_settings:
        current_display_status = Settings;
        display_needs_updating = true;
        break;
      }
    }
    if (current_display_status == Menu) {
      draw_menu();
    }
    break;

    case Manual:
      manual_motor_control();
      update_coordinates();
      if (joystick_btn_pressed) { //Joystick switch detection
        stop_manual_control();
        current_display_status = Menu;
        display_needs_updating = true;
        return;
      }
      if (joystick_x == 0 && joystick_y == 0) { //Only update display when motor is not moving
        if (millis() - last_display_refresh >= display_refresh) {
          display_needs_updating = true;
          draw_manual();
          last_display_refresh = millis();
        }
      }
      current_telescope_status = Manual_Movement;
      break;

    case Polaris_sync:
        if (!calibration_requested) { //Requests calibration data from serial
          Serial.println("REQUEST_CALIBRATION");
          calibration_requested = true;
    }
      if (double_press) {
          if (!calibration_data_received) {
          Serial.println("ERROR - No Calibration Data");
          return;
        }
        polaris_ha_steps = motor1.currentPosition(); //Saves motor positions in steps
        polaris_dec_steps = motor2.currentPosition();
        current_ha = polaris_ha; //Updates telescope position
        current_dec = polaris_dec;
        telescope_calibrated = true;
        calibration_requested = false; //Resets calibration data checks for next alignment
        calibration_data_received = false;
        current_success_type = Polaris_success;
        success_time = millis(); //Start success screen timer
        current_display_status = Success_screen; //Triggers success screen
        display_needs_updating = true;
        return;
      }
      draw_polaris_sync();
      break;

    case Success_screen:
      switch (current_success_type)
      {
      case Polaris_success:
        draw_success_screen("Polaris Alignment", " --- Success ---", polaris_ha, polaris_dec);
        break;
      
      case Data_import_success:
        draw_success_screen("Data Import", " --- Success ---", target_ha, target_dec);
        break;
      }
      if (millis() - success_time >= success_delay) {
        if (current_success_type == Data_import_success) {
          current_display_status = Track; //Directs back to track after data import
        }
        else {
          current_display_status = Menu; //Directs back to menu after calibration
        }
        display_needs_updating = true;
      }
      break;
      
    

    case Track:
      if (joystick_btn_pressed) {
        current_display_status = Menu; 
        display_needs_updating = true;
        return;
      }
      draw_track();
      break;

    case Settings:
      if (joystick_btn_pressed) {
        current_display_status = Menu; 
        display_needs_updating = true;
        return;
      }
      draw_settings();
      break;
  }
}
