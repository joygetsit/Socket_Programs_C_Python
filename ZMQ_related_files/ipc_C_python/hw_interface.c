// For printf()
#include <stdio.h>
// For EXIT_*
#include <stdlib.h>
// For memcpy()
#include <string.h>
// For sleep()
#include <unistd.h>

#include <zmq.h>

// #include "libfancyhw.h"

// https://opensource.com/article/20/3/zeromq-c-python

int main(void)
{
    // const unsigned int INIT_PARAM = 12345;
    const unsigned int REPETITIONS = 10;
    const unsigned int PACKET_SIZE = 3;
    const char *TOPIC = "Haptic_Pos_data";

    void *context = zmq_ctx_new();

    if (!context)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    void *data_socket = zmq_socket(context, ZMQ_PUB);
    
    // Make a sub client
    void *data_socket_recv = zmq_socket(context, ZMQ_SUB);
    int sock_recv_connect = zmq_connect(data_socket_recv, "tcp://127.0.0.1:7000");
    if (sock_recv_connect != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new()_recv2: %s\n", zmq_strerror(errno));
	printf("%s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }
    // Subscribe to everything
    int zzm = zmq_setsockopt(data_socket_recv, ZMQ_SUBSCRIBE, "", 0);
    if (zzm != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new()_recv: %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }
    //

    const int rb = zmq_bind(data_socket, "tcp://*:5555");

    if (rb != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    const size_t topic_size = strlen(TOPIC);
    const size_t envelope_size = topic_size + 1 + PACKET_SIZE * sizeof(double);


    printf("Topic: %s; topic size: %zu; Envelope size: %zu\n", TOPIC, topic_size, envelope_size);

    for (unsigned int i = 0; i < REPETITIONS; i++)
    {
        float buffer[PACKET_SIZE];

        for (unsigned int j = 0; j < PACKET_SIZE; j++)
        {
            double id[3] = {3.8271033,45.8201,39.9198311};
            buffer[j] = id[j];
        }

        printf("Read %u data values\n", PACKET_SIZE);
	printf("%lf %lf %lf\n", buffer[0], buffer[1], buffer[2]);

        zmq_msg_t envelope;
   
        const int rmi = zmq_msg_init_size(&envelope, envelope_size);
        if (rmi != 0)
        {
            printf("ERROR: ZeroMQ error occurred during zmq_msg_init_size(): %s\n", zmq_strerror(errno));
   
            zmq_msg_close(&envelope);
   
            break;
        }
       
        memcpy(zmq_msg_data(&envelope), TOPIC, topic_size);

        memcpy((void*)((char*)zmq_msg_data(&envelope) + topic_size), " ", 1);

        memcpy((void*)((char*)zmq_msg_data(&envelope) + 1 + topic_size), buffer, PACKET_SIZE * sizeof(int16_t));
   
        const size_t rs = zmq_msg_send(&envelope, data_socket, 0);
        if (rs != envelope_size)
        {
            printf("ERROR: ZeroMQ error occurred during zmq_msg_send(): %s\n", zmq_strerror(errno));
   
            zmq_msg_close(&envelope);
   
            break;
        }
   
        zmq_msg_close(&envelope);
        printf("Message sent; i: %u, topic: %s\n", i, TOPIC);
	
        // Now listen to incoming messages from python instance for posDiff
	sleep(1);
	printf("Hello2\n");
        // Define a variable where you will store incoming data
        //char string[256] = "";
        //zmq_msg_t in_msg;
        //zmq_msg_init(&in_msg);
        printf("Hello1\n");
        //int size = zmq_recv(data_socket_recv, string, 255, 0);
	//printf("%d", size);
	//string[size < 256 ? size : 256-1] = '\0';
	//char *in_data = strdup(string);
	//printf("%s", string);
	//free(in_data);
	/*
        int size = zmq_msg_size (&in_msg);
        memcpy(string, zmq_msg_data(&in_msg), size);
        zmq_msg_close(&in_msg);
        string[size] = 0;
        printf("%s\n", string); 
	*/       
        //
        sleep(1);
    }

    // Close the PUB client
    const int rc = zmq_close(data_socket);

    if (rc != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_close(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    // Close the SUB client at the end
    zmq_close (data_socket_recv);
    //

    const int rd = zmq_ctx_destroy(context);

    if (rd != 0)
    {
        printf("Error occurred during zmq_ctx_destroy(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
