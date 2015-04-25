#!/usr/bin/python           # This is server.py file

import socket               # Import socket module

def start_server():
	print "Launching server"
	s = socket.socket()         # Create a socket object
	host = socket.gethostname() # Get local machine name
	print host
	port = 8888              # Reserve a port for your service.
	s.bind((host, port))        # Bind to the port
	s.listen(5)                 # Now wait for client connection.

	while True:
	   c, addr = s.accept()     # Establish connection with client.
	   print 'Got connection from', addr
	   print c
	   while True:
	   	print "------------------"
	   	print c.recv(2000)
	   	print "------------------"
	   	c.send('Thank you for connecting')
	   c.close()

start_server()


