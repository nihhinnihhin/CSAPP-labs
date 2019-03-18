#include <stdio.h>
#include <string.h>
#include "csapp.h"

/* Recommended max Cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

// data structure
typedef struct 
{
  char host[MAXLINE];
  char port[MAXLINE];
  char query[MAXLINE];
} Rqst_line;

typedef struct
{
  char key[MAXLINE];
  char val[MAXLINE];
} Rqst_header;

typedef struct 
{
  char *id;
  char *object;
} Cache_line;

typedef struct 
{
  int cnt;
  Cache_line *objects;
} Cache;


// reader-writer data objects
// Cache cache;
// int readcnt;
// sem_t mutex, w;

// function prototype
void asServer(int serverfd);
void parse_request(int serverfd, rio_t *serverRio, Rqst_line *line, Rqst_header *headers, int *num_hdrs);
void parse_uri(char *uri, Rqst_line *line);
Rqst_header parse_header(char *line);
void send_request(int clientfd, rio_t *serverRio, Rqst_line *line, Rqst_header *headers, int num_hdrs);
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
	int listenfd, serverfd;
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
		serverfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
		Getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE,
				client_port, MAXLINE, 0);
		printf("Proxy connected to (%s, %s)\n", client_hostname, client_port);
		asServer(serverfd);
		Close(serverfd);
	}
    printf("%s", user_agent_hdr);
    return 0;
}


void asServer(int serverfd)
{
  int clientfd=0;
  int num_hdrs;
  char buf[MAXLINE];
  Rqst_line line;
  Rqst_header headers[20];
  rio_t serverRio, clientRio;
  Rio_readinitb(&serverRio, serverfd);

	// Parse request
  parse_request(serverfd, &serverRio, &line, headers, &num_hdrs);
	
	// proxy connect the server as client
	clientfd = Open_clientfd(line.host, line.port);
	Rio_readinitb(&clientRio, clientfd);

	// send the request to server
	send_request(clientfd, &serverRio, &line, headers, num_hdrs);

	// read result from server and write to client
	Rio_readlineb(&clientRio, buf, MAXLINE);
	while(strcmp(buf, "\r\n")){
		Rio_writen(serverfd, buf, strlen(buf));
		printf("< %s", buf);
		Rio_readlineb(&clientRio, buf, MAXLINE);
	}
	Close(clientfd);
}


void parse_request
(int serverfd, rio_t *serverRio, Rqst_line *line, Rqst_header *headers, int *num_hdrs)
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];

    Rio_readinitb(serverRio, serverfd);
    if (!Rio_readlineb(serverRio, buf, MAXLINE))  
        return;
    
    //parse request line
    sscanf(buf, "%s %s %s", method, uri, version);
    parse_uri(uri, line);
    *num_hdrs = 0;
    Rio_readlineb(serverRio, buf, MAXLINE);
    while(strcmp(buf, "\r\n")) {
      headers[(*num_hdrs)++] = parse_header(buf);
      Rio_readlineb(serverRio, buf, MAXLINE);
    }
}




void parse_uri(char *uri, Rqst_line *line)
{
	char *afterTwoSlash, *thirdSlash, *colon;
	afterTwoSlash = strstr(uri, "/");
	afterTwoSlash+=2; // skip the first two /
	thirdSlash = strstr(afterTwoSlash, "/");
	colon = strstr(afterTwoSlash, ":");
	strcpy(line->query, thirdSlash);
	if(colon != NULL)
	{
		int nameLen = colon - afterTwoSlash;
		strncpy(line->host, afterTwoSlash, nameLen);
		line->host[nameLen]='\0';
		int portLen = thirdSlash-(colon+1);
		strncpy(line->port, colon+1, portLen);
		line->port[portLen]='\0';
	}
	else
	{
		int nameLen = thirdSlash - afterTwoSlash;
		strncpy(line->host, afterTwoSlash, nameLen);
		line->host[nameLen]='\0';
    strcpy(line->port,"80");
	}
}


Rqst_header parse_header(char *line) {
    Rqst_header header;
    char *c = strstr(line, ": ");
    if (c == NULL) {
        fprintf(stderr, "Error: invalid header: %s\n", line);
        exit(0);
    }
    *c = '\0';
    strcpy(header.key, line);
    strcpy(header.val, c+2);
    return header;
}

void send_request(int clientfd, rio_t *serverRio, Rqst_line *line, Rqst_header *headers, int num_hdrs)
{
  char buf[MAXLINE], *buf_head = buf;
  sprintf(buf_head, "GET %s HTTP/1.0\r\n", line->query);
  printf("> %s", buf_head);
  buf_head = buf + strlen(buf);
  for (int i = 0; i < num_hdrs; ++i) {
        sprintf(buf_head, "%s: %s", headers[i].key, headers[i].val);
        printf("> %s", buf_head);
        buf_head = buf + strlen(buf);
    }
  sprintf(buf_head, "\r\n");
  printf("> %s", buf_head);
  Rio_writen(clientfd, buf, MAXLINE);
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
