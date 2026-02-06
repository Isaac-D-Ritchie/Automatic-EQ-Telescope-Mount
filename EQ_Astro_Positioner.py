"""
ASTEPS - Astronomical Equatorial Positioning System


ASTEPS is a person portfolio project where i will me combining my electrical engineering knowledge with Python programming to create an automated telescope mounting system which can be retrofitted to manual equatorial telescope mounts.

This will be a designed as a personal astrophotography and observation tool for my 135 diameter, 800mm Telescope with a manual equatorial telescope
"""
#=====================================================================#
"""Imports & Modules"""
from astropy.time import Time #For getting time
from astropy.coordinates import EarthLocation #For location
from astropy.coordinates import SkyCoord #For sky object coordinates
from astropy.coordinates import AltAz #For checking horizon location
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
    
def get_location_info() -> list[dict]:
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
            u_lat: float = (latitude * u.deg)
            u_lon: float = (longitude * u.deg)
            u_height: float = (sea_level * u.m)

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
        return {
            "latitude": u_lat, "longitude": u_lon, "sea_level": u_height
        }

#=====================================================================#
"""Main code"""
if __name__ == "__main__":
    print("Starting Program")
    observer_location: dict = (get_location_info()) #Get location info
    t : str = Time.now() #Get current time