#! /usr/bin/env python3

import zmq
import struct

#topic = "Haptic_Pos_data".encode('ascii')

# print("Sending messages with topic: {}".format(topic))

with zmq.Context() as context:
    # Step 1. Receive Haptic data from C program

    # Step 2. Send back Position error to Daptic Device
    #maybe you have to encode topic as above
    socketsendH_C = context.socket(zmq.PUB)
    socketsendH_C.bind("tcp://*:5556")

    ###

    i = 10000

    try:
        while i > 0:
            i -= 1
            print("%d", i)
            # binary_topic, data_buffer = socket.recv().split(b' ', 1)
            #topic = binary_topic.decode(encoding='ascii')

            #print("Message {:d}:".format(i))
            #print("\ttopic: '{}'".format(topic))



            #packet_size = len(data_buffer) // struct.calcsize("h")

            #print("\tpacket size: {:d}".format(packet_size))

            #struct_format = "{:d}h".format(packet_size)

            #data = struct.unpack(struct_format, data_buffer)

            # Sending back error data to Haptic Device
            #sendmsg = "This is not so bad"
            #listtostr = "Hey"
            #topic = "10202"
            #socketsendH_C.send_string("%s %s" % (topic, listtostr))
            sendmsg = "This"
            socketsendH_C.send_string("%s\n" % sendmsg)
            #print(sendmsg)
            #print("\tdata: {}".format(data))
        socketsendH_C.close()
    except KeyboardInterrupt:
        socketsendH_C.close()
    except Exception as error:
        print("ERROR: {}".format(error))
        socketsendH_C.close()
