/*

Name: Project 2 
Class: CS 372 
Author: Benjamin Fondell
Description: The server side of a network program for implementing FTP.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>

//***************** Function Prototypes **********************
struct addrinfo* getIP(char* addresults, char* port);
int makeSocket(struct addrinfo *IP);
void connectSocket(int sockFd,struct addrinfo * IP);
void socketBind(int sockfd, struct addrinfo * IP);
void socketListen(int sockfd);
void acceptConnection(int sockfd);
void ftpTalk(int newSockFD);
int getFiles(char ** files);


int main(int argc, char *argv[]){
    if(argc != 2)
    {
        fprintf(stderr, "Server says invalid number of arguments.\n");
        exit(1);
    }
    printf("Server open on port %s\n", argv[1]);
    char* nullPointer = NULL;
    struct addrinfo * IP = getIP(nullPointer,argv[1]);
    int sockfd = makeSocket(IP);
    socketBind(sockfd, IP);
    socketListen(sockfd);
    acceptConnection(sockfd);
    freeaddrinfo(IP);
}



// documentation for gathering socket addresults information in c
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530741(v=vs.85).aspx
struct addrinfo* getIP(char* ipAddress, char* port)
{
    struct addrinfo *IP,
    hints;
    int status;
    
    //if ip is not specified 
    if(ipAddress != NULL)
    {
        
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        status = getaddrinfo(ipAddress, port, &hints, &IP);
        if(status != 0)
        {
            printf("Server says invalid IP/Port.\n");
            exit(1);
        }
        
        return IP;
        
    }
    //if ip adress is specified
    else
    {
        
        memset(&hints,0,sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        hints.ai_flags = AI_PASSIVE;
        
        status = getaddrinfo(NULL,port,&hints,&IP);
        if(status != 0)
        {
            printf("Server says get socket address failed.\n");
            exit(1);
        }
        
        return IP;
    }
}

//Define the socket with family, type, protocol
int makeSocket(struct addrinfo *IP)
{
    int sockFd;
    
    sockFd = socket(IP->ai_family,IP->ai_socktype, IP->ai_protocol);
    
    if(sockFd == -1)
    {
        fprintf(stderr,"Creating the socket failed!\n");
        exit(1);
    }
    
    return sockFd;
}

//bind socket to port
void socketBind(int sockfd, struct addrinfo * IP)
{
    if (bind(sockfd, IP->ai_addr, IP->ai_addrlen) == -1) 
    {
        close(sockfd);
        fprintf(stderr, "Error in binding socket\n");
        exit(1);
    }
}

//listen on socket, only accept one connection at a time
void socketListen(int sockfd)
{
    if(listen(sockfd, 1) == -1)
    {
        close(sockfd);
        fprintf(stderr, "Error in listening on socket\n");
        exit(1);
    }
}

// connect socket with client
void connectSocket(int sockFd,struct addrinfo * IP)
{
    int status;
    
    
    status = connect(sockFd,IP->ai_addr,IP->ai_addrlen);
    
    if (status == -1)
    {
        fprintf((stderr), "Connecting socket failed!\n");
        exit(1);
    }
    
}

void acceptConnection(int sockfd){
    
    struct sockaddr_storage clientIP;
    
    socklen_t ipSize;
    
    int newSockFD;
    
    //while a sigint has not been entered
    while(1){
        
        ipSize = sizeof(clientIP);
        
        //accept connection from client
        newSockFD = accept(sockfd, (struct sockaddr *)&clientIP, &ipSize);
        
        if(newSockFD == -1)
        {
            
            return;
        }
        
        //link connection to handle requests for ftp
        ftpTalk(newSockFD);
        //close socket and wait for new connection
        close(newSockFD);
    }
}

//http://www.sanfoundry.com/c-program-list-files-directory/
// Modified program from above source for getting files
// from current directory
int getFiles(char ** files)
{
    DIR * directory;
    struct dirent * Directory;
    directory = opendir(".");
    int fileCount = 0;
    if (directory)
    {
        while ((Directory = readdir(directory)) != NULL)
        {
            if (Directory->d_type == DT_REG)
            {
                //fill files array with filenames
                strcpy(files[fileCount], Directory->d_name);
                fileCount++;
            }
        }
        closedir(directory);
    }
    //printf("This is how many files i counted in getFiles: %i\n",i );
    // return number of files in directory
    return fileCount;
}

//handles the requests for file transfer from the client
// accepts a tcp socket connection as parameter
void ftpTalk(int newSockFD){
    
    //messages for client validation
    char* validPort = "valid port";
    char* invalidCommand = "invalid command";
    char* validCommand = "valid command";
    char* fileNotFound = "no such file in directory";


    int i = 0;
    
    //buffers for sent client arguments
    char portBuffer[100];
    char commandBuffer[100];
    char ipBuffer[100];
    
    //collect the port from the tcp connection
    memset(portBuffer,0,sizeof(portBuffer));
    recv(newSockFD,portBuffer,sizeof(portBuffer) - 1 ,0);
    send(newSockFD,validPort,strlen(validPort) ,0);
    
    //collect the command flag from the tcp connection
    memset(commandBuffer,0,sizeof(commandBuffer));
    recv(newSockFD,commandBuffer,sizeof(commandBuffer) -1 ,0);
    

    //collect the ip address from the tcp connection
    memset(ipBuffer,0,sizeof(ipBuffer));
    recv(newSockFD,ipBuffer,sizeof(ipBuffer) - 1 ,0);
    
    printf("Connection from: %s\n",ipBuffer);
    
    //if client requests a list of files in the server directory
    if(strcmp(commandBuffer,"l") == 0)
    {
        
        //send command validation message
        send(newSockFD, validCommand, strlen(validCommand),0);
        printf("List directory requested on port %s\n", portBuffer);
        printf("Sending directory contents to %s :%s\n", ipBuffer, portBuffer);
        
        //create array of strings for file names
        i = 0;
       	char ** files = malloc(100*sizeof(char *));
       	for(;i < 100; i++)
        {
       		files[i] = malloc(100*sizeof(char));
       		memset(files[i],0,sizeof(files[i]));
       	}
        
        //call getFiles to count files and fill the files array with the
        // collected file names
        int fileCount = getFiles(files);

        sleep(1); //sleep for race condition that was breaking the connection

        //build ftp connection
        struct addrinfo * IP = getIP(ipBuffer, portBuffer);
        int dataSocket = makeSocket(IP);
        connectSocket(dataSocket, IP);
        
        //send files over ftp connection
        int i = 0;
        for (; i < fileCount; i++)
        {
            send(dataSocket, files[i], 100,0);
        }

        //printf(" This is how many files im sending back: %i\n",i);
        
        //end of file list flag for signaling to client the end
        // of the list
        char * eofList = "eof_list";
        send(dataSocket, eofList, strlen(eofList),0);
        
        //free the file array 
        for(i=0;i<100;i++)
            free(files[i]);
        free(files);

        //close the ftp connection and free address object
        close(dataSocket);
        freeaddrinfo(IP);
    }
    //if client requests a file from the server
    else if(strcmp(commandBuffer, "g") == 0)
    {
        //printf("This is in the portBuffer: %s\n",portBuffer);
        //validation for command 
        send(newSockFD, validCommand, strlen(validCommand),0);
        
        //accept the file name the client passed as an argument
        // and sent over the tcp connection
        char fileName[100];
        memset(fileName, 0, sizeof(fileName));
        recv(newSockFD, fileName, sizeof(fileName)-1,0);
        printf("File %s requested on port %s\n", fileName, portBuffer);

        //create array of string to hold file list
        i = 0;
       	char ** files = malloc(100*sizeof(char *));
       	for(;i < 100; i++)
        {
       		files[i] = malloc(100*sizeof(char));
       		memset(files[i],0,sizeof(files[i]));
       	}

        //collect files and count files
        int fileCount = getFiles(files);

        // search for file in list
        // set fileFound flag if found
        int fileFound = 0;
        i = 0;
        for (; i < fileCount; i++)
        {
            if(strcmp(files[i], fileName) == 0){
                fileFound = 1;
            }
        }
        // If client requests a file that is in the directory
        // send the file to the client
        if(fileFound == 1)
        {
            
            //File was found, validate found file in the directory to client
            printf("Sending %s to %s: %s\n", fileName,ipBuffer,portBuffer);
            char * foundMsg = "found";
            send(newSockFD, foundMsg, strlen(foundMsg),0);
         
            //build command to open file
            // http://www.cplusplus.com/reference/cstdio/sprintf/
            char newFile[100];
            memset(newFile,0,sizeof(newFile));
            strcpy(newFile, "./");
            char * commandLine = newFile + strlen(newFile);
            commandLine = commandLine + sprintf(commandLine, "%s", fileName);

            sleep(1); //sleep for race condition that was breaking the connection

            //build ftp connection
            struct addrinfo * IP = getIP(ipBuffer, portBuffer);
            int dataSocket = makeSocket(IP);
            connectSocket(dataSocket, IP);
            
            //hold read file in temp buffer
            char tempBuffer[1000];
            memset(tempBuffer, 0, sizeof(tempBuffer));
            
            //open and read file 
            int file = open(fileName, O_RDONLY);
            while (1) {
                
                int fileRead = read(file, tempBuffer, sizeof(tempBuffer)-1);
                //if file is empty
                if (fileRead < 0)
                {
                    printf("Error reading the file.\n");
                    return;
                }

                //If the file has finished being read
                // break the loop
                if(fileRead == 0)
                {
                    break;
                }

                //buffer for the contents of the file to be sent
                void * fileContents = tempBuffer;
                while (fileRead > 0) 
                {
                    //send file contents to client
                    int fileWritten = send(dataSocket, fileContents, sizeof(tempBuffer),0);
                    if (fileWritten < 0) 
                    {
                        fprintf(stderr, "Error writing to socket\n");
                        return;
                    }
                    //Initializes buffers to new sizes  
                    // breaks the file into sections based on how 
                    // much the client will accept
                    // See section 6.3 in BEEJ's guide to network programming
                    // for handling partial sends
                    fileRead = fileRead - fileWritten;
                    fileContents = fileContents + fileWritten;
                }
                
                //clear buffer
                memset(tempBuffer, 0, sizeof(tempBuffer));
            }
            
            //concatenate a eof flag for the client
            // to find the end of file
            memset(tempBuffer, 0, sizeof(tempBuffer));
            strcpy(tempBuffer, "__eof__");
            send(dataSocket, tempBuffer, sizeof(tempBuffer),0);
            
            //close connection
            close(dataSocket);
            freeaddrinfo(IP);
        }
        // if no file was found log debug script
        else
        {
            
            printf("File not found, sending error message to %s: %s\n",ipBuffer,portBuffer);
            send(newSockFD, fileNotFound, strlen(fileNotFound), 0);

        }
       
        //free files array
        for(i=0;i<100;i++)
            free(files[i]);
        free(files);
    }
    //if command flag was invalid
    else
    {   
        send(newSockFD, invalidCommand, strlen(invalidCommand), 0);
        printf("Invalid command sent\n");
    }
    printf("Waiting for connection...\n");
}
