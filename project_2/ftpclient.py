#!/bin/python

#
#
# Name: Project 2 
# Class: CS 372 
# Author: Benjamin Fondell
# Description: The client side of a network program for implementing FTP.
#
#

from socket import *
import sys

# Validate arguments entered on the command line 
# useage: ./ftpclient.py <host name> <server port> < -l (list files)> <data port> 
#  OR
# useage: ./ftpclient.py <host name> <server port> < -g (get file)> <file name> <data port> 
#
def  argumentValidator():
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print "Invalid number of arguments."
		exit(1)
	elif(sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):
		print "Invalid server host name. Please specify flip1, flip2, or flip3"
		exit(1)
	elif(int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535
		 or int(sys.argv[2]) == 30021 or int(sys.argv[2]) == 30020):
		print "Invalid port."
		exit(1)
	elif(sys.argv[3] != "-l" and sys.argv[3] != "-g"):
		print "Invalid option flag. Useage: -l (list files) -g (get file)"
		exit(1)
	elif(sys.argv[3] == "-l"):
		if(int(sys.argv[4]) < 1024 or int(sys.argv[4]) > 65535
		 or int(sys.argv[4]) == 30021 or int(sys.argv[4]) == 30020):
			print "Invalid data port."
			exit(1)
		pass
	elif(sys.argv[3] == "-g"):
		if(int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535
		 or int(sys.argv[5]) == 30021 or int(sys.argv[5]) == 30020):
			print "Invalid data port."
			exit(1)
		pass

#Create data socket for ftp
# if -l is passed then the data port
# is arg 4 else arg 5
def createDataSocket():
	if sys.argv[3] == "-l":
		portIndex = 4
	else:
		portIndex = 5
	ftpserver_port = int(sys.argv[portIndex])
	ftpserver_socket = socket(AF_INET,SOCK_STREAM)
	ftpserver_socket.bind(('',ftpserver_port))
	ftpserver_socket.listen(1)
	ftpserver_dataSocket, address = ftpserver_socket.accept()
	return ftpserver_dataSocket

# Connect to the server with
# TCP socket connection
def connectTCPServer():
	ftpserver_name = sys.argv[1] + ".engr.oregonstate.edu"
	ftpserver_port = int(sys.argv[2])
	ftpclient_socket = socket(AF_INET,SOCK_STREAM)
	ftpclient_socket.connect((ftpserver_name,ftpserver_port))
	return ftpclient_socket

# Receive data from the data connection
def getData(dataSocket):
	if sys.argv[3] == "-l":
		fileIndex = dataSocket.recv(100)
		while fileIndex != "eof_list":
			print fileIndex
			fileIndex = dataSocket.recv(100)
		return
	else:
		inputBuffer = dataSocket.recv(1000)
		newFile = open(sys.argv[4],'w')
		while "_eof_" not in inputBuffer:
			newFile.write(inputBuffer)
			inputBuffer = dataSocket.recv(1000)

# Send all necessary commands over
# TCP connection to determine the data requested
def ftpTalk(clientSocket):
	option = sys.argv[3]
	if option == "-l":
		print"Client is requesting file list."
		portIndex = 4
	else:
		print"client is requesting file: {}".format(sys.argv[4])
		portIndex = 5

	#Handshake
	clientSocket.send(sys.argv[portIndex])
	clientSocket.recv(1024)

	#Send command flag
	if option == "-l":
		clientSocket.send("l")
	else:
		clientSocket.send("g")

	# Solution for getting the ip address of the client pulled from 
	# stack overflow at the following link
	# https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib/25850698#25850698
	s = socket(AF_INET, SOCK_DGRAM)
	s.connect(("8.8.8.8", 80))
	ip = s.getsockname()[0]

	# send ip address
	clientSocket.send(ip)
	response = clientSocket.recv(1024)
	if response[:7] == "invalid":
		print"Server says invalid command."
		exit(1)

	# Send file name requested with -g flag
	if option == "-g":
		clientSocket.send(sys.argv[4])
		response = clientSocket.recv(1024)
		if response != "found":
			print"Server says it could not find the file."
			return

	# create the data socket to recieve from server
	dataSocket = createDataSocket()
	# get the data from the server
	getData(dataSocket)
	# close the data socket connection
	dataSocket.close()

# This is my preffered method for handling main in python sourced from below
#https://stackoverflow.com/questions/4041238/why-use-def-main
if __name__ == "__main__":
	#print "im in here!"
	#validate passed command line arguments
	argumentValidator()
	#connect the tcp socket
	clientSocket = connectTCPServer()
	#start talking with the server
	ftpTalk(clientSocket)































