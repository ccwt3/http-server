#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define BUF_SIZE 4096

int main()
{
    // So, here i created a socket based on the IPV6 format (compatible with IPV4)
    // along with the standart way of managing bytes in TCP (SOCK_STREAM)
    // Parameters, 0 means using the default protocol for this type, which is TCP.
    int mainSocket = socket(AF_INET6, SOCK_STREAM, 0);

    if (mainSocket == -1)
    { // error handling because, why not?
        printf("Main socket failed to create \n");
        return -1;
    }
    /*
        DISCLAIMER: I really don't understand at all how this works, even so i figured it out by looking at the man examples, researching through
        the web and asking AI to conceptually whar errno:22 means.

        first i declare the compatible struct with my address family (Ipv6).

        Then I saw that it's recommended to clean up the space that you'll use with 0.

        I assigned the values with help of (1), AF_INET6 is the must when dealing with ipv6,

        There is something called endianing that asks for byte order and it's there where htons arranges de port in byte order.
        The way this works is with "Big endian" and "Little endian" where our system is little-endian (arranges the addres from
        smallest to biggest value) and htons transforms it into big-endian, making it compatible with the network standart

        For the address looback and any are the most general and easy ones however loopback only accepts request from my machine
        while _any accepts from, any interface of the machine (wifi, ethernet, inside-routes), so we are going with _any.

        The same with flowinfo and scopeid so in most cases we set them to 0.

        We bind, make a type cast and declare the byte size, check for error and all done!.

        HELP:
        (1) https://oneuptime.com/blog/post/2026-03-20-struct-sockaddr-in6-c/view
    */

    struct sockaddr_in6 my_addr;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(8080);
    my_addr.sin6_flowinfo = 0;
    my_addr.sin6_addr = in6addr_any;
    my_addr.sin6_scope_id = 0;

    int socketBind = bind(mainSocket, (struct sockaddr *)&my_addr, sizeof(my_addr));

    if (socketBind == -1)
    {
        printf("bind failed: %d \n", errno);
        return -1;
    }

    /*
        This part was easy, I just followed what the documentation said.
        Inserted the socket i wanted to make "passive" along with how many non-accepted connectios in
        backlog can be in there, checked for error and that's all.
    */

    int listenResult = listen(mainSocket, 511);

    if (listenResult == -1)
    {
        printf("socket listener failed: %d \n", errno);
        return -1;
    }

    /*
        So, again, here we do some shinenanigans where we declare a 'peer addres'
        which is a struct where there will be store the client's address.

        Then we add the main socket (listener), add the address of the perr struct
        to know where to store the info, and the size of this.

        at the end we get the clientSocket that is the socket in charge of handling
        the client's specific requests.
    */
    struct sockaddr_in6 peer_addr;
    socklen_t peer_addr_size;
    peer_addr_size = sizeof(peer_addr);

    int clientSocket = accept(mainSocket, (struct sockaddr *)&peer_addr, &peer_addr_size);

    if (clientSocket == -1)
    {
        printf("socket accept failed: %d \n", errno);
        return -1;
    }

    /*
        NOTES:
            recv can store whole requess or partial ones, so it isn't  limited by amount of requests
            but by it's size.

        Now we get the receiver, this guy as far as i understand copies whatever is in
        the buffer and returns the amount of bytes copied (receive is a flow blocking process).
        
        So what now? We declare the buffer, this guy is the one that will reserve space
        for storing the request, 4096 is a reasonable size for the buffer (at least for this)
        so we set it like that.

        Once the buffer is set, we tell the receiver to read from the CLIENT socket because
        this is the one that will handle this specific client's requests.

        The 0 value represents the flags, at this point as our server is very basic and tiny 
        we have not to worry about these and so we set the flags to default.

        When checking for error we can also encounter with the value 0 which,
        has a special meaning in this usecase for it mean that the client
        closed the conection without any request.
    */
    
    char buf[BUF_SIZE];

    ssize_t receive = recv(clientSocket, buf, BUF_SIZE, 0);

    if (receive == -1) {
        printf("socket receive failed: %d \n", errno);
        return -1;
    } else if (receive == 0) {
        printf("client closed conection \n");
        return 0;
    }

    return 0;
}