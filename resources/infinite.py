#!/usr/bin/env python

import os
from os import environ
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

while True:
    print("loop")
