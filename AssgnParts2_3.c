#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <limits.h>
#include <string.h>
#include <time.h>

//http://www.google.com
//http://bits-pilani.ac.in


int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    char recbuff[1024*60];
    memset(recbuff,0,sizeof(recbuff));
    char* host = argv[1];
    char* req = "GET http://www.google.com HTTP/1.1\r\n"
    "Host: http://www.google.com \r\n"
    "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
    "User-Agent: myapp\r\n"
    "Accept: */*\r\n"
    "Proxy-Connection: close\r\n"
    "\r\n";
    int socksuccess = 0;
    if((socksuccess = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("\n");
        printf("ERROR : SOCKET FAILED");
        printf("\n");
        return 1;
    }
    if(argc<3 || argc>3)
    {
        printf("\n");
        printf("ERROR : INCOMPLETE INFO");
        printf("\n");
        return 2;
    }
    struct hostent *server;
    server = gethostbyname(host);
    if(server==NULL)
    {
        printf("HOST NAME INCORRECT");
        printf("\n");
        return 3;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;   
    serv_addr.sin_port = htons(atoi(argv[2])); 
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    int inetp = inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
    if(inetp<=0)
    {
        printf("\n");
        printf("ERROR : INET_PTON");
        printf("\n");
        return 4;
    }

    int conn = connect(socksuccess, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(conn<0)
    {
        printf("\n");
        printf("ERROR : CONNECT FAILED");
        printf("\n");
        return 5;
    }
    
    int len = strlen(req);
    int track = 0;
    int tobewritten = write(socksuccess,req+track,len-track);
    if (tobewritten < 0)
    {
        printf("ERROR : WRITING REQUEST TO SOCKET");
        return 6;
    }
    track+=tobewritten;
    for(;track < len;)
    {
        tobewritten = write(socksuccess,req+track,len-track);
        if (tobewritten < 0){
            printf("ERROR : WRITING REQUEST TO SOCKET");
            return 6;
        }
        if (tobewritten == 0)
            break;
        track+=tobewritten;
    } 

    printf("REQUEST SENT\n");
    printf("%s\n",req);

    
    FILE *fl = fopen("index.html", "w+");
    int downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    for(;downloadtrack>0;)
    {
        printf("%s", recbuff);
        fputs(recbuff, fl);
        memset(recbuff,0,sizeof(recbuff));
        downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    }
    fclose(fl);
    //Check_if_moved("index.html");
    return 0;

}