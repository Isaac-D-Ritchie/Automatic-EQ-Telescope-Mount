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
    # config_and_logging
    # motor_controls
    # reuseable_functions)
"""

"""Imports"""
import config
from config import logger
import reuseable_functions
import astropy_positions


"""Main code"""
if __name__ == "__main__":
    print("Starting Program")

    current_time = astropy_positions.get_time()
    logger.debug(f"Generated time = {current_time}")

    observer_location: EarthLocation = (astropy_positions.get_location_info())

    while True:
        target_location: SkyCoord = astropy_positions.get_target_location(observer_location,
                                                        current_time)
        #Gets format to parse RA and DEC angles
        icrs_value_target = target_location.transform_to("icrs")
        logger.debug(f"Converted target data to icrs = {icrs_value_target}")

        mount_angles: tuple[float,float] = astropy_positions.get_mount_angles(icrs_value_target,
                                            observer_location, current_time)

        print(f"Hour angle = {mount_angles[0]:.2f}\n"
              f"Declination = {mount_angles[1]:.2f}\n"
              f"Right Ascension = {mount_angles[2]:.2f}\n"
              f"Altitude = {target_location.alt.deg:.2f}\n")
        


"""Saved Locations"""
#Southport UK
l1_lat = 53.6311
l1_lon = -2.9925
l1_height = 6
l1_location = EarthLocation(lat=(test_lat * u.deg),lon=(test_lon * u.deg),
                              height=(test_height * u.m))