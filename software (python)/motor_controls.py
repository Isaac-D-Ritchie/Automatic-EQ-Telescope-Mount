"""
This file contains all the functions to convert the astrological data
into instructions for the motor.
"""



"""Mount protection (Fixed constants)"""
#Prevents overturning
max_dec = 180
min_dec = -180
max_hour_angle = -90
min_hour_angle = 90