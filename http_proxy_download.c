/* f20180248@hyderabad.bits-pilani.ac.in Mudit Chaturvedi */

/* Program to scrape the html page of the url given and logo for info.in2p3.fr */
/* Submitted By Mudit Chaturvedi , ID Number = 2018A7PS0248H */

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

void redirect();
void findredirect();
void getLogo(char* host);
void extractLogo(char* host);
void writehtml();
void writelogo();

char redlink[1024]={'\0'};
//char host[1024];
char* ip;
char* port;
char* login;
char* password;
char* flc;
char* userpass;
char* fltext = "dummy.txt";
char* logofl;
char* logofltxt = "dummy2.txt";
char req[2048];
char logourl[1024];
char recbuff[4096];
char logobuff[2048];
//http://www.google.com
//http://bits-pilani.ac.in

char *encoder(char *key,int key_len) 
{
    char encoderval[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H','I', 'J', 'K', 'L', 'M', 'N', 'O', 'P','Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X','Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f','g', 'h', 'i', 'j', 'k', 'l', 'm', 'n','o', 'p', 'q', 'r', 's', 't', 'u', 'v','w', 'x', 'y', 'z', '0', '1', '2', '3','4', '5', '6', '7', '8', '9', '+', '/'};
    
    int fin_len = 4 * ((key_len + 2) / 3);
    int modular[] = {0, 2, 1};
 
    char *ans = malloc(sizeof(char)*fin_len);
    int i=0;
    int j=0;
    while(i<key_len) 
    {
        int alpha,beta,gamma;
        if(i < key_len)
            alpha=(char)key[i++];
        else
            alpha=0;
        
        if(i < key_len)
            beta= (char)key[i++];
        else
            beta=0;
        
        if(i < key_len)
            gamma= (char)key[i++];
        else
            gamma=0;
 
        int convfac = (alpha << 0x10) + (beta << 0x08) + gamma;
 
        ans[j++] = encoderval[(convfac >> 3 * 6) & 0x3F];
        ans[j++] = encoderval[(convfac >> 2 * 6) & 0x3F];
        ans[j++] = encoderval[(convfac >> 1 * 6) & 0x3F];
        ans[j++] = encoderval[(convfac >> 0 * 6) & 0x3F];
    }

    i = 0;
    while(i<modular[key_len%3])
    {
    	ans[fin_len - 1 - i] = '=';
    	i++;
    }
        
 
    return ans;
}

void reqprocess(char* host,char fconn[1024])
{
    int c = 0;
    int cumm = 0;
    memset(req, '\0', sizeof(req));
    //printf("%s\n",host);
    fflush(stdin);
    fflush(stdout);
    if(fconn[0]=='\0')
    {
        char* htc = "http://";
        if(strstr(host,"http://")!=NULL)
        {
            sprintf(req,"GET %s/ HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Proxy-Authorization: Basic %s\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",host,host,userpass);
        }
        else
        {
            sprintf(req,"GET %s%s/ HTTP/1.1\r\n"
            "Host: %s%s\r\n"
            "Proxy-Authorization: Basic %s\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",htc,host,htc,host,userpass);
        }
        
        
    }
    else
    {
        strcpy(req,fconn);
    }
    
    struct sockaddr_in serv_addr;
    //char recbuff[4096*2];
    memset(recbuff,0,sizeof(recbuff));
    int socksuccess = 0;
    if((socksuccess = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("\n");
        printf("ERROR : SOCKET FAILED");
        printf("\n");
        return ;
    }

    struct hostent *server;
    server = gethostbyname(ip);
    if(server==NULL)
    {
        printf("HOST NAME INCORRECT");
        printf("\n");
        return ;
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
        return ;
    }

    int conn = connect(socksuccess, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(conn<0)
    {
        printf("\n");
        printf("ERROR : CONNECT FAILED");
        printf("\n");
        return ;
    }
    
    int len = strlen(req);
    int track = 0;
    int tobewritten = write(socksuccess,req+track,len-track);
    if (tobewritten < 0)
    {
        printf("ERROR : WRITING REQUEST TO SOCKET");
        return ;
    }

    track+=tobewritten;
    for(;track < len;)
    {
        tobewritten = write(socksuccess,req+track,len-track);
        if (tobewritten < 0){
            printf("ERROR : WRITING REQUEST TO SOCKET");
            return ;
        }
        if (tobewritten == 0)
            break;
        track+=tobewritten;
    } 

    //printf("REQUEST SENT\n");
    //printf("%s\n",req);

    FILE *fl = fopen(fltext, "w+");
    int flag = 0;
    memset(recbuff,'\0',sizeof(recbuff));
    int downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    for(;downloadtrack>0;)
    {
        fwrite(recbuff, downloadtrack , 1, fl );
        memset(recbuff,'\0',sizeof(recbuff));
        downloadtrack = recv(socksuccess,recbuff,sizeof(recbuff),0);
    }
    
    //printf("\n");
    fclose(fl);
    close(socksuccess);
    
    redirect();
    writehtml();
    if(strstr("http://info.in2p3.fr/",host)!=NULL)
    {
        getLogo(host);
    }
}
void writehtml()
{
    FILE* fr = fopen("dummy.txt","r");
    FILE* fw = fopen(flc,"w+");
    char c;
    while((c = fgetc(fr))!=EOF)
    {
        while((c = fgetc(fr))!=EOF && c!='<')
        continue;
        if(c=='<')
        fputc(c,fw);
        while((c = fgetc(fr))!=EOF)
        {
            fputc(c,fw);
        }
        fclose(fw);
        fclose(fr);
        break;
    }
}

void getLogo(char* host)
{
    FILE* fl = fopen("dummy.txt","r");
    char buff[4096];
    memset(buff,'\0',sizeof(buff));
    char* tagcheck = "<IMG SRC=";
    char* pos;
    int counter=0;
    while(fgets(buff,4096,fl) != NULL)
    {
        pos = strstr(buff,tagcheck);
        if(pos!=NULL)
        {
            int offset = strlen(tagcheck);
            pos+=offset;
            while(*pos!='\"')
            pos++;
            pos++;
            while(*pos!='\"')
            {
                logourl[counter++] = *pos;
                pos++;
            }
            logourl[counter++] = '\0';
            //printf("%s\n",logourl);
            //printf("%d\n",strlen(logourl));
            //printf("%s\n",host);
            //printf("%d\n",strlen(host));
            extractLogo(host);
            
        }
        memset(buff,'\0',sizeof(buff));
    }
    fclose(fl);

}

void extractLogo(char* host)
{
    memset(req, '\0', sizeof(req));

    char* htc = "http://";
        if(strstr(host,"http://")!=NULL)
        {
            sprintf(req,"GET %s/%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Proxy-Authorization: Basic %s\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",host,logourl,host,userpass);
        }
        else
        {
            sprintf(req,"GET %s%s/%s HTTP/1.1\r\n"
            "Host: %s%s\r\n"
            "Proxy-Authorization: Basic %s\r\n"
            "User-Agent: myapp\r\n"
            "Accept: */*\r\n"
            "Proxy-Connection: close\r\n"
            "\r\n",htc,host,logourl,htc,host,userpass);
        }

    struct sockaddr_in serv_addr;
    memset(recbuff,0,sizeof(recbuff));
    int socksuccess = 0;
    if((socksuccess = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("\n");
        printf("ERROR : SOCKET FAILED");
        printf("\n");
        return ;
    }

    struct hostent *server;
    server = gethostbyname(ip);
    if(server==NULL)
    {
        printf("HOST NAME INCORRECT");
        printf("\n");
        return ;
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
        return ;
    }

    int conn = connect(socksuccess, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(conn<0)
    {
        printf("\n");
        printf("ERROR : CONNECT FAILED");
        printf("\n");
        return ;
    }
    
    int len = strlen(req);
    int track = 0;
    int tobewritten = write(socksuccess,req+track,len-track);
    if (tobewritten < 0)
    {
        printf("ERROR : WRITING REQUEST TO SOCKET");
        return ;
    }

    track+=tobewritten;
    for(;track < len;)
    {
        tobewritten = write(socksuccess,req+track,len-track);
        if (tobewritten < 0){
            printf("ERROR : WRITING REQUEST TO SOCKET");
            return ;
        }
        if (tobewritten == 0)
            break;
        track+=tobewritten;
    } 

    //printf("REQUEST SENT\n");
    //printf("%s\n",req);
    //printf("HI\n");
    
    memset(logobuff,0,sizeof(logobuff));
        FILE* flw = fopen(logofl, "w+");
        int flag=0;
        long total=0;
        int receive = recv(socksuccess,logobuff,sizeof(logobuff),0);
        while(receive>0){
            char* pos = strstr(logobuff,"\r\n\r\n");
            //char * checkpos = strstr(logobuff,"\n\n\n\n");
            if(flag)
            {
            	
                fwrite(logobuff,receive,1,flw);
                //printf("%s",logobuff);
            }
            else if(pos!=NULL)
            {
            	//printf("%s",logobuff);
            	//printf("\n");
            	//printf("pos aaya %s\n",pos);
                pos+=4;
                //checkpos+=4;
                //printf("pos aaya %s\n",pos);
                int tetris= pos-logobuff;
                //printf("%d\n",receive-tetris);
                //fprintf(flw,"%s",pos);
                fwrite(pos,receive-tetris,1,flw);
                //printf("%s",logobuff);
                flag=1;
            }
            memset(logobuff,0,sizeof(logobuff));
            receive = recv(socksuccess,logobuff,sizeof(logobuff),0);
            
        }
        fclose(flw);
        close(socksuccess);
    //writelogo();
}
void writelogo()
{
    FILE* fr = fopen("dummy2.txt","r");
    FILE* fw = fopen(logofl,"wb");
    char c;
    while((c = fgetc(fr))!=EOF)
    {
        while((c = fgetc(fr))!=EOF && c!='<')
        continue;
        if(c=='<')
        fputc(c,fw);
        while((c = fgetc(fr))!=EOF)
        {
            fputc(c,fw);
        }
        fclose(fw);
        fclose(fr);
        break;
    }
}

int main(int argc, char *argv[])
{    
    if(argc<8)
    {
        printf("\n");
        printf("ERROR : INCOMPLETE INFO");
        printf("\n");
        return 0;
    }
    ip = argv[2];
    port = argv[3];
    login = argv[4];
    password = argv[5];
    flc = argv[6];
    logofl = argv[7];

    char fconn[1024];

    fflush(stdin);
    fflush(stdout);
    char reqapp[1024];
    sprintf(reqapp,"%s:%s",argv[4],argv[5]);
    userpass = encoder(reqapp,strlen(reqapp));
    char* htc = "http://";
    if(strstr(argv[1],"http://")!=NULL)
    {
        sprintf(fconn,"GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Proxy-Authorization: Basic %s\r\n"
        "User-Agent: myapp\r\n"
        "Accept: */*\r\n"
        "Proxy-Connection: close\r\n"
        "\r\n",argv[1],argv[1],userpass);
    }
    else
    {
        sprintf(fconn,"GET %s%s HTTP/1.1\r\n"
        "Host: %s%s\r\n"
        "Proxy-Authorization: Basic %s\r\n"
        "User-Agent: myapp\r\n"
        "Accept: */*\r\n"
        "Proxy-Connection: close\r\n"
        "\r\n",htc,argv[1],htc,argv[1],userpass);
    }
    fflush(stdin);
    reqprocess(argv[1],fconn);
}

void redirect()
{
    FILE* fl = fopen(fltext,"r");
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
    //printf("\nCode is %d\n", atoi(str));
    if(atoi(str)==303 || atoi(str)==301 || atoi(str)==302 || atoi(str)==300 || atoi(str)==308 || atoi(str)==309 || atoi(str)==304)
    {
        printf("Redirecting request to moved location\n");
        fclose(fl);
        findredirect();
    }
}

void findredirect()
{
    FILE* fl = fopen(fltext,"r");
    char loc[] = "Location: ";
    int offset = strlen(loc);
    char* pos;
    char buff[4096];
    memset(redlink,'\0',sizeof(redlink));
    int counter=0;
    while(fgets(buff,4096,fl) != NULL)
    {
        pos = strstr(buff,loc);
        if(pos != NULL)
        {
            pos+=offset;
            while(*pos!=' ' && *pos!='\n' && *pos!='\r')
            {
                redlink[counter++] = *pos;
                pos++;
            }
            redlink[counter++] = '\0';
            //printf("%s\n",redlink);
            //printf("%d\n",strlen(redlink));
            fflush(stdout);
            char* ptr = redlink;
            if(strstr(redlink,"https:")!=NULL)
            printf("Redirected to https\n Ending now\n");
            if(redlink[0]!='\0')
            reqprocess(ptr,"\0");
            break;
        }
    }
    fclose(fl);
}
