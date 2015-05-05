'''
    Simple socket server using threads
'''
 
import socket
import sys
from thread import *
import json
import os

HOST = 'localhost'   # Symbolic name meaning all available interfaces
PORT = 8888 # Arbitrary non-privileged port
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Socket created'
 
#Bind socket to local host and port
try:
    s.bind((HOST, PORT))
except socket.error as msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
     
print 'Socket bind complete'
 
#Start listening on socket
s.listen(10)
print 'Socket now listening'
 
def process(mesg):
    os.system("cd solutions; mkdir " + str(mesg["gsize"])) 

    if (mesg["msg_type"] == "counter-example"):
#''' Currently write to a file, should write to S3 using boto '''
        fname = "solutions/" + str(mesg["gsize"]) + "/CE-" + str(mesg["gsize"]) + ".txt"
        print fname
	fp = open(fname, "w")
	fp.write(json.dumps(mesg))
    else:
        fname = "solutions/" + str(mesg["gsize"]) + "/update" + ".txt"

        #do something else

     
#Function for handling connections. This will be used to create threads
def clientthread(conn):
    #Sending message to connected client
    #infinite loop so that function do not terminate and thread do not end.
    while True:
        data = conn.recv(1024)
        print data.strip()
        mesg = json.loads(data.strip())
        process(mesg)
        if not data: 
            break
     
        
     
    #came out of loop
        conn.close()
	break;
 
#now keep talking with the client
while 1:
    #wait to accept a connection - blocking call
    conn, addr = s.accept()
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
     
    #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
    start_new_thread(clientthread ,(conn,))
 
s.close()
