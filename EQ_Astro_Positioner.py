"""
ASTEPS - Astronomical Equatorial Positioning System


ASTEPS is a person portfolio project where i will me combining my electrical engineering knowledge with Python programming to create an automated telescope mounting system which can be retrofitted to manual equatorial telescope mounts.

This will be a designed as a personal astrophotography and observation tool for my 135 diameter, 800mm Telescope with a manual equatorial telescope
"""

"""Imports"""
from __future__ import annotations
import json # Data parsing for API.
import requests # API module.
import os
import math # for EQ geometry
import time
import logging
