"""
Configuration settings for ASTEPS including:
logging setup
motor specs
Mount protection
Tracking
"""
import logging


"""Logging config"""
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
    

"""Electronic motor config (custom to part)"""
steps_per_rev = 0 #TBD
microsteping = 0
ra_gear_ratio = 0
dec_gear_ratio = 0
total_steps_ra = (steps_per_rev * microsteping * ra_gear_ratio)
steps_per_deg = total_steps_ra / 360 #calculates motor steps per 1 ra deg


"""Mount protection (Fixed constants)"""
max_dec = 180
min_dec = -180
max_hour_angle = -90
min_hour_angle = 90
