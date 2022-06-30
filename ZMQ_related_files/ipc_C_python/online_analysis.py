#! /usr/bin/env python3

import zmq
import struct

topic = "Haptic_Pos_data".encode('ascii')

print("Reading messages with topic: {}".format(topic))

with zmq.Context() as context:
    # Step 1. Receive Haptic data from C program
    socket = context.socket(zmq.SUB)
    socket.connect("tcp://127.0.0.1:5555")
    socket.setsockopt(zmq.SUBSCRIBE, topic)


    # Step 2. Send back Position error to Daptic Device
    #maybe you have to encode topic as above
    socketsendH_C = context.socket(zmq.PUB)
    socketsendH_C.bind("tcp://127.0.0.1:7000")

    ###

    i = 0

    try:
        while True:
            print("Haha3")
            binary_topic, data_buffer = socket.recv().split(b' ', 1)
            print("Haha4")
            topic = binary_topic.decode(encoding='ascii')

            print("Message {:d}:".format(i))
            print("\ttopic: '{}'".format(topic))



            packet_size = len(data_buffer) // struct.calcsize("h")

            print("\tpacket size: {:d}".format(packet_size))

            struct_format = "{:d}h".format(packet_size)

            data = struct.unpack(struct_format, data_buffer)

            # Sending back error data to Haptic Device
            socketsendH_C.send_string("This is not so bad")

            print("\tdata: {}".format(data))

            i += 1

    except KeyboardInterrupt:
        socket.close()
	socketsendH_C.close()
    except Exception as error:
        print("ERROR: {}".format(error))
        socket.close()
	socketsendH_C.close()	
