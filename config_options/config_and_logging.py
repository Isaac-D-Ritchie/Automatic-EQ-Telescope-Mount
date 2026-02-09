"""
Configuration settings for ASTEPS including:
    #logging setup
    #motor specs
    #Mount protection
    #Tracking constants
"""

"""Logging config"""
import logging
enable_logging = True #Logging toggle

def config_logging() -> bool:
    """
    Function to configure logging settings if logging is enabled.
    Return:
        true or false bool (logging enabled status)
    """
    if enable_logging:
        logging.basicConfig(
                filename="asteps.log", level= logging.DEBUG,
                format= "%(asctime)s - %(levelname)s - %(message)s",
                datefmt= "%Y-%m-%d %H:%M:%S", filemode= "a")
    return enable_logging
    

"""Electronic motor config (custom to motor/telescope)"""
#Can be edited
steps_per_rev = 0
microsteps_per_step = 0
ra_gear_ratio = 0
dec_gear_ratio = 0

#Calculates other values based on motor/telescope config values
total_steps_per_full_ra = (steps_per_rev * microsteps_per_step * ra_gear_ratio)
microsteps_per_deg = total_steps_per_full_ra / 360
microsteps_per_rev = (steps_per_rev * microsteps_per_step)


"""Mount protection (Fixed constants)"""
#Prevents overturning
max_dec = 180
min_dec = -180
max_hour_angle = -90
min_hour_angle = 90

"""Tracking constants"""