#!/usr/bin/env python

import os
from os import environ
import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
if "user" in form:
    print("Hello "+ form["user"].value + "! Im a pyton script. Nice to meet you!");
else
