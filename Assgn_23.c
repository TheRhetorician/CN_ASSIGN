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

void redirect(char* flc);
void findredirect(char* flc);

char redlink[1024]={'\0'};
//char host[1024];
char* ip;
char* port;
char* login;
char* password;
char* flc;
char* logofl;
char req[1024];
//char* req;
//http://www.google.com
//http://bits-pilani.ac.in


int reqprocess(char* host,char fconn[1024])
{
    //memset(redlink, '\0', sizeof(redlink));
    int c = 0;
    int cumm = 0;
    memset(req, '\0', sizeof(req));
    //int x = strlen(host);
    //host[x] = '\0';
    //c = snprintf ( req, 1024*60, "GET %s HTTP/1.1\r\n", host);
    //printf("%s\n",req);
    //cumm += c;
    //c = snprintf ( req+cumm, 1024*60 - cumm, "Host: %s \r\n", host);
    //cumm += c;
    //c = snprintf ( req+cumm, 1024*60 -cumm, "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n");
    //cumm += c;
    //c = snprintf ( req+cumm, 1024*60 -cumm, "User-Agent: myapp\r\n");
    //cumm += c;
    //c = snprintf ( req+cumm, 1024*60 -cumm, "Accept: */*\r\n");
    //cumm += c;
    //c = snprintf ( req+cumm, 1024*60 -cumm, "Proxy-Connection: close\r\n");
    //cumm += c;
    //c = snprintf(req+cumm,1024*60-cumm,"\r\n");
    //printf("%s",req);
    printf("%s\n",host);
    //fflush(stdout);
    fflush(stdin);
    fflush(stdout);
    if(fconn[0]=='\0')
    {
        char* htc = "http://";
        if(strstr(host,"http://")!=NULL)
        {
            sprintf(req,"GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",host,host);
        }
        else
        {
            sprintf(req,"GET %s%s HTTP/1.1\r\n"
            "Host: %s%s\r\n"
            "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",htc,host,htc,host);
        }
        
        
    }
    else
    {
        strcpy(req,fconn);
    }
    
    //printf("GET%sHTTP/1.1\r\nHost:%s\r\nProxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\nUser-Agent: myapp\r\nAccept: */*\r\nProxy-Connection: close\r\n\r\n",host,host);
    //sprintf(req,"HTTP/1.1\r\nHost: %s\r\nProxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\nUser-Agent: myapp\r\nAccept: */*\r\nProxy-Connection: close\r\n\r\n",host);
    /*char trial[] = "GET http://";
    strcat(trial,host);
    printf("%s\n",trial);
    strcat(trial," ");
    printf("%s\n",trial);
    strcat(trial,req);
    printf("%s\n",trial);
    memset(req,'\0',sizeof(req));
    strcpy(req,trial);*/
    //char* gett = "GET ";
    //strcat(req,gett);
    //char* ht = " HTTP/1.1\r\nHost: ";
    //strcat(req,host);
    //strcat(req,ht);
    //strcat(req,host);
    //char* aage = " \r\nProxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\nUser-Agent: myapp\r\nAccept: */*\r\nProxy-Connection: close\r\n\r\n";
    //strcat(req,aage);
    //printf("%s",req);
    //printf("%d\n",strlen(req));
    struct sockaddr_in serv_addr;
    char recbuff[1024*60];
    memset(recbuff,0,sizeof(recbuff));
    //char* host = ip;

    int socksuccess = 0;
    if((socksuccess = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("\n");
        printf("ERROR : SOCKET FAILED");
        printf("\n");
        return 1;
    }

    struct hostent *server;
    server = gethostbyname(ip);
    if(server==NULL)
    {
        printf("HOST NAME INCORRECT");
        printf("\n");
        return 3;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;   
    serv_addr.sin_port = htons(atoi(port)); 
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    int inetp = inet_pton(AF_INET, ip, &serv_addr.sin_addr);
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

    FILE *fl = fopen(flc, "w+");
    int downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    for(;downloadtrack>0;)
    {
        printf("%s", recbuff);
        fputs(recbuff, fl);
        memset(recbuff,0,sizeof(recbuff));
        downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    }
    printf("\n");
    fclose(fl);
    close(socksuccess);
    redirect(flc);
    /*if(redlink[0]=='\0')
    printf("NULL MILA");
    else
    {
        memset(host, '\0', sizeof(host));
        strcpy(host,redlink);
        printf("REDIRECTING\n");
        int x  = strlen(redlink);
        while(x-1>=0)
        {

        }
        reqprocess();
    }*/

}

int main(int argc, char *argv[])
{
    
    
    //char* req = "GET http://bits-pilani.ac.in HTTP/1.1\r\n"
    //"Host: http://bits-pilani.ac.in \r\n"
    //"Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
    //"User-Agent: myapp\r\n"
    //"Accept: */*\r\n"
    //"Proxy-Connection: close\r\n"
    //"\r\n";
    
    if(argc<8)
    {
        printf("\n");
        printf("ERROR : INCOMPLETE INFO");
        printf("\n");
        return 2;
    }
    
    //strcpy(host,argv[1]);
    ip = argv[2];
    port = argv[3];
    login = argv[4];
    password = argv[5];
    flc = argv[6];
    logofl = argv[7];

    char fconn[1024];

    fflush(stdin);
    fflush(stdout);
    char* htc = "http://";
    if(strstr(argv[1],"http://")!=NULL)
    {
        sprintf(fconn,"GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
        "User-Agent: myapp\r\n"
        "Accept: */*\r\n"
        "Proxy-Connection: close\r\n"
        "\r\n",argv[1],argv[1]);
    }
    else
    {
        sprintf(fconn,"GET %s%s HTTP/1.1\r\n"
        "Host: %s%s\r\n"
        "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n"
        "User-Agent: myapp\r\n"
        "Accept: */*\r\n"
        "Proxy-Connection: close\r\n"
        "\r\n",htc,argv[1],htc,argv[1]);
    }
    fflush(stdin);
    reqprocess(argv[1],fconn);
    
    

    
    

}

void redirect(char* flc)
{
    FILE* fl = fopen(flc,"r");
    char c;
    char str[4]={'0'};
    int check,counter=0;
    while((c=fgetc(fl))!=EOF){
        if(c==' ')
            check++;
        if(check==1)
        {
            do
            {
                c=fgetc(fl);
                str[counter++] = c;
            } while (c!=EOF && c!=' ');
            break;
            
        }
    }
    str[4]='\0';
    printf("\nCode is %d\n", atoi(str));
    if(atoi(str)==303 || atoi(str)==301)
    {
        printf("Resource has been moved permanently\nSending req to given moved location\n");
        fclose(fl);
        findredirect(flc);
        //return &redlink;
    }
    //return NULL;
}

void findredirect(char* flc)
{
    FILE* fl = fopen(flc,"r");
    char loc[] = "Location: ";
    int offset = strlen(loc);
    char* pos;
    char buff[1024*50];
    //char* redlink;
    memset(redlink,'\0',sizeof(redlink));
    int counter=0;
    while(fgets(buff,1024*60,fl) != NULL)
    {
        pos = strstr(buff,loc);
        if(pos != NULL)
        {
            pos+=offset;
            while(*pos!=' ' && *pos!='\n' && *pos!='\r')
            {
                redlink[counter++] = *pos;
                //strncat(redlink, pos, 1);
                pos++;
            }
            redlink[counter++] = '\0';
            printf("%s\n",redlink);
            printf("%d\n",strlen(redlink));
            fflush(stdout);
            char* ptr = redlink;
            if(redlink[0]!='\0')
            reqprocess(ptr,"\0");
            //return &redlink;
        }
    }
    //return NULL;
}
