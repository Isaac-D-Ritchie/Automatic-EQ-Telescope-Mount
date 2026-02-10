"""
This file contains all of the positioning functions used to transform
the user inputs into astrological coordinates for the telescope.

This file imports functions from other files in /software (python)
    # config_and_logging
    # reuseable_functions)
"""



"""Imports"""
from astropy.time import Time #For getting observers local time
from astropy.coordinates import EarthLocation #For observer location
from astropy.coordinates import SkyCoord #For target coordinates
    #u.ra - Right ascension
    #u.dec - Declination
    #u.from_name - Gets coords from object name
from astropy.coordinates import get_body #For solar system objects
    #u.get_body("object") 
from astropy.coordinates.name_resolve import NameResolveError
from astropy.coordinates import AltAz #For checking horizon location
    #u.alt - gets altitude
    #u.az - gets azimuth
import astropy.units as u #Astropy units:
    #u.deg = degrees  
    #u.rad = radians
    #u.m = meters
    #u.s = seconds
    #u.hourangle = hours used for RA
    #u.arcsec = arcseconds

import reuseable_functions #for safe inputs
from config import logger


"""Functions"""
def get_time() -> Time:
    current_time = Time.now()
    return current_time


def get_location_info() -> EarthLocation:
        """
        Function to get user/telescope location
        """
        while True:
            latitude: float = reuseable_functions.get_valid_float( #Get latitude
                "Enter telescope latitude: ", -90, 90)

            longitude: float = reuseable_functions.get_valid_float( #Get longitude
                "Enter telescope longitude: ", -180, 180)

            sea_level: float = reuseable_functions.get_valid_float( #Gets sea level
                "Enter telescope sea level(m): ", -500, 8000)
            
            #convert to astropy units
            u_lat = (latitude * u.deg)
            u_lon = (longitude * u.deg)
            u_height = (sea_level * u.m)

            logger.info("Location data parsed from user")
            print(f"\nCurrent Information\n"
                  f"Latitude: {latitude}°\n"
                  f"Longitude: {longitude}°\n"
                  f"Sea level: {sea_level}m"
                )
            while True: #Input confirmation
                confirm = reuseable_functions.safe_input("Is this correct? (Y/N)").lower()
                if confirm == "y":
                    break
                elif confirm == "n":
                    logger.info("Location voided by user")
                    break
                else:
                    continue

            if confirm == "y":
                logger.info("Location validated by user")
                break

        location: SkyCoord = EarthLocation(lat=u_lat, lon=u_lon, height=u_height)
        logger.debug(f"Location: {location}")
        return location


def get_target_location(t_location: EarthLocation,
                        l_time: Time) -> SkyCoord:
    """
    Function that finds the coordinate of the object in relation to the user
    location, using get_body for local objects (planets) and from_name for 
    non-local objects (stars).

    Arguments:
        t_location - The user / telescope location.
        l_time - The users local time.

    Returns:
        Location in a SkyCoord value.
    """
    local_objects_check = [
            "sun","mercury","venus","earth-moon-barycenter","earth","moon",
            "mars","jupiter","saturn","uranus","neptune","pluto"]
    while True:
        try:
            object_name = reuseable_functions.get_non_empty_string("Enter target name: ")
            logger.info("Parsed sky target name from user")
            observer_frame = AltAz(obstime=l_time, location=t_location)

            if object_name.lower() in local_objects_check:
                print("Local object identified")
                logger.info("Local object detected")
                target_ra_dec = get_body(object_name, l_time)
                logger.info("Local object data parsed successfully")
                target_location = target_ra_dec.transform_to(observer_frame)

                if target_location.alt < 0 * u.deg: #Checks if above horizon
                    print("Object below horizon, try again")
                    logger.warning("Object below horizon")
                    continue

                logger.debug(f"Target location = {target_location}")
                return target_location

            logger.info("Deep sky object detected")
            target_ra_dec = SkyCoord.from_name(object_name.lower())
            logger.info("Deep sky object data parsed successfully")
            target_location = target_ra_dec.transform_to(observer_frame)
            print("Deep sky object identified")

            if target_location.alt < 0 * u.deg: #Checks if above horizon
                print("Object below horizon, try again")
                logger.warning("Object below horizon")
                continue

            logger.debug(f"Target location: {target_location}")
            return target_location
        
        except NameResolveError:
            print(f"\nError: {object_name} Not fount")
            logger.error("Object not found")
            continue


def get_mount_angles(icrs_data: SkyCoord, location: EarthLocation, 
                     local_time: Time) -> tuple[float, float, float]:
    """
    Function takes all the gathered user data and calculates the equatorial
    angles for the mount. 

    Arguments:
        icrs_data - International Celestial Reference System format.
                    The target sky coordinates.
        location - The observers location.
        local_time - The observers local time.

    Returns:
        A tuple of the hour angle and declination containing:
            HA (Hour angle) - Dynamic angle depending on time and location.
            DEC (Declination) - Fixed for deep sky objects, changes for local.
            RA (Right ascension) - Fixed object coordinate.
    """
    local_sidereal_time = local_time.sidereal_time("apparent", #Local RA
                                                   longitude=location.lon)
    logger.debug(f"LST = {local_sidereal_time}")

    dec_deg = icrs_data.dec.deg
    ra_deg = icrs_data.ra.deg
    logger.info("RA and DEC successfully parsed from icrs data")

    #The hour angle = (LST - RA)
    hour_angle = (local_sidereal_time - icrs_data.ra).wrap_at(180 * u.deg)
    hour_angle_deg = hour_angle.to(u.deg).value #For tracking
    logger.debug(f"Calculated local hour angle at {hour_angle_deg}")

    motor_data_tuple = (hour_angle_deg, dec_deg, ra_deg)
    logger.info("Generated position data for mount")
    logger.debug(f"data = {motor_data_tuple}")
    return motor_data_tuple