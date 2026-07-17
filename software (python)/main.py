"""
Astronomical Equatorial Positioning System (ASTEPS)

This is a person portfolio project where i will me combining my electrical
engineering knowledge with Python programming to create an automated mount
positioning system which can be retrofitted to manual equatorial telescope
mounts.

This will be a designed as a personal astrophotography and observation tool
for my 135 diameter, 800mm Telescope with a manual equatorial mount.

This main file imports functions from other files in /software (python)
    # astropy_positions
    # config_
    # main
    # reuseable_functions
    # serial_communication
"""

"""Imports"""
import datetime
import config
from config import logger
import reuseable_functions
import astropy_positions
import serial_communication


"""Main code"""
try:
    if __name__ == "__main__":
        print("\n--- Starting Program ---")
        telescope = serial_communication.TelescopeController("/dev/cu.usbmodem11301") #Telescope serial port
        if not telescope.connect(): #Closes program if user chooses not to connect after fail
            print("Closing Program")
            exit()

        current_time = datetime.datetime.now()
        logger.debug(f"Generated time = {current_time}")

        observer_location: EarthLocation = (astropy_positions.get_location_info())

        polaris = astropy_positions.get_polaris_position() #Gets Polaris coordinates
        current_time = astropy_positions.get_time()
        polaris_ha, polaris_dec, polaris_ra = astropy_positions.get_mount_angles(
            polaris, observer_location, current_time) # Convert Polaris coordinates to mount angles
        
        # Wait for Arduino request
        if telescope.wait_for_calibration():
            print("Sending Polaris calibration data...")
            telescope.calibrate(polaris_ha, polaris_dec)
            
            if telescope.wait_for_calibration_complete():
                print("Calibration successful\n")

        while True:
            current_time = astropy_positions.get_time()
            target_location: SkyCoord = astropy_positions.get_target_location(
                observer_location, current_time)
            #Gets format to parse RA and DEC angles
            icrs_value_target = target_location.transform_to("icrs")
            logger.debug(f"Converted target data to icrs = {icrs_value_target}")

            hour_angle, right_ascension, declination = astropy_positions.get_mount_angles(
                icrs_value_target, observer_location, current_time)

            print(f"Hour angle = {hour_angle:.2f}\n"
                  f"Declination = {declination:.2f}\n"
                  f"Right Ascension = {right_ascension:.2f}\n"
                  f"Altitude = {target_location.alt.deg:.2f}\n")
            telescope.goto(hour_angle, declination)
            time.sleep(5)

except KeyboardInterrupt:
    telescope.shutdown()
    logger.info("Keyboard Interrupt - Closing Program")


"""Saved Locations"""
#Southport UK
l1_lat = 53.6311
l1_lon = -2.9925
l1_height = 6
l1_location = EarthLocation(lat=(test_lat * u.deg),lon=(test_lon * u.deg),
                              height=(test_height * u.m))