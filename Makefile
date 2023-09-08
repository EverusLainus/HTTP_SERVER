http_server: process_msg.o http_server.o
	gcc process_msg.o http_server.o -o http_server

process_msg.o: process_msg.c
	gcc -c process_msg.c

http_server.o: http_server.c
	gcc -c http_server.c