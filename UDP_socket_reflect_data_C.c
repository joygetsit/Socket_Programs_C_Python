// build command
// gcc C_other_side.c comms.c -o C_other_side

// Feedback on/off
#define Enable_Haptics 1 //1 - for enable, 0 - for disable

// Include the header file for Socket Communication
#include "comms.h"
#define PORTSEND 18009
#define PORTRECV 18007
#define SendtoUDPSocket_IP "127.0.0.1"//"10.0.0.4"//
#define ReceiveUDPSocket_IP "127.0.0.1"//"10.0.0.3"//

// Define clock timestamp
int64_t start_time = 0, send_time = 0, receive_time = 0;

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
uint32_t temp_ID;
float tempx = 0, tempy = 0, tempz = 0;
// double tempdouble[16];

/******************************************************************************
 Runs the main loop of the application. Sets up Connections, 
 loops send and receive blocks.
******************************************************************************/
int main(int argc, char* argv[])
{

    uint32_t ID = 0;
   
    /* Setup connection */
    printf("Connection setup\n");
    int setup_sending_socket =0, setup_receive_socket = 0;
    uint16_t portsend = PORTSEND;
    uint16_t portrecv = PORTRECV;
	setup_sending_socket = Get_UDP_SendSocket();
    FillDestinationPortInfo(SendtoUDPSocket_IP, portsend);
	setup_receive_socket = Get_UDP_RecvSocket(ReceiveUDPSocket_IP, portrecv);
    printf("sockets: %d , %d\n", setup_sending_socket, setup_receive_socket);
	char bufferrecv[MAXLINE];
	char buffersend[sizeof(payload_send_msg)];

	int Received_Byte_Size;

    /* Run the main loop. */
    while(1)
    {

        /************** Receive data **********************/
        // sleep(2);

        //Start Time
        start_time = s_clock();

        if (Enable_Haptics == 1)
        {
            Received_Byte_Size = recv_payload(bufferrecv);
            bufferrecv[Received_Byte_Size] = '\0';
            printf("Received bytes from Client: %d\n", Received_Byte_Size);

            payload_recv *precv = (payload_recv*)bufferrecv;

            // printf("Received: ID_received=%d, PosDiff=%f\n\n",
            //         precv->MsgIDfeedback, precv->PosDiff);
            printf("Received: ID_received=%d, PosDiff=%f\n",
                    precv->IDr, precv->moveDXr);
            temp_ID = precv->IDr;
            tempx = precv->moveDXr;
            tempy = precv->moveDYr;
            tempz = precv->moveDZr;                
        }

        /********* Receive data ****************************/

        /************** Send data **********************/

        payload_send_msg.IDs = temp_ID;//ID;
        payload_send_msg.moveDXs = tempx;
        payload_send_msg.moveDYs = tempy;
        payload_send_msg.moveDZs = tempz;
        // for(int i = 0; i<16; i++)
        // {
        //     payload_send_msg.send[i] = tempdouble[i];
        // }

        memcpy(buffersend, &payload_send_msg, sizeof(payload_send));
        payload_send *psend = (payload_send*) buffersend;

        printf("Data sent: %f %f %f - %u\n", psend->moveDXs,psend->moveDYs,psend->moveDZs, psend->IDs);

        send_payload(psend, sizeof(payload_send));

        // ID += 1;
        /************** Send data **********************/        

        receive_time = s_clock();
    	printf("Time Start & recv: %ld, %ld us\n", start_time, receive_time);
        printf("Time Difference: %ld us\n\n", (receive_time - start_time));
        // s_sleep(100);     
    }


    /* Terminate the network socket connections */
    printf("Connection terminate\n");
    closeSocket(setup_receive_socket);
    return 0;
}   
