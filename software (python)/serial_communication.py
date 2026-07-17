"""
This file contains all the code needed to communicate to the 
micrcontroller (arduino) controlling the telescope.
"""

""" Imports """
import serial #For serial port communication
import time

class TelescopeController:
    def __init__(self, port: str, baudrate: int = 115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = None


    def start(self): #For making connection handshake to telescope
        self.serial.reset_input_buffer()
        self.serial.write(b"START\n")
        self.serial.flush() #Force send all serial data

        delay = time.time() + 3 #Delay for connection
        while time.time() < delay:
            if self.serial.in_waiting:
                response = self.serial.readline().decode(errors="ignore").strip() #Decodes serial data
                print(repr(response)) #Prints incoming serial data
                if response == "READY":
                    return True
        return False
    

    def connect(self): #Prompts user to try again if connection was not made - returns true if connection made
        while True:
            print("\nConnecting to telescope...")
            try:
                self.serial = serial.Serial(port=self.port,baudrate=self.baudrate,timeout=1)
                time.sleep(3) #Wait for telescope to boot/load

            except serial.SerialException as e: #Exception if serial port cannot be opened
                print(F"Unable to open serial port - ERROR({e})\n")
                retry = input("Try again? (Y/N): ").strip().lower()
                if retry == "n":
                    return False
                else: #Anything but 'n' will try connect again
                    continue

            if self.start():
                print("Connection successful\n")
                return True

            print("Connection failed\n")
            self.serial.close()
            retry = input("Try again? (Y/N): ").strip().lower()
            if retry != "y":
                return False


    def goto(self, hour_angle: float, declination: float): #For sending target data
        command = f"GOTO,{hour_angle:.6f},{declination:.6f}\n"
        self.serial.write(command.encode())


    def calibrate(self, hour_angle: float, declination: float):
        command = f"CALIBRATE,{hour_angle:.6f},{declination:.6f}\n"
        self.serial.write(command.encode())


    def wait_for_calibration(self): #Waits for user to request calibration telescope
        print("\nWaiting for telescope calibration request...")
        while True:
            if self.serial.in_waiting:
                response = self.serial.readline().decode(errors="ignore").strip()
                print(f"Arduino: {response}")
                if response == "REQUEST_CALIBRATION":
                    return True


    def wait_for_calibration_complete(self): #Waits for calibration to be complete before proceeding
        print("Waiting for Polaris alignment...")
        while True:
            if self.serial.in_waiting:
                response = self.serial.readline().decode(errors="ignore").strip()
                print(f"Arduino: {response}")
                if response == "CALIBRATION_COMPLETE":
                    return True

    def stop(self): #For sending emergency stop
        self.serial.write(b"STOP\n")


    def status(self): #To request telescope status
        self.serial.write(b"STATUS\n")


    def read(self): #For receiving serial response from micrcontroller
        if self.serial.in_waiting:
            return self.serial.readline().decode().strip()
        return None