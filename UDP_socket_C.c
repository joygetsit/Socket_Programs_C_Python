// Feedback on-1/off-0
#define Enable_Haptics 1

// Include the header file for Socket Communication
#include "comms.h"
#define PORTSEND                18007
#define PORTRECV                18009
#define SendtoUDPSocket_IP      "127.0.0.1" //"10.114.64.87" //"10.0.0.3" //
#define ReceiveUDPSocket_IP     "127.0.0.1" //"10.221.19.64" //"10.0.0.4" //

char filepath[] = "Timing_HD_decoy.csv";

// Define clock timestamp
int64_t Timer_Start = 0, Time_to_receive = 0, Timer_Loop = 0;

/* Data structures to send and receive */
typedef struct payloadrecv_t {
    uint32_t    rID;
    float       rPosition[3];
    float       r1Data;
    double      receive[4];
} payload_recv;

typedef struct payloadsend_t {
    uint32_t    IDs;
    float       Position;
    float       Data1;
    double      send[8];
} payload_send;
/* Data structures to send and receive */

static payload_send payload_send_msg;

/******************************************************************************
 Runs the main loop of the application. Sets up connections,
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
    printf("sockets: send - %d , receive - %d\n",
                setup_sending_socket, setup_receive_socket);
    char bufferrecv[MAXLINE];
    char buffersend[sizeof(payload_send_msg)];
    int Received_Byte_Size;

    int Received_ID = 0;

    /************ Logging - Create file and open it ***************/
    int logging = 1;
    FILE *fpt;
    if (fopen(filepath, "w+") == NULL) {
        fprintf(stderr, "Couldn't open file to write: Provide a proper file path\n");
        logging = 0;
    }
    else {
        fpt = fopen(filepath, "w+");
        fprintf(fpt, "ID, Received_ID, Timer_Start(us), Timer_Loop(us),"
                    "Time_to_receive(us), Data Sent Z\n");
    }
    /************ Logging - Create file and open it ***************/

    /* Run the main loop. */
    while(1)
    {
        // Start stopwatch
        Timer_Start = s_clock();

        /************** Send data **********************/
        payload_send_msg.IDs = ID;
        payload_send_msg.Position   = 10;
        payload_send_msg.Data1      = 11;
        payload_send_msg.send[0]    = 12.50;
        payload_send_msg.send[1]    = 12.55;
        payload_send_msg.send[3]    = 12.60;

        memcpy(buffersend, &payload_send_msg, sizeof(payload_send));
        payload_send *psend = (payload_send*) buffersend;

        send_payload(psend, sizeof(payload_send));
        ID += 1;

        printf("TX: [ID: %9.0u] - [%5.3f] [%f] [%5.3lf %5.3lf %5.3lf %5.3lf] \n",
            psend->IDs,
            psend->Position,
            psend->Data1,
            psend->send[0], psend->send[1], psend->send[2], psend->send[3]);
        /************** Send data **********************/

        /************** Receive data **********************/
        if (Enable_Haptics == 1)
        {
            Time_to_receive                 = s_clock() - Timer_Start;
            Received_Byte_Size              = recv_payload(bufferrecv);
            bufferrecv[Received_Byte_Size]  = '\0';
            payload_recv *precv             = (payload_recv*)bufferrecv;
            Received_ID                     = precv->rID;
            // clear the buffer by filling null,
            // it might have previously received data
            memset(bufferrecv,'\0', MAXLINE);
            printf("Received bytes from Client: %d\n", Received_Byte_Size);
            printf("RX [rID,rPosition3,r1Data1,receive4]:"
                        "[%d, (%f %f %f), %f, (%lf %lf %lf %lf %lf %lf %lf %lf)]\n",
                            precv->rID,
                            precv->rPosition[0], precv->rPosition[1], precv->rPosition[2],
                            precv->r1Data,
                            precv->receive[0],
                            precv->receive[1],
                            precv->receive[2],
                            precv->receive[3],
                            precv->receive[4],
                            precv->receive[5],
                            precv->receive[6],
                            precv->receive[7]
                            );
            printf("StartTime, Time to Receive: [%ld, %ld] us\n\n",
                            Timer_Start, Time_to_receive);
        }

        Timer_Loop = s_clock() - Timer_Start;
        if (logging == 1){
            fprintf(fpt, "%d, %d, %ld, %ld, %ld, %f\n",
                psend->IDs, Received_ID, Timer_Start, Timer_Loop, Time_to_receive,
                psend->Position);
        }
        /********* Receive data ****************************/
        // s_sleep(100);
    }

    /* close the file */
    if (logging = 1){
        fclose(fpt);
    }

    /* Terminate the network socket connections */
    printf("Terminate connection \n");
    closeSocket(setup_receive_socket);
    return 0;
}
