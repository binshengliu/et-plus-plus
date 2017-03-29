#ifndef sunsockets_First
#define sunsockets_First

extern "C" {
    int TcpConnect(char *hostname, char *servicename);
    int UnixConnect(char *name);
    int TcpService(char *servicename);
    int UnixService(char *servername);
    int ConnectService(char *servername, char *servicename);    
    int accept(int, int, int);
}

#endif sunsockets_First
