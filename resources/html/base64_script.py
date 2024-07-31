import sys
import base64
import cgi

def encode_decode(operation, string):
    if operation == 'encode':
        return base64.b64encode(string.encode('utf-8')).decode('utf-8')
    elif operation == 'decode':
        try:
            return base64.b64decode(string).decode('utf-8')
        except:
            return "Error: Invalid base64 string"
    else:
        return "Error: Invalid operation"

# Read from stdin (for CGI)
form = cgi.FieldStorage()

operation = form.getvalue('operation')
string = form.getvalue('string')

if operation and string:
    result = encode_decode(operation, string)
else:
    result = "Error: Missing operation or string"

# CGI response
print("Content-Type: text/html\r\n")
if (operation == 'encode'):
    print("<h2>Base64 encoded result</h2>")
elif (operation == 'decode'):
    print("<h2>Base64 decoded result</h2>")
print()
print(result)
