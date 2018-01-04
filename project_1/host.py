#!/bin/python

from socket import *
import sys


def chat(connectionSock,clientname,username):
	
	while 1:
		#recieve message from client
		received_msg = connectionSock.recv(501)[0:-1]
		if received_msg == "":
			print "connection closed, standby for connection"
			return
		#attach handle and print the recieved message
		print "{}> {}".format(clientname,received_msg)

		#Send message back from host
		sending_msg = ""
		while len(sending_msg) == 0 or len(sending_msg) > 500:
			sending_msg = raw_input("{}> ".format(username))
			if len(sending_msg) == 0:
				print "That message is empty."
			if len(sending_msg) > 500:
				print "That message is too long."

		# If \quit is entered the connection will close and wait for another
		if sending_msg == "\quit":
			print "connection closed, standby for connection"
			return
		connectionSock.send(sending_msg)

#https://stackoverflow.com/questions/4041238/why-use-def-main
if __name__ == "__main__":
	
	# accept port number from command line
	portnum = sys.argv[1]

	#declare socket
	serverSocket = socket(AF_INET, SOCK_STREAM)
	#bind socket to port
	serverSocket.bind(('',int(portnum)))
	# listen on socket for up to 5 connections
	serverSocket.listen(5)
	#print "The server is ready to recieve."
	
	#get the username for the host
	username = ""	
	while len(username) < 1 or len(username) > 10:
		username = raw_input("Please enter username: ")
		if len(username) < 1 or len(username) > 10:
			print "Please enter a username less than 10 charachters."
	print "Waiting for client..."
	# accept connection at socket
	while 1:
		connectionSock,addr = serverSocket.accept()
		print "Started connection at address: {} port: {}.".format(addr,portnum)
		
		#Handshake for tcp connection{
		clientname = connectionSock.recv(1024)
		connectionSock.send(username)
		# }


		#begin chat with client
		chat(connectionSock,clientname,username)
		#close the connection if chat is quit or ctrl-c is entered
		connectionSock.close()