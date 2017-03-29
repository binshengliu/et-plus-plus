#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>

extern void gethostname();
extern char *getenv();

#define SERVERPATH "/tmp"
#define PROTOCOLNAME "tcp"


int TcpConnect(hostname, servicename)
char *hostname, *servicename;
{
    int sock;
    struct sockaddr_in server;
    struct hostent *host_ptr;
    struct servent *sp;
    
    if ((sp= getservbyname(servicename, PROTOCOLNAME)) == NULL) {
	Error("TcpConnect", "no service \"%s\" with protocol \"%s\"\n",
						servicename, PROTOCOLNAME);
	return -1;
    }
    
    if ((host_ptr= gethostbyname(hostname)) == 0) {
	Error("TcpConnect", "unknown host %s\n", hostname);
	return -1;
    }
    
    /* Check the address type for an internet host */
    if (host_ptr->h_addrtype != AF_INET) {
	Error("TcpConnect", "%s is not an internet host\n", hostname);
	return -1;
    }
    
    bzero(&server, sizeof(server));
    bcopy(host_ptr->h_addr, &server.sin_addr, host_ptr->h_length);
    server.sin_family = host_ptr->h_addrtype;
    server.sin_port= htons(sp->s_port);
    
    /* Create socket */
    if ((sock= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	SysError("ConnectTcp", "socket");
	return -1;
    }
	
    if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
	/* SysError("ConnectTcp", "connect"); */
	return -1;
    }
    return sock;
}

int UnixConnect(name)
char *name;
{
    int sock;
    char buf[100];
    struct sockaddr_un unserver;
    
    sprintf(buf, "%s/%s", SERVERPATH, name);
    
    unserver.sun_family= AF_UNIX;
    strcpy(unserver.sun_path, buf);
    
    /* open socket */
    if ((sock= socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	SysError("UnixConnect", "socket");
	return -1;
    }
	    
    if (connect(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2) < 0) {
	/* SysError("UnixConnect", "connect"); */
	return -1;
    }
    return sock;
}

int TcpService(servicename)
char *servicename;
{
    int retry, sock;
    struct sockaddr_in inserver;
    struct servent *sp;
    
    if ((sp= getservbyname(servicename, PROTOCOLNAME)) == NULL) {
	Warning("TcpService", "no service \"%s\" with protocol \"%s\"\n",
						    servicename, PROTOCOLNAME);
	return -1;
    }
    
    /* Create tcp socket */
    if ((sock= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	SysError("TcpService", "socket");
	return -1;
    }
	
    bzero(&inserver, sizeof(inserver));
    inserver.sin_family= AF_INET;
    inserver.sin_addr.s_addr= htonl(INADDR_ANY);
    inserver.sin_port= htonl(sp->s_port);
    
    /* bind socket */
    for (retry= 20; bind(sock, (struct sockaddr*) &inserver, sizeof(inserver)); retry--) {
	if (retry <= 0) {
	    SysError("TcpService", "bind");
	    return -1;
	}
	sleep(10);
    }

    /* Start accepting connections */
    if (listen (sock, 5)) {
	SysError("TcpService", "listen");
	return -1;
    }
    
    return sock;
}

/* returns socket fd or -1 */
int UnixService(servername)
char *servername;
{
    struct sockaddr_un unserver;
    int sock, oldumask;
    
    bzero(&unserver, sizeof(unserver));
    unserver.sun_family = AF_UNIX;
    
    /* assure that socket directory exists */
    oldumask= umask(0);
    mkdir(SERVERPATH, 0777);
    umask(oldumask);
    sprintf(unserver.sun_path, "%s/%s", SERVERPATH, servername);
    
    /* remove old socket */
    unlink(unserver.sun_path);
    
    /* Create socket */
    if ((sock= socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	SysError("UnixService", "socket");
	return -1;
    }
    
    if (bind(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2)) {
	SysError("UnixService", "bind");
	return -1;
    }
    
    /* Start accepting connections */
    if (listen(sock, 5)) {
	SysError("UnixService", "listen");
	return -1;
    }
    
    return sock;
}

int ConnectService(servername, servicename)
char *servername, *servicename;
{    
    char *host, hostname[30], *display, *cp;
    
    host= getenv("ET_DISPLAY");
    if (host == 0)
	host= "unix:ET++";
	
    for (cp= host; *cp; cp++)
	if (*cp == ':') {
	    *cp= '\0';
	    display= cp+1;
	}

    if (*host == '\0')
	host= "unix";

    gethostname(hostname, sizeof(hostname));
    
    if (strcmp(host, "unix") == 0 || strcmp(host, hostname) == 0)
	return UnixConnect(servername);
    return TcpConnect(host, servicename);
}
