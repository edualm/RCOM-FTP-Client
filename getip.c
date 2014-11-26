#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

const char * get_ip_address_with_hostname(const char *hostname) {
    struct hostent *h;
    
    if ( (h = gethostbyname(hostname)) == NULL ) {
        herror("gethostbyname");
        
        return NULL;
    }
    
    return inet_ntoa(* ( (struct in_addr *) h->h_addr) );
}
