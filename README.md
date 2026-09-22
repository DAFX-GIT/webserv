
*This project has been created as part of the 42 curriculum by adastugu, etessoer, flomulle.*

# WebServ

## Description

 WebServ is a lightweight, single-threaded, non-blocking HTTP/1.1 server built from scratch in C++98, without any external web server libraries. It mimics a subset of the behaviour of production servers like Nginx, focusing on the fundamentals of HTTP communication, concurrent connections, and file serving. The goal is to understand how web servers work internally by implementing the core features of HTTP communication.

### Features

- Configuration file parsing
- HTTP/1.1 request parsing
- GET, POST and DELETE methods
- Static file serving
- File uploads
- CGI execution (php, python)
- HTTP redirections
- Cookie handling
- Accurate error codes and default error pages
- Non-blocking sockets, use of one epoll
- Multiple virtual servers
- Multiple simultaneous clients

---

## Instructions

### Requirements

- Linux
- C++98 compatible compiler
- Make

### Compilation

```bash
make
````

### Launch

```bash
./webserv [configuration file]
```

Example:

```bash
./webserv conf/webserv.conf
```

The server will listen on the ports specified in the configuration file.


### Testing

Open a browser:

```text
http://localhost:8080
```

Or use curl:

```bash
curl http://localhost:8080

curl -X POST http://localhost:8080/upload

curl -X DELETE http://localhost:8080/file.txt
```

---

## Configuration Example

```conf
server {
	listen 8080
	server_name localhost

	location / {
		root ./content
		index index.html
		allowed_methods GET POST
	}

	location /cgi {
		root ./
		isCgi	YES
		cgi_assign {
			/usr/bin/php-cgi	.php
			/usr/bin/python3 	.py
		}
		allowed_methods GET
	}
}
```

---

## Architecture

### Workflow Summary (Client Request → Response)

1. Connection accepted → Client created, added to epoll.

2. Data arrives (EPOLLIN) → HTTPRequest::parseChunk reads incrementally until request complete.

3.  Request complete → process() matches location, selects handler.

4.  Handler fills oBuffer or delegates to async operation (CGI/upload).

5.  If async → client state changes, epoll events updated.

6.  When response ready → client state WRITING_RESPONSE, EPOLLOUT sends data.

7.  Response fully sent → client connection closed and cleaned up.


### Core Data Structures
| Structure | Purpose |
| -- | ----- |
Config	| Global configuration container: server blocks, types, error pages, epoll file descriptor, binary path.
ServerConfig	| Holds a server’s port, server name, and a list of Location objects.
Location	| Defines routing rules: prefix, root directory, allowed HTTP methods, index file, autoindex flag, CGI flag, interpreter mapping, upload directory, redirection settings.
HTTPRequest	| Parses an incoming HTTP request. Stores method, path, protocol, headers, body, and a parsing state machine.
Client	| Represents an active client connection. Contains the HTTP request, I/O buffers, client state, timers, CGI child PID/pipes, and upload state.
Connection	 | A wrapper used inside epoll to differentiate between server listening sockets, client connections, and CGI pipes. Holds a type (SERVER/CLIENT/CGI) and a data pointer (e.g., to a Client).
ParseState / ClientState	| Enums that drive the HTTP parsing and the client’s lifecycle.

### Startup & Configuration

 - main() reads the configuration file (given as argument) and calls Config::parseConf().

 - parseConf() parses server blocks (server { … }), location blocks (location /path { … }), error pages, and types (from conf/types.conf).

 - For each unique port found in server blocks, initServ() creates a non‑blocking listening socket and binds it.

 - serv() creates the epoll instance, registers all listening sockets with EPOLLIN, and enters the main event loop.

### Event Loop (serv.cpp)

The main loop uses epoll_wait() with a timeout of 5000 ms. Every event is handled according to the Connection::type:

 - SERVER → accept new client, set client socket non‑blocking, create a Client and a Connection of type CLIENT, add to epoll with EPOLLIN.

 - CLIENT → events EPOLLIN or EPOLLOUT (handled separately).

 - CGI → reading from the CGI script’s output pipe or handling pipe closure/errors.

After processing events, the loop scans all active connections for timeouts:

 - Client idle > CLIENT_TIMEOUT_SEC → connection closed.

 - CGI script running longer than CGI_TIMEOUT_SEC → kill the child process and return a 504 error.

 ### HTTP Request Parsing (http.cpp)

Each Client has an HTTPRequest object and an input buffer iBuffer. The parseChunk() method implements a state machine:

1. PARSE_REQUEST_LINE
Reads until \r\n or \n. Splits into method, path, protocol. On failure → handleBadRequest() (400).

2. PARSE_HEADERS
Reads lines until an empty line. Stores headers in a map. If Content-Length is present, switches to PARSE_BODY with known length. If Transfer-Encoding: chunked, also goes to PARSE_BODY.

3. PARSE_BODY

- For Content-Length: reads exactly that many bytes.

- For chunked encoding: parses hex chunk sizes, removes the chunk headers, and appends data to the body.

- When the body is complete, state becomes PARSE_COMPLETE.

After parsing completes, the client’s state becomes PROCESSING_RESPONSE.

### Request Processing (process.cpp, method/*.cpp)

process() is called when a complete HTTP request is ready.

- It extracts the Host header, determines the correct ServerConfig (by port and server name), and finds the matching Location by longest‑prefix match.

- If a redirection is configured (return 301 ...), handleRedirection() generates the response.

- If the location has isCgi = YES, execCgi() is invoked.

- Otherwise, it dispatches by HTTP method:

  - GET → handleGet()

    - If path equals location prefix and autoindex is enabled → generate directory listing.

    - Else serve the file (check for .. traversal, use root + path).

  - POST

    - Special paths /cookie and /killcookie set/delete cookies.

    - If Content-Type contains multipart/form-data → handleNativeUpload() (async file upload).

    - Other POST requests are not implemented (empty handler).

  - DELETE → handleDelete()

     - Checks .., path safety, then calls remove(). Returns 200 or an error.

   - Other methods (HEAD, PUT, etc.) → 501 Not Implemented.

   - Unsupported method for the location → 405.

If the response can be generated immediately (no CGI, no upload), the oBuffer is filled and the client state becomes WRITING_RESPONSE. The epoll event is modified to EPOLLOUT.


### CGI Execution (execCgi.cpp)

 - Determines the CGI interpreter (e.g., .py → /usr/bin/python3) and the script path from the location’s cgi_interpreters map.

 - Extracts optional PATH_INFO and QUERY_STRING from the request path.

 - Opens the script file to verify existence, then:

    1. Creates two pipes (in_pipe, out_pipe).

    2. fork().

    3. Child process:

         - dup2 to redirect stdin/stdout to the pipes.

		 - Sets CGI environment variables (REQUEST_METHOD, QUERY_STRING, PATH_INFO, SCRIPT_FILENAME, HTTP_COOKIE, etc.).

		 - For POST, sets CONTENT_LENGTH and CONTENT_TYPE.

         - Calls execve() with the interpreter and script path.

    4. Parent process:

    	 - Stores cgi_pid, cgi_fd (read end of out_pipe).

         - Writes the POST body (if any) into in_pipe and closes it.

         - Makes cgi_fd non‑blocking and creates a Connection of type CGI, adding it to epoll with EPOLLIN.

         - Client state → WAITING_CGI

When the CGI pipe becomes readable in the main loop:

- Data is read into curClient->cgi_buffer.

 - If read returns 0 or EPOLLHUP occurs, the pipe is closed. The server then:

    - Cleans up the CGI connection, waits for the child process (kills it if necessary).

     - Calls cgi_response() to parse the CGI output (split headers and body, look for a Status: header).

     - Builds the final HTTP response and sets client state to WRITING_RESPONSE.

### File Upload (post.cpp + serv.cpp)

handleNativeUpload() parses a multipart/form-data body:

 - Finds the boundary, extracts the filename.

- Locates the file data section and stores it in curClient->upload_buffer.

 - Opens the destination file (loc->upload_dir + filename) and stores the file descriptor.

 - Client state → WAITING_UPLOAD.

 -  The event is modified to EPOLLOUT.

Inside the EPOLLOUT handler for a client in WAITING_UPLOAD:

 -  Writes the buffered data in chunks to the upload file descriptor.

-  On completion, sends a 201 Created response.

-   On write error, returns a 500 error.

### Writing the Response

When the client state is WRITING_RESPONSE:

- The EPOLLOUT handler calls write() on the client socket, sending as much as possible from oBuffer.

- After each successful write, the sent portion is erased.

- When oBuffer becomes empty, the client is deleted (and its associated resources, including any CGI connection).

### Error Handling & Cleanup

- getError() generates an error page:

	- Looks for a custom error page in the configuration (error_page directive).

	 - Falls back to a default webserv page in /tmp/WebServ/DefaultError/*.

	 - Falls back to a built‑in default page.

	- Builds a complete HTTP response (status, headers, body).

- The Connection destructor automatically removes the file descriptor from epoll, closes it, and deletes the associated Client (and cascades to cleanup of CGI connections).

- The main loop’s timeout scanner kills long‑running CGI scripts and terminates idle clients.

### Security Considerations

- Path traversal attempts (..) are blocked in handleGet, handleDelete, and isPathSafe().

- isPathSafe() uses realpath to ensure that the requested file resides inside the server’s binary directory (sandbox).

- CGI environment is carefully set; no user‑supplied data goes directly into execve arguments except the script path.

---

## Error Handling

Custom error pages can be configured for specific status codes.

If a configured error page is missing, WebServ automatically generates a default error page.

Supported error codes include:

* 400 Bad Request
* 403 Forbidden
* 404 Not Found
* 405 Method Not Allowed
* 413 Payload Too Large
* 500 Internal Server Error
* 501 Not Implemented
* 502 Bad Gateway
* 503 Service Unavailable
* 504 Gateway Timeout
* 505 HTTP Version Not Supported


---

## Resources

### Recommanded functions

It is recommanded to use these functions, some were new to us so the linux man pages were obviously useful.

* execve
* waitpid
* pipe
* strerror - gai_strerror - errno
* dup - dup2
* fork
* socketpair
* htons - htonl - ntohs - ntohl
* select
* poll
* epoll - epoll_create - epoll_ctl - epoll_wait
* kqueue - kqueue - kevent
* socket
* accept
* listen
* send - recv
* chdir
* bind
* connect
* getaddrinfo - freeaddrinfo
* setsockopt - getsockname - getprotobyname
* fcntl
* close - read - write
* kill - signal
* access
* stat
* open - opendir - readdir - closedir

Specifically, these man pages :
* man 7 epoll – great guide to epoll in Linux
* man 2 epoll_create, man 2 epoll_ctl, man 2 epoll_wait
* man 2 fcntl (for O_NONBLOCK)
* man 2 socket, man 2 setsockopt
* man 2 poll / man 2 select were useful for understanding event loops before tackling epoll, though we ended up with epoll.

Although the subject states: *Make sure to leverage as many C++ features as possible (e.g., choose <cstring>
over <string.h>). You are allowed to use C functions, but always prefer their C++
versions if possible.* So we can basically use any function we consider useful.

### HTTP

* [RFC 2616 — HTTP/1.1 Hypertext Transfer Protocol](https://datatracker.ietf.org/doc/html/rfc2616)
* [RFC 7230 — HTTP/1.1 Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230)
* [RFC 7231 — HTTP/1.1 Semantics and Content](https://datatracker.ietf.org/doc/html/rfc7231)
* [RFC 3875 — CGI Version 1.1](https://datatracker.ietf.org/doc/html/rfc3875)
* [RFC 3986 – Uniform Resource Identifier (URI): Generic Syntax](https://datatracker.ietf.org/doc/html/rfc3986)

### Documentation

* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/)


---
## AI Usage

Artificial Intelligence has been used during the development of this project:

* During the discovery phase, to understand what was expected from a webserver and to understand HTTP and CGI specifications
* When we did not know what was the expected result, we researched Nginx behavior
* Creating test scenarios
* Base readme structure

All architecture decisions, code writing, debugging, testing and validation were performed by us.

---








# webserv
