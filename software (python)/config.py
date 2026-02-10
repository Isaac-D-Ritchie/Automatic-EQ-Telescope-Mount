"""
This file contains all the config settings and constants used for the
software and hardware

Configuration settings for ASTEPS including:
    #logging setup
    #motor specs
    #Tracking constants
"""
"""Imports"""
import logging
import os


"""Logging config"""
enable_logging = True #Logging toggle

if enable_logging:
    current_folder = os.path.dirname(__file__)
    project_folder = os.path.abspath(os.path.join(current_folder, ".."))
    LOG_FILE = os.path.join(os.path.dirname(current_folder), "asteps.log")

    # Configure the logger
    logging.basicConfig(
        filename=LOG_FILE,
        level=logging.DEBUG,
        format="%(asctime)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S", filemode="a")
    
    logger = logging.getLogger("asteps")


"""Hardware config"""
#Can be edited
steps_per_rev = 0
microsteps_per_step = 0
ra_gear_ratio = 0
dec_gear_ratio = 0

#Calculates other values based on motor/telescope config values
total_steps_per_full_ra = (steps_per_rev * microsteps_per_step * ra_gear_ratio)
microsteps_per_deg = total_steps_per_full_ra / 360
microsteps_per_rev = (steps_per_rev * microsteps_per_step)


"""Tracking constants"""