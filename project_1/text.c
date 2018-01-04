#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

void getUserName(char *input)
{
    printf("Please enter a chat username: ");
    scanf("%s",input);
}

//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530741(v=vs.85).aspx
struct addrinfo* create_address(char* address, char* port)
{
    struct addrinfo *result,
    hints;
    
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    
    int iResult;
    
    iResult = getaddrinfo(address,port,&hints,&result);
    
    if(iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        exit(1);
    }
    
    return result;
}

int create_socket(struct addrinfo *result)
{
    int sockFd;
    if((sockFd = socket(result->ai_family,result->ai_socktype, result->ai_protocol) == -1))
    {
        fprintf(stderr,"Creating the socket failed!\n");
        exit(1);
    }
    
    return sockFd;
}

void connect_socket(int sockFd,struct addrinfo *result)
{
    int status;
    
    
    if ((status = connect(sockFd,result->ai_addr,result->ai_addrlen) == -1))
    {
        fprintf((stderr), "Connecting socket failed!\n");
        exit(1);
    }
    
}

void handshake(int sockFd, char* username, char* servername)
{
    int sending = send(sockFd,username,strlen(username),0);
    int recieving = recv(sockFd,servername,10,0);
}

void chat(int sockfd, char * username, char * servername){
    
    char input[503];
    char output[501];
    
    memset(input,0,sizeof(input));
    memset(output,0,sizeof(output));
    
    int num_bytes_sent = 0;
    int status;
    
    fgets(input, 500, stdin);
    while(1){
        
        printf("%s> ", username);
        fgets(input, 502, stdin);
        
        if (strcmp(input, "\\quit\n") == 0){
            break;
        }
        
        num_bytes_sent = send(sockfd, input, strlen(input) ,0);
        
        if(num_bytes_sent == -1){
            fprintf(stderr, "Error when sendind data to host\n");
            exit(1);
        }
        status = recv(sockfd, output, 500, 0);
        if (status == -1){
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
            printf("%s> %s\n", servername, output);
        }
        
        memset(input,0,sizeof(input));
        memset(output,0,sizeof(output));
    }
    
    close(sockfd);
    printf("Closed Connection\n");
}

int main(int argc, char *argv[]){
    
    if(argc != 3){
        fprintf(stderr, "Invalid number of arguments\n");
        exit(1);
    }
    
    char username[10];
    getUserName(username);
    
    struct addrinfo * res = create_address(argv[1], argv[2]);
    
    int sockfd = create_socket(res);
    
    connect_socket(sockfd, res);
    
    char servername[10];
    handshake(sockfd, username, servername);
    
    chat(sockfd, username, servername);
    
    freeaddrinfo(res);
}
