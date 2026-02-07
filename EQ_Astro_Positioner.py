"""
ASTEPS - Astronomical Equatorial Positioning System


ASTEPS is a person portfolio project where i will me combining my electrical engineering knowledge with Python programming to create an automated telescope mounting system which can be retrofitted to manual equatorial telescope mounts.

This will be a designed as a personal astrophotography and observation tool for my 135 diameter, 800mm Telescope with a manual equatorial telescope
"""
#=====================================================================#
"""Imports & Modules"""
from astropy.time import Time #For getting local time
from astropy.coordinates import EarthLocation #For location
from astropy.coordinates import SkyCoord #For sky object coordinates
    #u.ra - Right ascension
    #u.dec - Declination
    #u.from_name - Gets coords from object name
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

#=====================================================================#
"""Functions"""
#Get safe input (from my reusable function collection)
def safe_input(prompt: str) -> str:
    """
    Function to validate safe input 
    Arguments:
      prompt - string for input prompt
    Return:
        Input data or empty string if error occurs
    """
    try:
        return input(prompt)
    except (KeyboardInterrupt, EOFError):
        print("\nInput Error")
        return ""


#Get non-empty string (from my reusable function collection)
def get_non_empty_string(prompt: str) -> str:
    """
    Function to get non empty string using while loop
    Utilizes safe_input function for input
    Arguments:
        prompt - string for input prompt
    Return:
        Non-empty string
    """
    while True:
        raw_input: str = safe_input(prompt)
        sanitized_input: str = raw_input.strip()

        if sanitized_input == "":
            print("\nError: Input cannot be empty")
            continue
        else:
            return sanitized_input


#Get float with optional range (from my reusable function collection)
def get_valid_float(prompt, min_value: float = None,
                     max_value: float = None) -> float:
    """
    Function to get a valid float value within a range if given
    Utilizes safe_input function for input
    Arguments:
        prompt - string for float input prompt
        min_value - minimum float value
        max_value - maximum float value
    Returns:
        valid float
    """
    while True:
        raw_input = safe_input(prompt)
        sanitized_input = raw_input.strip()

        if not sanitized_input:
            print("\nError: Input cannot be empty")
            continue

        try:
            float_value = float(raw_input)
        except ValueError:
            print("\nError: Invalid float value")
            continue

        if min_value is not None and float_value < min_value:
            print(f"\nError: Value must be between"
                  f" {min_value} and {max_value}")
            continue
        if max_value is not None and float_value > max_value:
            print(f"\nError: Value must be between"
                  f" {min_value} and {max_value}")
            continue

        return float_value
    

#Get telescope location
def get_location_info() -> EarthLocation:
        """
        Function to get user location 
        """
        while True:
            latitude: float = get_valid_float( #Get latitude
                "Enter telescope latitude: ", -90, 90)

            longitude: float = get_valid_float( #Get longitude
                "Enter telescope longitude: ", -180, 180)

            sea_level: float = get_valid_float( #Gets sea level
                "Enter telescope sea level(m): ", -500, 8000)
            
            #convert to astropy units
            u_lat = (latitude * u.deg)
            u_lon = (longitude * u.deg)
            u_height = (sea_level * u.m)

            print(f"\nCurrent Information\n"
                  f"Latitude: {latitude}°\n"
                  f"Longitude: {longitude}°\n"
                  f"Sea level: {sea_level}m\n"
                )
            while True: #Input confirmation
                confirm = safe_input("Is this correct? (Y/N)").lower()
                if confirm == "y":
                    break
                elif confirm == "n":
                    break
                else:
                    print("Enter (Y/N)")
                    continue
            if confirm == "y":
                break
        return EarthLocation(lat=u_lat, lon=u_lon, height=u_height)


#Get telescope target location (except solar system objects)
def get_target_location(t_location: EarthLocation,
                        l_time: Time) -> SkyCoord:
    """
    Function that finds the coordinate of the object in relation 
    to the user location, except solar system objects where a 
    different function is called
    Arguments:
        t_location - The user / telescope location
        l_time - The users local time
    Returns:
        Location in a SkyCoord value
    """
    while True:
        try:
            local_objects_check = [
            "sun","mercury", "venus", "earth-moon-barycenter", "earth", "moon",
            "mars", "jupiter", "saturn","uranus", "neptune", "pluto"]

            object_name = get_non_empty_string("Enter target name: ")

            if object_name.lower() in local_objects_check:
                print("Local object identified")
                return get_planet_location()

            target_ra_dec = SkyCoord.from_name(object_name)
            observer_frame = AltAz(obstime=l_time, location=t_location)
            target_location: SkyCoord = target_ra_dec.transform_to(observer_frame)
            return target_location
        
        except NameResolveError:
            print(f"\nError: {object_name} Not fount")
            continue


#Get planet location
def get_planet_location():
    local_objects = [
        "sun","mercury", "venus", "earth-moon-barycenter", "earth",
        "moon", "mars", "jupiter", "saturn","uranus", "neptune", "pluto"]
    #To be developed
    return

#=====================================================================#
"""Testing variables"""
#Test location (Southport UK)
test_lat = 53.6311
test_lon = -2.9925
test_height = 6
test_location = EarthLocation(lat=(test_lat * u.deg),
        lon=(test_lon * u.deg),height=(test_height * u.m))

#testing calls
# current_time = Time.now() #Get current time
# target_location = get_target_location(test_location, current_time)

#=====================================================================#
"""Main code"""
if __name__ == "__main__":
    print("Starting Program")
    current_time = Time.now() #Get current time
    observer_location: EarthLocation = (get_location_info()) #Get location info
    target_location = get_target_location(observer_location, current_time)
    print(f"target: {target_location}\n location: {observer_location}")