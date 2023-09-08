
To compile the two c files, http_client.c and 
http_server.c, type "make" in the directory, from the
command prompt to start the Makefile. The client can 
be a bit finnicky, although it will generally not run
into parsing errors for any commands targetting files.
In the case that the command promptdoesn't reappear 
after receiving a message due to buffering errors, just
press ctrl+c.

To send request as client:
./http_client -p (optional) host/path port_number
To start Server:
./http_server port_number
To Kill Server:
./http_client -p linux.wpi.edu/SIGTERM port_number 

Any output will be saved in the output file, and 
will thus need to have its file system changed. 
The server will stay on unless either SIGTERM is 
sent to the server or ctrl+c is used in the command 
line. 