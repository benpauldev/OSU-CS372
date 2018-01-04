
CS 372
Project 2 
Author: Benjamin Fondell


******** Compile and execute ftpserver.c ********


 compile>  $ gcc ftpserver.c

 execute>  $ ./a.out <port number>



******** Compile and execute client.py ********

 compile>  $ chmod +x ftpclient.py


*** To return a list files in the server program's directory ***

 execute>  $ ./ftpclient.py <host name (flip1, flip2, flip3)> <server port> < -l (list files)> <data port> 

** OR **

*** To return a file from server directory and place it in client program's directory ***

 execute>  $ ./ftpclient.py <host name (flip1, flip2, flip3)> <server port> < -g (get file)> <file name> <data port> 


NOTES: 
		> ftpserver.c and ftpclient.py must both be in seperate directories.
		> The server directory should have two example txt files for testing. 
			one short.txt and one long.txt. 
		> The file also must be on two seperate flip server instances (flip1, flip2, flip3)




