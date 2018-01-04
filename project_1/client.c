#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

//***************** Function Prototypes **********************
struct addrinfo* getAddresults(char* addresults, char* port);
int makeSocket(struct addrinfo *result);
void connectSocket(int sockFd,struct addrinfo * result);
void chat(int sockFd, char * username, char * servername);

//***************** Main **********************
int main(int argc, char *argv[])
{
    //accept address and port from the command line
    if(argc != 3)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        printf("Usage: ./client {Host IP Addresults} {Port}\n");
        exit(1);
    }
    
    char username[10];
    //get username of client and error check for length < 10
    int tooLong = 0;
    do
    {
        tooLong = 0;
        printf("Please enter a chat username: ");
        scanf("%s",username);
        
        if(strlen(username) > 10)
        {
            printf("Please enter a username less than 10 charachters.\n");
            tooLong = 1;
        }
        
    }while(tooLong == 1);
    
    //get socket address
    struct addrinfo * result = getAddresults(argv[1], argv[2]);
    
    //make socket
    int sockFd = makeSocket(result);
    
    //connect the socket
    connectSocket(sockFd, result);
    
    //buffer for collecting server name
    char servername[10];
    
    //handshake for tcp connection
    send(sockFd,username,strlen(username),0);
    recv(sockFd,servername,10,0);
    
    //begin chat over socket connection
    chat(sockFd, username, servername);
    
    //clears address struct after session ends, avoids memory leak
    freeaddrinfo(result);
    return(0);
}


//***************** Functions **********************

// documentation for gathering socket address information in c
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530741(v=vs.85).aspx
struct addrinfo* getAddresults(char* addresults, char* port)
{
    struct addrinfo *result,
    hints;
    
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    
    int status;
    
    status = getaddrinfo(addresults,port,&hints,&result);
    if(status != 0)
    {
        printf("Get socket address failed!\n");
        exit(1);
    }
    
    return result;
}

int makeSocket(struct addrinfo *result)
{
    int sockFd;
    
    sockFd = socket(result->ai_family,result->ai_socktype, result->ai_protocol);
    
    if(sockFd == -1)
    {
        fprintf(stderr,"Creating the socket failed!\n");
        exit(1);
    }
    
    return sockFd;
}

void connectSocket(int sockFd,struct addrinfo * result)
{
    int status;
    
    //printf("%s\n",result->ai_addr );
    
    status = connect(sockFd,result->ai_addr,result->ai_addrlen);
    
    if (status == -1)
    {
        fprintf((stderr), "Connecting socket failed!\n");
        exit(1);
    }
    
}

void chat(int sockFd, char * username, char * servername)
{
    char recieved[503];
    char sent[501];
    int sizeOfSent = 0;
    int status;
    
    fgets(recieved, 500, stdin);
    do{
        
        printf("%s> ", username);
        fgets(recieved, 502, stdin);
        
        if (strcmp(recieved, "\\quit\n") == 0)
        {
            break;
        }
        
        sizeOfSent = send(sockFd, recieved, strlen(recieved) ,0);
        
        if(sizeOfSent == -1)
        {
            fprintf(stderr, "Error when sendind data to host\n");
            exit(1);
        }
        status = recv(sockFd, sent, 500, 0);
        if (status == -1)
        {
            fprintf(stderr, "Error when receiving data from host\n");
            exit(1);
        }
        else if (status == 0)
        {
            printf("Connection closed by server\n");
            break;
        }
        else
        {
            printf("%s> %s\n", servername, sent);
        }
        
        //clears chat in and out buffers, avoids memory leaks
        memset(recieved,0,sizeof(recieved));
        memset(sent,0,sizeof(sent));
        
    }while(1);
    
    memset(recieved,0,sizeof(recieved));
    memset(sent,0,sizeof(sent));
    
    close(sockFd);
    printf("Socket connection closed.\n");
}


