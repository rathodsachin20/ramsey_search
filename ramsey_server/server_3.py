'''
    Simple socket server using threads
'''
 
import socket
import sys
from thread import *
import json
import os
import time
import dynamo

clients = {}
updates = { }
ce = { }
clientaddress = {}
cluster_name = ""

# Processing mesg
def process_ce(data, conn):
    global ce
    gsize_bytes = int(data["gsize_bytes"])
    conn.send(json.dumps({"return":"ok"}))
    graph = conn.recv(gsize_bytes)
    conn.send(json.dumps({"return":"ok"})) 
    conn.close()

    hostname = data["host_name"]
    ce[hostname] = { }
    ce[hostname]["from"] = hostname
    ce[hostname]["ce_size"] = data["gsize"]
    ce[hostname]["ce_graph"] = graph

    db = dynamo.DynamoDB()
    putdata = {}
    putdata['clientId'] = hostname
    putdata['gsize'] = str(data['gsize'])
    putdata['graph'] = graph
    db.put_item('graphs', putdata)
    #print updates


def update_client_list(data,conn):
    global clients
    global clientaddress
    global cluster_name
    print data["host_name"], data["ip"], data["port"]
    clients[data["host_name"]] = (data["ip"],int(data["port"]))
    conn.send(json.dumps({"return":"ok"}))

    db = dynamo.DynamoDB()
    putdata = {}
    putdata['clientId'] = data['host_name']
    #putdata['ip'] = data['ip']
    putdata['port'] = str(data['port'])
    putdata['cluster'] = cluster_name  #TODO
    putdata['algo'] = ''     #TODO
    db.put_item('clients', putdata)
    print "Dynamo updated with new client"

def clientthread(conn):
    while True:
        data = conn.recv(4096).strip()
        if not data:
            print "Client ", conn, "has failed"
            break

        if data:
            data = json.loads(data)
            if data["msg_type"] == "counter-example":
                process_ce(data, conn)
            elif data["msg_type"] == "update":
                process_update(data, conn)
            elif data["msg_type"] == "broadcast":
                update_client_list(data, conn)
            break
    exit()

def process_update(data,conn):
    global updates    
    g_size = int(data["gsize_bytes"])		
    print "recieved graph size", g_size
            
    conn.sendall(json.dumps({'return':"ok"}))
    graph = conn.recv(g_size)
    print graph             
    conn.sendall(json.dumps({'return':"ok"}))   
    print "close the connecion"
    conn.close()

    hostname = data["host_name"]
    updates[hostname] = {} 
    updates[hostname]["from"] = hostname
    updates[hostname]["best_count"] = data["best_count"]
    updates[hostname]["current_graph"] = graph
    updates[hostname]["current_gsize"] = data["gsize"]

    db = dynamo.DynamoDB()
    putdata = {}
    putdata['clientId'] = hostname
    putdata['gsize'] = str(data['gsize'])
    putdata['graph'] = graph
    putdata['count'] = str(data['best_count'])
    db.put_item('status', putdata)
     
#    print updates
   
# To DO: to send algorithm that needs to be run on client
def send_broadcast(graph, count):
    global clients
    global clientaddress

    print "send broadcast to", clients
    size = len(graph)
    for host in clientaddress:
        server_address1 = clientaddress[host]
        print "server adress is :",server_address1
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ''' ensures server is listening on the other side -> synchronization'''
        while True:
            try:
                sock.connect((server_address1[0], server_address1[1]))
                break
            except socket.error, msg:
                print "Couldnt connect with the socket-server: %s\n retrying again" % msg
                continue

        payload = { }
        payload["gsize"] = count
        payload["gsize_bytes"] = size
        print "payload", payload
        sock.sendall(json.dumps(payload))
        print "sent broadcast payload"
        data = sock.recv(1024).strip()
        print "beforre",data
        data = json.loads(data)
        print"after data", data
        if data["return"] == "ok":
            print "sending graph pa"
            sock.sendall(graph)
            data = sock.recv(1024).strip()
            data = json.loads(data)
            if data["return"] == "ok":
                print "Graph sent to", host 
        sock.close()


def check_for_broadcast():
    global updates
    global ce
    global clients
    
    max_CE = 0
    min_best_count = 9999
    best_ce_graph = ""
    best_update_graph = ""
    #the graph that needs to be broadcasted
    graph_to_update = ""
    graph_update_size = 0
    while True:
        if not clients:
            continue       
        print "some client has registered"
        for client in clients:
            if ce.has_key(client) == False:
                continue
           # print "ce dict ce,ce var",ce,ce[client]["ce_size"]
            ce_count = ce[client]["ce_size"]
            if ce_count > max_CE:
                max_CE = ce_count
                best_ce_graph = ce[client]["ce_graph"]


        for client in clients:
            if updates.has_key(client) == False:
                continue
            best_count = updates[client]["best_count"]
            if best_count > min_best_count:
                min_best_count = best_count
                best_update_graph = updates[client]["current_graph"]
                graph_update_size = updates[client]["current_gsize"]     
        ''' Need to modify this to suit more clients '''
        if graph_update_size > best_ce_graph:
            graph_to_update = best_update_graph
            send_broadcast(graph_to_update, min_best_count)
        time.sleep(3)
          

def main():
    global cluster_name
    # replace this by sock.gethostname()
    HOST = '0.0.0.0'   # Symbolic name meaning all available interfaces
    PORT = 8888 # Arbitrary non-privileged port

    if len(sys.argv)>1:
        cluster_name = sys.argv[1]
 
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
    graph = {}
    port = 8899
    #now keep talking with the client
    start_new_thread(check_for_broadcast,())
    while 1:
    #wait to accept a connection - blocking call
        conn, addr = s.accept()
        if clientaddress.has_key(addr[0]) == False:
            clientaddress[addr[0]] = (addr[0], port)

        print 'Connected with ' + addr[0] + ':' + str(addr[1])

        start_new_thread(clientthread ,(conn,))
    s.close()

if __name__ == "__main__":
    main()

