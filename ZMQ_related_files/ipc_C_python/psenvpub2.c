//  Pubsub envelope publisher
//  Note that the zhelpers.h file also provides s_sendmore

// build command - clang -I. psenvpub2.c conio.c -lzmq -o psenvpub2

#include "zhelpers_modified.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "conio.h"

// For file creation
#include<string.h>

int main (void)
{
    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUB);
    zmq_bind (publisher, "tcp://127.0.0.1:5740");
    printf("Pub created\n");

    // Prepare subscriber
    void *subscriber2 = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber2, "tcp://localhost:5760");
    zmq_setsockopt (subscriber2, ZMQ_SUBSCRIBE, "10010", 5);
    printf("Sub2 created\n");

    // Define msg_ID
    unsigned long int ID = 0;
    char ID_str[16];

    // Create file and open it
    FILE *fpt;
    fpt = fopen("MyFile.csv", "w+");
    fprintf(fpt, "ID,contents,time,timeD\n");
    // Define clock timestamp
    int64_t start_time = s_clock(), send_time = 0, receive_time = 0;
    printf("%lu\n", start_time);

    while (!_kbhit()) {
	    // Write two messages, each with an envelope and content
	    // printf("Sending message\n");
        sprintf(ID_str, "%lu", ID);
        send_time = s_clock();
        s_sendmore (publisher, "A");
        s_send (publisher, "We don't want to see this");
        s_sendmore (publisher, "10005");
        s_sendmore (publisher, ID_str);
        s_send (publisher, "99.8101,59.17,46.16");
        printf("Send: ID: %lu @ %ld\n", ID, (send_time - start_time));
        fprintf(fpt, "%s,%ld\n",  ID_str, (send_time - start_time));

	    // printf("Sent message\n");

        //  Read envelope with address
        char *address2 = s_recv (subscriber2);
        char *msg_ID = s_recv (subscriber2);
        receive_time = s_clock();
	    // printf("receive message\n");
	    //  Read message contents
        char *contents2 = s_recv (subscriber2);
        printf("Received: ID: %s PosDiff: %s @ %ld TimeDifference: %ld\n", msg_ID, contents2, (receive_time - start_time), (receive_time - send_time));
        fprintf(fpt, "%s,%s,%ld,%ld\n",  msg_ID, contents2, (receive_time - start_time), (receive_time - send_time));
//        if (contents2!=NULL) {
//            float contents2_f = atof(contents2);
//            printf("%g\n", contents2_f);
//        }
        // printf ("[%s] %s\n", address2, contents2);
        // printf ("%s\n", contents2);
        free (address2);
        free (contents2);
        //s_sleep(1000);
        ID += 1;
    }
    fclose(fpt);
    //  We never get here, but clean up anyhow
    zmq_close (publisher);
    zmq_close (subscriber2);
    zmq_ctx_destroy (context);
    return 0;
}
