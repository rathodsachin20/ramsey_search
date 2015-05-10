'''
    Simple socket server using threads
'''
 
import socket
import sys
from thread import *
import json
import os

count = 1
broadcastFlag = True
results = {}
graphs = {}

# Processing mesg
def process(mesg):
    os.system("cd solutions; mkdir " + str(mesg["gsize"])) 

    if (mesg["msg_type"] == "counter-example"):
        fname = "solutions/" + str(mesg["gsize"]) + "/CE-" + str(mesg["gsize"]) + ".txt"
        fp = open(fname, "w")
        fp.write(json.dumps(mesg))
    else:
        fname = "solutions/" + str(mesg["gsize"]) + "/update" + ".txt"

# broadcast chat messages to all connected clients
def broadcast ():
    #  When converting to json check if result is best
    # At the end send messages to all the nodes with the best graph and the list of moves
    
    global count
    global broadcastFlag
    global results
    global graphs

    broadcastFlag = False
    Tmaxgsize = 0
    Tmincount = 9999999
    Tbestidx = -1
    Tconn = -1

    print "Broadcasting ...."
    for conn in results:
        idx = 0
        maxgsize = 0
        mincount = 9999999
        bestidx = -1
        for val in results[conn]:
            if val["gsize"] > maxgsize:
                maxgsize, bestidx = val["gsize"], idx
            elif val["gsize"] == maxgsize & val["count"] < mincount:
                mincount, bestidx = val["count"], idx
            idx +=1

        if maxgsize > Tmaxgsize:
            Tmaxgsize,  Tbestidx, Tconn = maxgsize, bestidx, conn
        elif Tmaxgsize == maxgsize & mincount < Tmincount:
            Tmincount,  Tbestidx, Tconn = mincount, bestidx, conn

    

    print "Best graph is :"+json.dumps(results[conn][bestidx])

    for conn in results:
        solution = "graph: "+graphs[Tconn][Tbestidx]
        conn.sendall(solution)

    results = {}
    graphs = {}
    count = 0
    broadcastFlag = True

#Function for handling connections. This will be used to create threads
def clientthread(conn):
    conn.send('Welcome to the server. Type something and hit enter\n') #send only takes string
    
    global count
    global broadcastFlag
    global results
    global graphs

    key = ''
    graphdata = ''
    while True:
        # Server recives data from client
        data = conn.recv(1024).strip()
        data = str(data.decode('utf-8')).replace('\x00', '')
        if not data:
            print "Client ", conn, "has failed"
            break

        if data:
            if "gsize" in data: 
                # Server sends reply to the client
                reply = " --- "
                conn.sendall(reply)
                
                # Add prev graph to dictionary
                if graphdata:
                    if not conn in graphs:
                        graphs[conn] = []
                    graphs[conn].append(graphdata)
                    graphdata = ''

                # Add json to the dictionary
                data = data.replace("'",'"') 

                if not conn in results:
                    results[conn] = []

                if "{" in data:
                    dataJSON = json.loads( data )
                    results[conn].append( dataJSON )

                if count > 10 and broadcastFlag:
                    broadcast()
                
                count += 1
                print "@count:",count

            if "01" in data:
                graphdata +=data
     
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
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
     
    start_new_thread(clientthread ,(conn,))

s.close()



 

