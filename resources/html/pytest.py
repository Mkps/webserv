import os
from os import environ
import cgi

# Read from stdin (for CGI)
method = os.environ['REQUEST_METHOD']
form = cgi.FieldStorage()

firstName = form.getvalue('firstName')
lastName = form.getvalue('lastName')
mailAddress = form.getvalue('mailAddress')
if firstName and lastName and mailAddress:
    result = "<ul><li>" + firstName + "</li><li>" + lastName + "</li><li>" + mailAddress + "</li></ul>"
else:
    result = "Error: Missing operation or string"

# CGI response
print("Content-Type: text/html\r\n")
if method == "GET":
    print("<h2>Get Parameters</h2>")
elif method == "POST":
    print("<h2>POST Parameters</h2>")
else:
    print("Invalid method")

print()
print(result)
