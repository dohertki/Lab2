#!/usr/bin/python

"""
# Python code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error, msg:
        print 'Failed to create. socket. Error code: ' + str(msg[0])   
        sys.exit()

    print 'Socket Created'

"""

""" FTP Client
"""
import socket
import sys
import time
import threading


user_entry={'ftserver': None, 'port': None, 'request_flag': None, 
'filename': None, 'data_port': None}


def maxc():
    print 'hello'
    
    if  len(sys.argv) == 5:
             
        user_entry['ftserver'] = sys.argv[1]
        user_entry['port'] = int(sys.argv[2])
        user_entry['request_flag'] = sys.argv[3]
        user_entry['data_port'] = int(sys.argv[4])
        
        
        
        print "5 agrs" + str(len(sys.argv))
    
    
        print 'Port:' + str(user_entry['port']) 
    elif len(sys.argv) == 6:
    
        
        
        print "6 agrs" + str(len(sys.argv))
     
    else:
        print 'Usage: ./clientserv.py [ftserver name] [sever port#] [-l or -g filename] [local port#]'
        exit()

    return user_entry

def someargshit():
    print(sys.argv)
    if sys.argv.index("-l"):
        print("list directory flag")



def setSocket():

# Python code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error, msg:
        print 'Failed to create. socket. Error code: ' + str(msg[0])   
        sys.exit()

    print 'Socket Created'
    return s

def main():
    
    

    maxc()

    s = setSocket()   
   
   #chicken shit VV
    host = ''

    s.connect(('',user_entry['port']))
    #change message its off page user_entry.get('request_flag') +
    
    message ='g ' + str(user_entry.get('data_port')) + ' p00p' + ' myfile'
    
    print(message)
    
    time.sleep(1)

    #send the header message
    try:
        s.sendall(message)
    except socket.error:    
        #send failed
        print 'Send failed'
        sys.exit()

    print 'Much success'



    #Now receive data
    reply = s.recv(4096)
 
    print reply


# File IO code 
def readFile():

    myfile = open("gulliver.txt","r")
    small_buffer = myfile.read()
#    print(small_buffer)
    myfile.close()



'''
try:
    thread.start_new_thread( readFile,())
except:
    print("Error, thread failed to start")
  
'''
t1 = threading.Thread(target = readFile)

t1.start()

t1.join()










if __name__ == '__main__':
    main()

"""
 conn, addr = s.accept()
            print 'Connected with Server ' + addr[0] + ' : ' + str(addr[1])

n code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error, msg:
    print 'Failed to create. socket. Error code: ' + str(msg[0])   
    sys.exit()
print 'Socket Created'

s.setblocking(0)
HOST = ' '
PORT = 8888

try:
	s.bind((HOST, PORT))
except socket.error, msg:
	print 'Bind failed. Error Code: ' + str(msg[0]) + ' Message ' + msg[1]
	sys.exit()



s.listen(10) #Parameter backlog...the number of incoming connection kept waiting

print 'Socket listening'

#wait to accept a connection - blocking call
conn, addr = s.accept()
 
#display client information
print 'Connected with ' + addr[0] + ':' + str(addr[1])Message(conn) 



######################################################################### 
#                             readMessage(conn)                         # 
# Use: Function recieves messages to clients connected to the server    #
# Input: conn: Pointer to a socket connection                           #
# Output: function prints messages to the screen                        #
#########################################################################

def readMessage(conn):
    data = conn.recv(2056)

    print data

    

#########################################################################
#                                sendMessage()                          #
# Use: Function gets message from the user and send it to the socket    # 
#      connection                                                       #
# Input: conn: Point to a socket connection                             #
# Output: none.                                                         #
#########################################################################

def sendMessage(conn):
    msg = raw_input("Server> ")
    if msg == '\quit' :
        print 'Server quitting...'
        return 0 
    else:
        msg = S + msg
        conn.send(msg)
        return 1
"""    
