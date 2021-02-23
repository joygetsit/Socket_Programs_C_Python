// Server side implementation of UDP client-server model

// Build command - clang socket_udp_server_C.c -o socket_udp_server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

// Define keyboard hit library
#include "conio.h"

// Defines and creates socket
#include <sys/socket.h>
#include <arpa/inet.h>

// Gives socket structure
#include <netinet/in.h>

#define PORTSEND 20006
#define PORTRECV 20011
#define ReceiveUDPSocket_IP "127.0.0.1"
#define MAXLINE 256

/************** Define structures for sending and receiving data *****/
typedef struct payloadrecv_t {
    uint32_t ID;
    float PosDiff;
} payload_recv;

typedef struct payloadsend_t {
    uint32_t ID;
    float X;
    float Y;
    float Z;
} payload_send;

static payload_send payload_send_msg;
/************** Define structures for sending and receiving data *****/

// Test to see correct data being received or not
int i = 0;
// Driver code
int main() {
/***** Define communication sockets and parameters ******/
	int socksendfd, sockrecvfd;
	struct sockaddr_in UDPsendservaddr, UDPrecvcliaddr;
    int n;
	memset(&UDPsendservaddr, 0, sizeof(UDPsendservaddr));
	memset(&UDPrecvcliaddr, 0, sizeof(UDPrecvcliaddr));

	// Filling UDP socket server information to send from
	UDPsendservaddr.sin_family = AF_INET; // IPv4
	UDPsendservaddr.sin_addr.s_addr = INADDR_ANY; // Any address because this socket will just be used to send data
	UDPsendservaddr.sin_port = htons(PORTSEND);

	// Filling UDP socket server information to receive from
	UDPrecvcliaddr.sin_family = AF_INET; // IPv4
	UDPrecvcliaddr.sin_addr.s_addr = inet_addr(ReceiveUDPSocket_IP);
	UDPrecvcliaddr.sin_port = htons(PORTRECV);

    socklen_t address_length = sizeof(UDPrecvcliaddr);

	// Creating UDP socket file descriptor for sending
	if ( (socksendfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	// Creating UDP socket for receiving
	if ( (sockrecvfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
    }

	// Bind the socket with the server recv address
	if ( bind(sockrecvfd, (const struct sockaddr *)&UDPrecvcliaddr,
	                                sizeof(UDPrecvcliaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Set kernel buffer size
//    int optVal1 = 3328; //1024*3.25 // Default size = 212992 for 256*256 packets with overhead,
//    //which me ans 3.25 * buffer size you want
//    int optLen11 = sizeof(int);
//    setsockopt(sockrecvfd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal1, &optLen1);
//    printf("Setting SockOpt value: %d\n", optVal1);

/***** Define communication sockets and parameters ******/

/***** Define data to send according to data structure ******/

	char recvBuffer[MAXLINE];
	//char *hello = "Hello from server";
    char sendBuffer[sizeof(payload_send_msg)];
/***** Define data to send according to data structure ******/
    // Main Loop
    while (!_kbhit())
    {

//        // Test code for checking UDP rx and tx buffer sizes
//        int optVal;
//        int optLen = sizeof(int);
//        getsockopt(sockrecvfd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen);
//        printf("Initial SockOpt value: %d\n", optVal);

/******* Sending data ****************/
        float tx = 7.291, ty = 100.235, tz = 110.635;
        payload_send_msg.ID = 6+i;
        payload_send_msg.X = tx;
        payload_send_msg.Y = ty;
        payload_send_msg.Z = tz;
        memcpy(sendBuffer, &payload_send_msg, sizeof(payload_send_msg));
        payload_send *pq = (payload_send*) sendBuffer;
        printf("Data sent: ID=%d, (X,Y,Z)=(%f,%f,%f)\n",
                pq->ID, pq->X, pq->Y, pq->Z/*,getsockopt(sockrecvfd)*/);
        i++;
        // Sending message from arbitrary port using created socket
        sendto(socksendfd, (const char *)pq, sizeof(payload_send), 0,
            (const struct sockaddr *)&UDPsendservaddr,
                address_length); //MSG_CONFIRM,
/******* Sending data ****************/

/******* Receiving data ****************/
    	n = recvfrom(sockrecvfd, (char *)recvBuffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &UDPrecvcliaddr,
				&address_length);
        recvBuffer[n] = '\0';
        printf("Client : bytes: %d\n", n);

        payload_recv *p = (payload_recv*)recvBuffer;

        printf("Received contents: ID_received=%d, PosDiff=%f\n\n",
                p->ID, p->PosDiff);

        // Test code to see calculation
        //printf("Add: %d\n", (p->id + p->counter));
/******* Receiving data ****************/

        sleep(1);
    }
    close(sockrecvfd);
    return 0;
}
