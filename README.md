# HTTP_SERVER
 
A simple HTTP server using epoll.

HTTP_SERVER uses epoll to look and wait for clients. HTTP_SERVER takes in port number as a argument. 
It Accepts 
 1. Two HTTP methods: GET and POST.
 2. Two Paths: / and /echo.
 3. One protocal : HTTP/1.0.

The can be any number of headers.
Acceptable Field Names: "Content-Length", "Content-Type" and "Host"
Field names on the headers are case and space sensitibe. Each field name is followed by a colon, single space and the actual content of the fild name. For example: Content-Length: 20
Double enter marks the end of the header. 

The body receives the number of characters give by the Content-Length. For example if the Content-Length is set to 20, the exactly 20 characters are received for the body. Body can hold atmost 255 characters. After receiving the connection to the client is closed and server keep looking for client-connections. HTTP_SERVER can connect to only one server at one time. Press triple enter to close the connecting if any warning occurs on the client side.

# What happens in the code:

## main

The main take program name and port number as arguments. If there are two arguments calls server function with port as it's argument. 
    
## server

The server function creates a socket fd to listen. Once it accepts a client socket fd, it receives and calls process_msg function till it get a double enter.
when it gets a double enter it calls process_msg one last time. It also calls post_echo().
## process_msg

process_msg calls the function first_part first. For second time when it is called it calls second_part function. At last it sends the content_length given by the header.

## first_part 
    
It checks if method matches GET or POST, path matches / or /echo, and method matches HTTP/1.0. If any of these did not match it sends an error message. 

## second_part

It splits the header into two and store it in two buffer. Right half as one after the colon and and left half as one before the colon.
It checks if the Field name is appropriate. and stores the content length in a static variable.

## post_echo

If the method is POST,  path is echo and content length is positive this function returns one. Else it returns zero.




    


