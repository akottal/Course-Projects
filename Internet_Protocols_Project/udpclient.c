//=============================================udpExample.c====
//    Program to send UDP packets to remote server
//
//=============================================================
//
//
// Author= Vamsi Krishna Kalapala
//	   200062964	
//
//
//
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>        
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM           8222  // Port number used
#define  IP_ADDR      "10.0.0.254" // IP address of server1 (*** HARDWIRED ***)
#define LOCAL_IP_ADDRESS "10.0.2.3"// Local ip address
#define LOCAL_PORT 57913 // local port number to bind the socket



int main() {
    int ret, fd,port_num;
    struct sockaddr_in sa_dst;
    struct sockaddr_in sa_loc;
    char out_string[30];
    char name[]="WELCOME";
    char class_descp[]="CSC573 DEMO";

    //Creating socket along with its type

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Setting up local socket
    memset(&sa_loc, 0, sizeof(struct sockaddr_in));
    sa_loc.sin_family = AF_INET;
    sa_loc.sin_port = htons(LOCAL_PORT);
    sa_loc.sin_addr.s_addr = inet_addr(LOCAL_IP_ADDRESS);

    ret = bind(fd, (struct sockaddr *)&sa_loc, sizeof(struct sockaddr));
    port_num = htons(sa_loc.sin_port);

    // setting the desired message to be sent
    sprintf(out_string,"%s\n%s\n",name,class_descp);
    printf("%s",out_string);

    // Setting up remote socket
    memset(&sa_dst, 0, sizeof(struct sockaddr_in));
    sa_dst.sin_family = AF_INET;
    sa_dst.sin_port = htons(PORT_NUM); // remote server assigned port number
    sa_dst.sin_addr.s_addr = inet_addr(IP_ADDR); // remote server ip address

    //Connecting to the specified remote port on server
    ret = connect(fd, (struct sockaddr *)&sa_dst, sizeof(struct sockaddr));
    if (ret < 0)
       {
        printf("*** ERROR - connect() failed \n");
        exit(-1);
       }

   //Sending the packet to remote server
   ret= send(fd, out_string, strlen(out_string), 0);
   if (ret < 0)
     {
      printf("*** ERROR - send() failed \n");
      exit(-1);
     }

   //Receiving reply from the reote server
   ret= recv(fd, out_string, sizeof(out_string), 0);
   if (ret < 0)
     {
      printf("*** ERROR - recv() failed \n");
      exit(-1);
     }
printf("%s",out_string);
return 0;
}
