"""
This file contains all the functions which are reused multiple times
in the main code for validating safe inputs from the user.  
"""


"""Imports"""
from config import logger


"""Functions"""
def safe_input(prompt: str) -> str:
    """
    Function to validate safe input.

    Arguments:
      prompt - String for input prompt.

    Return:
        Input data or empty string if error occurs.
    """
    try:
        return input(prompt)
    except (KeyboardInterrupt, EOFError):
        print("\nInput Error")
        return ""


def get_non_empty_string(prompt: str) -> str:
    """
    Function to get non empty string using while loop Utilizes safe_input
    function for input.

    Arguments:
        prompt - String for input prompt.

    Return:
        Non-empty string.
    """
    while True:
        raw_input: str = safe_input(prompt)
        sanitized_input: str = raw_input.strip()

        if sanitized_input == "":
            print("\nError: Input cannot be empty")
            logger.error("Empty Input")
            continue
        else:
            return sanitized_input


def get_valid_float(prompt, min_value: float = None,
                     max_value: float = None) -> float:
    """
    Function to get a valid float value within a range if given Utilizes
    safe_input function for input.

    Arguments:
        prompt - String for float input prompt.
        min_value - Minimum float value.
        max_value - Maximum float value.

    Returns:
        Valid float.
    """
    while True:
        raw_input = safe_input(prompt)
        sanitized_input = raw_input.strip()

        if not sanitized_input:
            print("\nError: Input cannot be empty")
            logger.error("Empty Input")
            continue

        try:
            float_value = float(raw_input)
        except ValueError:
            print("\nError: Invalid float value")
            logger.error("Invalid float input")
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
    
