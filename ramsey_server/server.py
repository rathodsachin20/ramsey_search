'''
    Simple socket server using threads
'''
 
import socket
import sys
from thread import *
import json
import os

# Processing mesg
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


# broadcast chat messages to all connected clients
def broadcast (server_socket, sock, message):
    print "Broadcasting"
    for socket in SOCKET_LIST:
        # send the message only to peer
        if socket != server_socket and socket != sock :
            try :
                socket.send(message)
            except :
                socket.close()
                if socket in SOCKET_LIST:
                    SOCKET_LIST.remove(socket)
     
#Function for handling connections. This will be used to create threads
def clientthread(conn):
    conn.send('Welcome to the server. Type something and hit enter\n') #send only takes string
     
    #infinite loop so that function do not terminate and thread do not end.
    while True:
        data = conn.recv(1024).strip()
        stringdata = data.decode('utf-8')
        print stringdata
        reply = 'OK...' + data
        print reply
        conn.sendall(reply)

        # If there is a FIFO update broadcast it to everyone we know
        if "fifo_update" in reply:
            broadcast(s, conn, reply)
        if not data: 
            break
        #conn.sendall(reply)
     
    #came out of loop
    conn.close()

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

SOCKET_LIST = []

#now keep talking with the client
while 1:
    #wait to accept a connection - blocking call
    conn, addr = s.accept()
    SOCKET_LIST.append(conn)
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
     
    #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
    start_new_thread(clientthread ,(conn,))
 
s.close()



 

