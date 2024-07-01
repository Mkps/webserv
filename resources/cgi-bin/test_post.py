#!/usr/bin/env python
import os
import sys

print("Content-Type: text/plain\n")
print("Environment Variables:")
for key, value in os.environ.items():
    print(f"{key}={value}")

print("\nPOST Data:")
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length)
print(post_data)
