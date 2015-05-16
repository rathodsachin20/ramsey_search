import socket
import sys
import json
import os
import time
from threading import Thread
import subprocess
import signal
# Create a TCP/IP socket
#sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

ramsey_pid = 0
server_address = ('localhost', 8888)
def poll_for_CE():
# Connect the socket to the port where the server is listening
    global server_address
    print >>sys.stderr, 'connecting to %s port %s' % server_address

#sock.connect(server_address)

    json_CE = {}
    json_CE["msg_type"] = "counter-example"
    json_CE["client"] = socket.gethostname()

    count = 101
    while count < 135:
        fname = "solutions/CE-" + str(count) + ".txt"
	
        if not os.path.exists(fname):
            print fname
            time.sleep(10)
            continue	

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(server_address)
        f = open(fname, "r")
	
        line = f.readline()
        ascii_encode = line.split(" ")[1]
        json_CE["gsize"] = count
        json_CE["g_data"] = ascii_encode
        json_CE["host_name"] = socket.gethostname()
        graph = f.read()
        print json_CE["gsize"], json_CE["msg_type"]
        json_CE["gsize_bytes"] = len(graph)
        sock.send(json.dumps(json_CE))	

        data = json.loads(sock.recv(1024))
        if data["return"] == "ok":
            print "Sending actual graph" 
            sock.send(graph)
            print "waiting for server reply"
            data = json.loads(sock.recv(1024))
            print "server again sent", data	
        sock.close()
        count += 1
	
def poll_for_updates():
    global server_address
    json_upd = {}
    json_upd["msg_type"] = "update"
    json_upd["host_name"] = socket.gethostname()
    count = 101	
    while count < 130:
        fname = "solutions/CE-" + str(count) + "-upd.txt"
        print fname

        if not os.path.exists(fname):
            fname = "solutions/CE-" + str(count) + ".txt" 
            if os.path.exists(fname):
                count += 1
            else:
                time.sleep(10)
            continue	
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(server_address)
        f = open(fname,"r")
        line = f.readline()
        json_upd["gsize"] = line.split(" ")[0]
        json_upd["best_count"] = line.split(" ")[1]
        print "best_count", json_upd["gsize"]
        #read the entire graph
        graph = f.read()
        print graph
        json_upd["gsize_bytes"] = len(graph)
        sock.send(json.dumps(json_upd))
        
        data = json.loads(sock.recv(1024))
        
        if data["return"] == "ok":
            print "Sending actual graph" 
            sock.send(graph)
            print "waiting for server reply"
            data = json.loads(sock.recv(1024))
            print "server again sent", data	
        sock.close()
        count += 1


def act_on_broadcast():
    global server_address
    global ramsey_pid
    print "yeah broadcast"
    broadcast = {}
    while True:
   # Register with the server to recieve broadcast messages
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	
        sock.connect(server_address)
        broadcast["host_name"] = socket.gethostname()
        broadcast["msg_type"] = "broadcast"
        broadcast["ip"] = "localhost"
        broadcast["port"] = "8899"
        sock.send(json.dumps(broadcast))
        data = json.loads(sock.recv(1024).strip())
        if data["return"] == "ok":
            print "client registered with server ", server_address
            break
        else:
            print "client failed to register with server ", server_address
            
    sock.close()

  

    HOST = 'localhost'   # Symbolic name meaning all available interfaces
    PORT = 8899 # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print 'Broadcast Socket created'
    #Bind socket to local host and port
    try:
        s.bind((HOST, PORT))
    except socket.error as msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        exit()
    print 'Socket bind complete, listen now'
    #Start listening on socket
    s.listen(10)

    while 1:
    #wait to accept a connection - blocking call
        conn, addr = s.accept()
        print 'Connected with ' + addr[0] + ':' + str(addr[1])
         #Handle broadcast message from server. Kill ramsey search and start
          #  a new one with new graph. Update the new process pid 

        graph = {}
        data = json.loads(conn.recv(1024))
        gsize_bytes = int(data["gsize_bytes"])
        gsize = int(data["gsize"])
        print "got graph size - sending ok", gsize 
        conn.send(json.dumps({"return":"ok"})) 
        print "recieve the entire graph now"
        graph = conn.recv(gsize_bytes)
        
        print "recieved graph", graph
        conn.send(json.dumps({"return":"ok"}))
        #write graph to a file. start a new ramsey_search
        fname = write_graph_to_file(graph, gsize)
        print "killing process" + str(ramsey_pid)
        os.kill(ramsey_pid, signal.SIGKILL)
        start_ramsey_search(fname,gsize)
        
    s.close()
        
    

def write_graph_to_file(graph, gsize):
    fname = "graph-" + str(gsize) + ".txt"
    f = open(fname,"w")
    f.write(str(gsize) + "\n")
    f.write(graph)
    f.close() 
    return fname

''' input parameters constant ???'''
def start_ramsey_search(fname, size):
    global ramsey_pid
    executable = './ramsey_multi_flip ' +  str(size) + ' ' + fname
    print executable  
    proc = subprocess.Popen(executable, shell=True)
    time.sleep(1) # some buffer !!!
    ramsey_pid = proc.pid
    print ramsey_pid


''' for now starts with 101'''
def bootstrap_ramsey(size):
    global ramsey_pid
    argument = size
    executable = './ramsey_multi_flip ' + size
    print executable
    proc = subprocess.Popen( executable, shell=True)
    time.sleep(1) # some buffer !!!
    ramsey_pid = proc.pid
    print "bootstrapping done with pid %d " % ramsey_pid

threads = []
if  __name__ == "__main__":
    os.system("mkdir -p solutions/")
    bootstrap_ramsey("101")


    thread1 = Thread(target = poll_for_CE)
    thread2 = Thread(target = poll_for_updates)
    thread3 = Thread(target = act_on_broadcast)
    thread1.start()
    thread2.start()
    thread3.start()

    threads.append(thread1)
    threads.append(thread2)
    threads.append(thread3)
	
    for thread in threads:       
        thread.join()

    print "thread finished...exiting"
