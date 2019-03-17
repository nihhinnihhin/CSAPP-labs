#include <stdio.h>
#include <string.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

// function prototype
void clienterror(int fd, char *cause, char *errnum,
			     char *shortmsg, char *longmsg);
void asServer(int connfd);
void parse_uri_p(char *uri, char *svrHostname, char *svrPort, char *query_uri);
void read_requesthdrs_p(int clientfd, char *svrHostname, rio_t *serverRio);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

// robustness: long-running processes and web proxies are no exception.
// what kinds of types of errors should be considered
// how to react, should not immediately exit.

// client and server
int main(int argc, char **argv)
{

	// as server
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  // Enough space for any address
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if(argc != 2){
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	listenfd = Open_listenfd(argv[1]);
	while(1){
		clientlen = sizeof(struct sockaddr_storage);
		connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
		Getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE,
				client_port, MAXLINE, 0);
		printf("Proxy connected to (%s, %s)\n", client_hostname, client_port);
		asServer(connfd);
		Close(connfd);
	}
    printf("%s", user_agent_hdr);
    return 0;
}


void asServer(int connfd)
{
	// as client
	int clientfd;
	char svrHostname[MAXLINE], query_uri[MAXLINE];
	char svrPort[MAXLINE] = "80";
	rio_t clientRio;

	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	rio_t serverRio;
	
	// Read request line from client
	Rio_readinitb(&serverRio, connfd);
	Rio_readlineb(&serverRio, buf, MAXLINE);
	printf("Request headers:\n");
	printf("%s", buf);
	sscanf(buf, "%s %s %s", method, uri, version);	// read input
    if (strcasecmp(method, "GET")) {                     
        clienterror(connfd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                   
	
	// Parse URI
	parse_uri_p(uri, svrHostname, svrPort, query_uri);

	printf("svrHostname: %s, port: %s, query %s", svrHostname, svrPort, query_uri);
	
	// proxy connect the server as client
	clientfd = Open_clientfd(svrHostname, svrPort);
	Rio_readinitb(&clientRio, clientfd);

	// send the request line to server
	sprintf(buf, "GET %s HTTP/1.0", query_uri);
	Rio_writen(clientfd, buf, strlen(buf));
	
	// read request headers from client and send them to server
	read_requesthdrs_p(clientfd, svrHostname, &serverRio);

	// read result from server and write to client
	Rio_readlineb(&clientRio, buf, MAXLINE);
	while(strcmp(buf, "\r\n")){
		Rio_writen(connfd, buf, strlen(buf));
		printf("%s", buf);
		Rio_readlineb(&clientRio, buf, MAXLINE);
	}
	Close(clientfd);
}




void parse_uri_p(char *uri, char *svrHostname, char *svrPort, char *query_uri)
{
	char *afterTwoSlash, *thirdSlash, *colon;
	afterTwoSlash = strstr(uri, "/");
	afterTwoSlash+=2; // skip the first two /
	thirdSlash = strstr(afterTwoSlash, "/");
	colon = strstr(afterTwoSlash, ":");
	strcpy(query_uri, thirdSlash);
	if(colon != NULL)
	{
		int nameLen = colon -afterTwoSlash;
		strncpy(svrHostname, afterTwoSlash, nameLen);
		svrHostname[nameLen]='\0';
		int portLen = thirdSlash-(colon+1);
		strncpy(svrPort, colon+1, portLen);
		svrPort[portLen]='\0';
	}
	else
	{
		int nameLen = thirdSlash - afterTwoSlash;
		strncpy(svrHostname, afterTwoSlash, nameLen);
		svrHostname[nameLen]='\0';
	}
	
}

void read_requesthdrs_p(int clientfd, char *svrHostname, rio_t *serverRio) 
{
    char buf[MAXLINE];
	char sendBuf[MAXLINE];
    Rio_readlineb(serverRio, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
	Rio_readlineb(serverRio, buf, MAXLINE);
	printf("%s", buf);
    }

	// always send some request headers
	sprintf(sendBuf, "Host: %s", svrHostname);
	Rio_writen(clientfd, sendBuf, strlen(sendBuf));
	printf("%s", sendBuf);

	strcpy(sendBuf , user_agent_hdr);
	Rio_writen(clientfd, sendBuf, strlen(sendBuf));
	printf("%s", sendBuf);

	strcpy(sendBuf,"Connection: close");
	Rio_writen(clientfd, sendBuf, strlen(sendBuf));
	printf("%s", sendBuf);
	
	strcpy(sendBuf,"Proxy-Connection: close");
	Rio_writen(clientfd, sendBuf, strlen(sendBuf));
	printf("%s", sendBuf);

    return;
}

/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
/* $end clienterror */
