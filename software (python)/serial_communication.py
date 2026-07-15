"""
This file contains all the code needed to communicate to the 
micrcontroller (arduino) controlling the telescope.
"""

""" Imports """
import serial #For serial port communication
import time

class TelescopeController:
    def __innit__(self, port: str, baudrate: int = 115200):
        self.serial = serial.Serial(port= port, baudrate= baudrate, timeout= 1)
        time.sleep(2) #Wait for micrcontroller to respond after connection
    
    def goto(self, hour_angle: float, declination: float): #For sending target data
        command = f"GOTO,{hour_angle:.6f},{declination:.6f}\n"
        self.serial.write(command.encode())

    def stop(self): #For sending emergency stop
        self.serial.write(b"STOP\n")

    def status(self): #To request telescope status
        self.serial.write(b"STATUS\n")

    def read(self): #For receiving serial response from micrcontroller
        if self.serial.in_waiting:
            return self.serial.readline().decode().strip()

        return None