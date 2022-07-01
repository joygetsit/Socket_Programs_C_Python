/*************************************************************
Reflect back data from application.
Use this as a decoy to test proper functionality.
*************************************************************/

// Feedback on(1)/off(0)
#define Enable_Haptics 1

// Include the header file for Socket Communication
#include "comms.h"
#define PORTSEND 18007
#define PORTRECV 18009
#define SendtoUDPSocket_IP "127.0.0.1"//"10.0.0.4"//
#define ReceiveUDPSocket_IP "127.0.0.1"//"10.0.0.3"//
int     setup_sending_socket;
int     setup_receive_socket;

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

static  payload_send payload_send_msg;
char    bufferrecv[MAXLINE];
char    buffersend[sizeof(payload_send_msg)];
int     receivedbytesize = 0;
double  temp_copy[16];

/* Define clock timestamp */
int64_t Timer_Start     = 0,
        Timer_Loop      = 0,
        Time_to_receive = 0;

float tempx = 0, tempy = 0, tempz = 0;
// double tempdouble[16];

/******************************************************************************
 Runs the main loop of the application. Sets up Connections, 
 loops send and receive blocks.
******************************************************************************/
int main(int argc, char* argv[])
{

    uint32_t ID = 0;
    uint32_t temp_ID;

    /* Communication setup */
    setup_sending_socket = 0, setup_receive_socket = 0;
	setup_sending_socket = Get_UDP_SendSocket();
    FillDestinationPortInfo(SendtoUDPSocket_IP, PORTSEND);
	setup_receive_socket = Get_UDP_RecvSocket(ReceiveUDPSocket_IP, PORTRECV);
    printf("sockets: %d , %d\n", setup_sending_socket, setup_receive_socket);

    /* Run the main loop. */
    while(1)
    {

        /************** Receive data **********************/
        // sleep(2);

        //Start Time
        start_time = s_clock();
        printf("Start Time: %ld us\n", Timer_Start);

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
//            for (int i = 0; i<16; i++)
//            {
//                temp_copy[i] = precv->receive[i];
//            }
//            printf("Received: ID_received=%d, Data=%f\n\n",
//                    precv->IDr, precv->receive[12]);
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
//        printf("Data sent: %f %f %f - %u\n", psend->send[12],psend->send[13],psend->send[14], psend->IDs);

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
