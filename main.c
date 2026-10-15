#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int main() {
    // So, here i created a socket based on the IPV6 format (compatible with IPV4)
    // along with the standart way of managing packages in TCP (SOCK_STREAM)
    // Parameters, i don't really know but docs said 0 is the most common.
    int mainSocket = socket(AF_INET6, SOCK_STREAM, 0);

    if (mainSocket == -1) { // error handling because, why not?
        printf("Main socket failed to create \n");
        return -1;
    }
    /*
        Here I declare the address where the server is going to receive the requests
        After declaring the address using an array o charachters i declare a struct that helps
        dealing with the compiler 
            DISCLAIMER: I really don't understand at all how this works, even so i figured it out by looking at an
            example in the man documentation and researching the strncpy function and just gluing all in,
        
        after all that i just declare the bind, check for errors and we all done.

        HELP:
        https://oneuptime.com/blog/post/2026-03-20-struct-sockaddr-in6-c/view
    */

    char SOCK_ADDRESS[] = "[::1]:8080";

    struct sockaddr_in6 my_addr;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(8080);
    my_addr.sin6_flowinfo = 0;
    my_addr.sin6_addr = in6addr_loopback;
    my_addr.sin6_scope_id = 0;

    int mainBind = bind(mainSocket, (struct sockaddr *) &my_addr, sizeof(my_addr));

    if (mainBind == -1) {
        printf("mainBind failed: %d \n", errno);
        return -1;
    }

    return 0;
}