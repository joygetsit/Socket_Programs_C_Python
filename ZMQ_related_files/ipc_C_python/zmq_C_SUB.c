// For printf()
#include <stdio.h>
// For EXIT_*
#include <stdlib.h>
// For memcpy()
#include <string.h>
// For sleep()
#include <unistd.h>

#include <zmq.h>

//  Receive 0MQ string from socket and convert into C string
static char *
s_recv (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    int size = zmq_msg_recv (&message, socket, 0);
    if (size == -1)
        return NULL;
    char *string = malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return (string);
}

int main(void)
{
    const unsigned int REPETITIONS = 10;
    const unsigned int PACKET_SIZE = 3;
    const char *TOPIC = "Haptic_Pos_data";
    printf("Creating context\n");
    // Create a new zmq context
    void *context = zmq_ctx_new();

    if (!context)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }
    printf("Creating subscribe client\n");
    // Make a sub client
    void *data_socket_recv = zmq_socket(context, ZMQ_SUB);
    int sock_recv_connect = zmq_connect(data_socket_recv, "tcp://127.0.0.1:5556");
    if (sock_recv_connect != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new()_recv2: %s\n", zmq_strerror(errno));
	printf("%s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }
    printf("%d\n", sock_recv_connect);

    printf("Subscribe start\n");

    // Subscribe to everything
    int zzm = zmq_setsockopt(data_socket_recv, ZMQ_SUBSCRIBE, "", 0);
    if (zzm != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new()_recv: %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }
    printf("Subscribe successfull %d\n", zzm);
    //
    
    printf("Start receiving\n");
    // Receive string and print it
    char *buffer = s_recv(data_socket_recv);
    printf("Data received\n");
    /*if (size == -1)
        return EXIT_FAILURE;
    if (size > 255)
        size = 255;
    buffer[size] = '\0';*/
    /* use strndup(buffer, sizeof(buffer)-1) in *nix */
    //return strdup (buffer);
    /*char* msg_rec = (char*)strndup(buffer, sizeof(buffer)-1);
    printf("Now print\n");
    printf("%s", msg_rec);*/

    // Close the SUB client at the end
    zmq_close (data_socket_recv);
    printf("Closing socket\n");
    //

    // Shutdown the zmq context
    const int rd = zmq_ctx_destroy(context);
    if (rd != 0)
    {
        printf("Error occurred during zmq_ctx_destroy(): %s\n", zmq_strerror(errno));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
