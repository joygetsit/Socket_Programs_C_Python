// Feedback on-1/off-0
#define Enable_Haptics 0

// Include the header file for Socket Communication
#include "comms.h"
#define PORTSEND 18007
#define PORTRECV 18009
#define SendtoUDPSocket_IP "127.0.0.1" //"10.114.64.87" //"10.0.0.3" //
#define ReceiveUDPSocket_IP "127.0.0.1" //"10.221.19.64" //"10.0.0.4" //

// Define clock timestamp
int64_t Timer_Start = 0, Time_to_receive = 0, Timer_Loop = 0;

/* Data structures to send and receive */
typedef struct payloadrecv_t {
    // uint32_t MsgIDfeedback;
    // float PosDiff;
    uint32_t IDr;
    // double receive[16]; 
    // float PosDiff;
    float moveDXr;
    float moveDYr;
    float moveDZr;    
} payload_recv;


typedef struct payloadsend_t {
    uint32_t IDs;
    // double send[16];  
    float moveDXs;
    float moveDYs;
    float moveDZs;
} payload_send;
/* Data structures to send and receive */

static payload_send payload_send_msg;

/******************************************************************************
 Runs the main loop of the application. Sets up Connections, 
 loops send and receive blocks.
******************************************************************************/
int main(int argc, char* argv[])
{

    uint32_t ID = 0;
    
    /* Setup connection */
    printf("Connection setup\n");
    int         setup_sending_socket = 0, setup_receive_socket = 0;
    uint16_t    portsend = PORTSEND;
    uint16_t    portrecv = PORTRECV;
	setup_sending_socket = Get_UDP_SendSocket(),
    FillDestinationPortInfo(SendtoUDPSocket_IP, portsend);
	setup_receive_socket = Get_UDP_RecvSocket(ReceiveUDPSocket_IP, portrecv);
    printf("sockets: send - %d , receive - %d\n", setup_sending_socket, setup_receive_socket);
	char bufferrecv[MAXLINE];
	char buffersend[sizeof(payload_send_msg)];

	int Received_Byte_Size;

    // int j = 0;
    int Received_ID = 0;

    /***** Logging - Create file and open it **********************/
    int logging = 1;
    FILE *fpt;
    char filepath[] = "/home/joy/Documents/HapticTestbedLinux/Working_Testbed/Data_and_Plots/Timing_HD_decoy.csv";

    if(fopen(filepath, "w+") == NULL ) {
        fprintf(stderr, "Couldn't open file to write: Provide a proper file path\n");
        logging = 0;
    }
    else {
        fpt = fopen(filepath, "w+");
        fprintf(fpt, "ID, Received_ID, Timer_Start(us), Timer_Loop(us), Time_to_receive(us), Data Sent Z\n");
    }
    /***** Logging - Create file and open it **********************/

    /* Run the main loop. */
    while(1)
    {
        // Start stopwatch
        Timer_Start = s_clock();
    
        /************** Send data **********************/
        payload_send_msg.IDs = ID;
        payload_send_msg.moveDXs = 47;
        payload_send_msg.moveDYs = 77;
        payload_send_msg.moveDZs = 147.87;

        // for (int i=0; i<16; i++)
        // {
        //     payload_send_msg.send[i] = 90.27 + j;
        //     j += 1;
        // }
                    
        memcpy(buffersend, &payload_send_msg, sizeof(payload_send));
        payload_send *psend = (payload_send*) buffersend;

        printf("Data sent: %f %f %f - %u\n",
            psend->moveDXs,
            psend->moveDYs,
            psend->moveDZs,
            psend->IDs);

        send_payload(psend, sizeof(payload_send));

        ID += 1;
        /************** Send data **********************/

        /************** Receive data **********************/
        //sleep(0.1);

        if (Enable_Haptics == 1)
        {

            Received_Byte_Size              = recv_payload(bufferrecv);
            bufferrecv[Received_Byte_Size]  = '\0';
            printf("Received bytes from Client: %d\n", Received_Byte_Size);

            payload_recv *precv             = (payload_recv*)bufferrecv;
            Time_to_receive                 = s_clock() - Timer_Start;
            printf("Received: ID_received=%d, PosDiff=%f\n", precv->IDr, precv->moveDXr);
            printf("Start_Time:%ld, Time to Receive: %ld us\n\n", Timer_Start, Time_to_receive);
            Received_ID                     = precv->IDr;
            // clear the buffer by filling null, it might have previously received data
            memset(bufferrecv,'\0', MAXLINE);

        }
        Timer_Loop = s_clock() - Timer_Start;
        if (logging == 1){
            fprintf(fpt, "%d, %d, %ld, %ld, %ld, %f\n",  psend->IDs, Received_ID, Timer_Start, Timer_Loop, Time_to_receive, psend->moveDZs);
        }

        /********* Receive data ****************************/
        // s_sleep(100);     
    }

    /* Close the file */
    if (logging = 1){
        fclose(fpt);
    }

    /* Terminate the network socket connections */
    printf("Connection terminate\n");
    closeSocket(setup_receive_socket);
    return 0;
}   
