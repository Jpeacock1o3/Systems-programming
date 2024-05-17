/*This assignment involves creating a program that reads and displays information from the host,
network, protocol, and services databases. The program outputs host names, address types, 
and addresses; network names and numbers; protocol names and numbers; and service names, ports, 
and protocols, displaying all information to the standard output.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main (int argc, char **argv) {
     struct hostent *hostEnt;
     struct netent *netEnt;
     struct protoent *protoEnt;
     struct servent *servEnt;

    sethostent(1);
    hostEnt = gethostent();
   
    while (hostEnt != NULL) {
        char *hostName = hostEnt->h_name;
        int type = hostEnt->h_addrtype;
        int len = hostEnt->h_length;
        char **addr_list = hostEnt->h_addr_list;


        printf("Host Name: %s, Address Type: %d, Address Length: %d\n", hostName, type, len);

        for (int i = 0; addr_list[i] != NULL; i++) {
            struct in_addr addr;
            memcpy(&addr.s_addr, addr_list[i], sizeof(addr.s_addr));

            char *addr_str = inet_ntoa(addr);
            if (addr_str == NULL) {
                perror("inet_ntoa");
                return -1;
            }

            printf("Network Address: %s\n", addr_str);
        }

        printf("\n");

        hostEnt = gethostent();
    }
   

    setnetent(1);
netEnt = getnetent();
    while (netEnt != NULL) {
        char *netName = netEnt->n_name;
        uint32_t netNum = netEnt->n_net;

        printf("Network Name: %s and Network Number: %d\n\n", netName, ntohl(netNum));
        netEnt = getnetent();  
    }

    setprotoent(1);
    protoEnt = getprotoent();
    while (protoEnt != NULL) {
        printf("Protocol Name: %s and Protocol Number: %d\n\n", protoEnt->p_name, protoEnt->p_proto);

        protoEnt = getprotoent();
    }

    setservent(1);
    servEnt = getservent();
    while (servEnt != NULL) {
        printf("Service Name: %s, Port Number: %d, Name of Protocol: %s\n\n", servEnt->s_name, servEnt->s_port, servEnt->s_proto);

        servEnt = getservent();
    }

    endservent();
    endprotoent();
    endnetent();
    endhostent();
    return 0;
}
