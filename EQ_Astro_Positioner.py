from __future__ import annotations
"""
ASTEPS - Astronomical Equatorial Positioning System


ASTEPS is a person portfolio project where i will me combining my electrical engineering knowledge with Python programming to create an automated telescope mounting system which can be retrofitted to manual equatorial telescope mounts.

This will be a designed as a personal astrophotography and observation tool for my 135 diameter, 800mm Telescope with a manual equatorial telescope
"""

"""Imports & Modules"""
from astropy.time import Time


"""Functions"""
#Get safe input
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

#Get non-empty string
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


"""Main code"""
if __name__ == "__main__":
    print("Start Program")
    t = Time.now()
    print(t)