#!/usr/bin/python


""" FTP Client
"""
import socket
import sys


def maxc():
    print 'hello'
if  len(sys.argv) == 2:
    PORT = int(sys.argv[1])
    print 'Port:' + str(PORT) 
else:
    print 'Missing argument'
    print 'Usage: ./clientserv.py [port#]'
    exit()



#if  len(sys.argv) == 2:
#maxc
# Python code for the communicating with network
# socket is based on network programming tutorial found at:
# http://www.binarytides.com/python-socket-programming-tutorial/

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error, msg:
    print 'Failed to create. socket. Error code: ' + str(msg[0])   
    sys.exit()

print 'Socket Created'
PORT = int(sys.argv[1])
print 'Port:' + str(PORT) 


#chicken shit VV
host = ''

s.connect(('',PORT))
#change message its off page
message = "luckysmokes 23000 4 myfile.txt"

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

myfile = open("gulliver.txt","r")

small_buffer = myfile.read()

#print(small_buffer)

myfile.close()


maxc()


