import socket
import sys
import json
import os
import time

# Create a TCP/IP socket
#sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 8888)
print >>sys.stderr, 'connecting to %s port %s' % server_address

#sock.connect(server_address)

json_CE = {}
json_upd = {}

json_CE["msg_type"] = "counter-example"
json_CE["client"] = socket.gethostname()

json_upd["msg_type"] = "update"
json_upd["client"] = socket.gethostname()

count = 170;
while count < 206:
	fname = "solutions/CE-" + str(count) + ".txt"
	
	if not os.path.exists(fname):
		print fname
		#time.sleep(10)
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect(server_address)
		fname = "solutions/CE-" + "29" + ".txt"
		f = open(fname,"r")
		line = f.readline()
		data = f.read()
		print data
		sock.send(data)
		data = sock.recv(1024).strip()
		if data:
			print "server sent", data	
		sock.close()	
		break
		continue	

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(server_address)
	f = open(fname, "r")
	
	line = f.readline()
	ascii_encode = line.split(" ")[1]
	json_CE["gsize"] = count
	json_CE["g_data"] = ascii_encode
	print json_CE["gsize"], json_CE["msg_type"]

	sock.send(json.dumps(json_CE))	
	sock.close()
	count += 1
	



