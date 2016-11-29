#!/usr/bin/python
#-*- coding: utf-8 -*-

"""
# Python code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/



"""

###########################################################################
""" FTP Client
"""
import socket
import sys
import time
import threading
import pickle

user_entry={'ftserver': None, 'port': None, 'request_flag': None, 
'filename': None, 'hostname': None, 'data_port': None}


########################################################################### 
########################################################################### 
def maxc():
    print 'hello'
    
    if  len(sys.argv) == 5:
             
        user_entry['ftserver'] = sys.argv[1]
        user_entry['port'] = int(sys.argv[2])
        user_entry['request_flag'] = sys.argv[3]
        user_entry['data_port'] = int(sys.argv[4])
        user_entry['hostname'] = socket.gethostname() 
        
        print "5 agrs" + str(len(sys.argv))
        print 'Port:' + str(user_entry['port']) 
        print 'Data port: ' + str(user_entry['data_port'])
    elif len(sys.argv) == 6:
    
        user_entry['ftserver'] = sys.argv[1]
        user_entry['port'] = int(sys.argv[2])
        user_entry['request_flag'] = sys.argv[3]
        user_entry['data_port'] = int(sys.argv[5])
        user_entry['hostname'] = socket.gethostname() 
        user_entry['filename'] =  sys.argv[4]        
        print "6 agrs" + str(len(sys.argv))
     
    else:
        print 'Usage: ./clientserv.py [ftserver name] [sever port#] [-l or -g filename] [local port#]'
        exit()

    return user_entry


########################################################################### 
#                            readMessage(conn)                            # 
# Use: Function recieves messages to clients connected to the server      #
# Input: conn: Pointer to a socket connection                             #
# Output: function prints messages to the screen                          #
###########################################################################

def readMessage(conn):
    data = conn.recv(2056)

    print data

    

############################################################################
#                              sendMessage()                               #
# Use: Function gets message from the user and send it to the socket       # 
#      connection                                                          #
# Input: conn: Point to a socket connection                                #
# Output: none.                                                            #
############################################################################

def sendMessage(conn):
    msg = raw_input("Server> ")
    if msg == '\quit' :
        print 'Server quitting...'
        return 0 
    else:
        msg = S + msg
        conn.send(msg)
        return 1
    
###########################################################################
#                               setSocket()                               #
# Python code for the communicating with network                          #
# socket is based on network programming tutorial found at:               # 
# http://www.binarytides.com/python-socket-programming-tutorial/          #
###########################################################################
def setSocket():

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error, msg:
        print 'Failed to create. socket. Error code: ' + str(msg[0])   
        sys.exit()

    print 'Socket Created'
    return s

###########################################################################
# Python code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/
###########################################################################
def setServer(clnt):
    try:
        sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error, msg:
        print 'Failed to create. socket. Error code: ' + str(msg[0])   
        sys.exit()
    print 'Socket Created'

    #sd.setblocking(0)
    HOST = ' '
    PORT = clnt['data_port']
    print 'Data port: ' + str(PORT)
    try:
	    sd.bind(('', user_entry['data_port']))
    except socket.error, msg:
	    print 'Bind failed. Error Code: ' + str(msg[0]) + ' Message ' + msg[1]
	    sys.exit()

    sd.listen(5) #Parameter backlog...the number of incoming connection kept waiting

    print 'Socket listening'
    
    if sd is None:
        print ' Socket is not open'
        sys.exit(1)
   
    conn, addr = sd.accept()
    return conn 
    
###########################################################################


# #http://www.binarytides.com/receive-full-data-with-the-recv-socket-
#       function-in-python/
###########################################################################
def fileX(sockfd):        

    #display client information
   #print 'Connected with ' + addr[0] + ':' + str(addr[1]) 
       
    file_buffer = '';
    reply = '';
    i = 0
    
    
    time_limit = 2
    set_timer = time.time()
    print 'set time' + str(set_timer)
    sockfd.setblocking(0)
  
    while 1:
        if file_buffer and time.time()-set_timer > time_limit:
            print 'File has timed out'
            break
        elif time.time() - set_timer > time_limit * 2:
            print 'File has timed out'
            break
            
        try:
            i + 1
            reply = sockfd.recv(4096)
            print '*',           
            if reply:
                print 'data in buffer' +str(time.time()) + ' ' + str(i)
                file_buffer += reply
                set_timer = time.time()
            else:
                print 'sleep'  + str(time.time())
                time.sleep(0.1)
        except:
            print 
            pass
            
    
    fp = open('(copy)' + user_entry['filename'], "w")
    
    print 'File of ' + str(file_buffer) + ' bytes received'
    print 'Opening File: ' + '(copy)' + user_entry['filename'] 
    
    fp.write(file_buffer)    
    fp.close()

    #Send some data to remote server
    message = "Have a happy day"
    try :
        #Set the whole string
        sockfd.sendall(message)
    except socket.error:
        #Send failed
        print 'Send failed'
        sys.exit()

###########################################################################
###########################################################################
def buildHeader():

    if '-l' in user_entry['request_flag']:
        message = user_entry.get('request_flag') + ' '  +  \
                  str(user_entry.get('data_port')) + ' ' + \
                  user_entry.get('hostname') +' NOFILE'
    
    elif '-g' in user_entry['request_flag']:
        message = user_entry.get('request_flag') + ' '  +  \
                  str(user_entry.get('data_port')) + ' ' + \
                  user_entry.get('hostname') +' ' + \
                  user_entry.get('filename')    
    else:
        print ' -g of -l not found '
        exit(1)
    
    print(message)
    return message

###########################################################################
def main():

    maxc()

    
    s = setSocket()   
   
    s.connect((user_entry['ftserver'], user_entry['port']))

    
    #send the header message
    header = buildHeader()
    
    try:
        s.sendall(header)
    except socket.error:    
        #send failed
        print 'Send failed'
        sys.exit()

    print 'Much success'

    reply = s.recv(4096)
    
    #Now receive data
    if '-g' in user_entry['request_flag']:
        print 'Retrieving File'
        conn = setServer(user_entry)
        fileX(conn)        

        #readMessage(conn)



    if '-l' in user_entry['request_flag']:
        print "Receiving directory"
        conn = setServer(user_entry)
        readMessage(conn)
    
    
    
    conn.close()
    







# File IO code 
def readFile():

    myfile = open("gulliver.txt","r")
    small_buffer = myfile.read()
#    print(small_buffer)
    myfile.close()




if __name__ == '__main__':
    main()


