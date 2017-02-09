'''
Program: chatserver.py
Assignment #1 
cs_372 Winter_2017
Erin Rosenbaum
'''

from socket import *

# so that sys.argv can be used
import sys

# specifies socket is reachable
host = ''

serverPort = int(sys.argv[1])
#print("Argument List:", str(sys.argv))
handle = "your_server> "
# TCP uses SOCK_STREAM

serverSocket = socket(AF_INET,SOCK_STREAM)

# bind the socket to the assigned port
serverSocket.bind(('', serverPort))


# create an infinite loop to listen indefinitely
#***********************************************
while 1:
    # create INET, streaming socket
    # listen for 5 connections, this is normal max
    serverSocket.listen(5)

    # display host name
    print("\nHostname: " + gethostname())
    print ("The server is listening on port number " + str(serverPort) + " ...")

    # accept connections
    connectionSocket, addr = serverSocket.accept()
    print 'Connected to ', addr

    while 1:
    
        buffer = connectionSocket.recv(1000)
        if not buffer: break  
        print buffer

        # get input from the keyboard, store in message variable,
        # and prompt the user with the server's handle
        message = raw_input(handle)

        # truncated message to 500 characters if it's longer than 500
        message = (message[:500]) if len(message) > 500 else message

        # quit program if user inputs quit command
        if message == "\quit" :
            connectionSocket.close()
            exit()
 
        connectionSocket.send(handle + message)

# closes this particular socket, but still listens for additional client connections
# connectionSocket.close()