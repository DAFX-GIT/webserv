#!/usr/bin/python3
import sys

def main():
    try:
        # 1. READ THE UN-CHUNKED BODY FROM WEBSERV
        # Since your webserv closes the write-end of the pipe, Python hits EOF and stops reading cleanly.
        received_body = sys.stdin.read()

        # 2. CGI Headers
        print("Content-Type: text/html\r")
        print("\r")
        
        # 3. HTML Content Body showing what we captured
        print("<!DOCTYPE html>")
        print("<html>")
        print("<head><title>CGI Body Tester</title></head>")
        print("<body>")
        print("<h1>Hello from Python CGI!</h1>")
        print(f"<p>Python Version: {sys.version.split()[0]}</p>")
        
        print("<h2>Data Received From Server:</h2>")
        if received_body:
            # We wrap it in a <pre> tag so we can see spaces/newlines exactly as sent
            print(f"<pre style='background: #eee; padding: 10px;'>{received_body}</pre>")
            print(f"<p><b>Total Characters:</b> {len(received_body)} bytes</p>")
        else:
            print("<p style='color: red;'><i>[No body received or body was empty]</i></p>")
            
        print("</body>")
        print("</html>")
        
        sys.stdout.flush()
        
    except Exception as e:
        sys.stderr.write(f"Python Script Error: {str(e)}\n")
        sys.stderr.flush()
        sys.exit(1)

if __name__ == "__main__":
    main()