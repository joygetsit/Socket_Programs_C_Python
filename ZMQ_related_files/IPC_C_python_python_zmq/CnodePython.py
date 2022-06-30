#!/usr/bin/python3
#
# Receive data from haptic device and send to Dobot
# Get back Position Difference from Dobot and send to Haptic Device

import zmq
import time
from random import randint
import sys

#  Start ZMQ context
context = zmq.Context()

## Sending haptic data to Dobot
FWD_dobot = context.socket(zmq.PUB)
FWD_dobot.bind("tcp://127.0.0.1:5800")
topic_FWD_dobot = "10600"
##

## Feedback Position difference to Haptic Device
BCWD_Hap = context.socket(zmq.PUB)
BCWD_Hap.bind("tcp://127.0.0.1:5760")
#BCWD_Hap.bind("tcp://10.0.0.2:5760")
topic_BWD_Hap = "10010"
##

## Get back Position Difference from Dobot
BCWD_dobot = context.socket(zmq.SUB)
print("Collecting updates from server...")
BCWD_dobot.connect("tcp://127.0.0.1:5780")
topic_BCWD_dobot = "10400"
BCWD_dobot.setsockopt_string(zmq.SUBSCRIBE, topic_BCWD_dobot)


## Receive position data from Haptic Device
RECV_Hap = context.socket(zmq.SUB)
RECV_Hap.connect("tcp://127.0.0.1:5740")
#RECV_Hap.connect("tcp://10.0.0.1:5740")
topic_RECV_Hap = "10005"
# RECV_Hap.subscribe("")
RECV_Hap.setsockopt(zmq.RCVHWM, 20)
# RECV_Hap.setsockopt(zmq.RCVBUF, 20*1024)


RECV_Hap.setsockopt_string(zmq.SUBSCRIBE, topic_RECV_Hap)
##

# Python 2 - ascii bytes to unicode str
#if isinstance(zip_filter, bytes):
#    zip_filter = zip_filter.decode('ascii')

# socket.setsockopt(zmq.RCVTIMEO, 100)
print("All connections are successful")

# count = 1

while True:
#while count < 10000:
    # count += 1
    # msgr = socket.recv(flags=0,copy=True,track=False)

    #msg = socket.recv_string()
    #print("Received string: %s" % msg)
    try:
        # msg2 = RECV_Hap.recv(flags = zmq.NOBLOCK)
        [msg2, msg_ID, msg3] = RECV_Hap.recv_multipart()

        # print("[%s] %s" % (msg2, msg3))
        # [b'10005'] b'99.8101,59.17,46.16'
        # print(type(msg3)) # <class 'bytes'>
        msg3 = msg3.decode("utf-8")
        msg_ID = msg_ID.decode("utf-8")
        msg3 = msg3.split(",")
        msg3 = list(map(float, msg3))
        # print(sys.getsizeof(msg3))
        print("Message: %s ID: %s" % (msg3, msg_ID))
        # print(type(msg3))
        ##
        #time.sleep(0.1)
        #DecoyHapticData = randint(144, 150), randint(195, 205), randint(45, 55)
        # # print(DecoyHapticData)
        Position_Hap = ' '.join(map(str, msg3))
        FWD_dobot.send_string("%s %s %s" % (topic_FWD_dobot, msg_ID, Position_Hap))
        ##
        #time.sleep(0.1)
        #print(DecoyHapticData)

        #topic, msg_1, msg_2, msg_3 = list(msg2.split(" "))
        #print("Received string2: %s %s %s %s" % (topic, msg_1, msg_2, msg_3))
        ##
        #PosDiff = randint(0,10)

        # Receive position difference from Dobot
        ##
        Rec_from_Dobot = BCWD_dobot.recv_string()
        # print("Hellothere3")
        topic_from_dobot, msg_ID2, pos_difference = list(Rec_from_Dobot.split(" "))
        print("%s %s %s" % (topic_from_dobot, msg_ID2, pos_difference))
        # print(type(pos_difference)) # str
        ##
        # Convert string into bytes before sending in multipart frame
        pos_difference_b = bytes(pos_difference, 'utf-8')
        # print("hellothere2")
        msg_ID2_b = bytes(msg_ID2, 'utf-8')
        #k = 5
        #BCWD_Hap.send_multipart([b"10010", b"89.114"])
        topic_BWD_Hap_b = bytes(topic_BWD_Hap, 'utf-8')
        # print("Sent\n")
        #time.sleep(0.1)
        BCWD_Hap.send_multipart([topic_BWD_Hap_b, msg_ID2_b, pos_difference_b], zmq.NOBLOCK)

        # print("Hellohere3")

        #topic, msg = msgr.split()
        #print(msg)
        # msgre = np.frombuffer(msgr, dtype='int32')
        # print(msgre)

    except Exception as error:
        print("ERROR: {}".format(error))
        FWD_dobot.close()
        BCWD_dobot.close()
        BCWD_Hap.close()
        RECV_Hap.close()
        context.term()
    except KeyboardInterrupt:
        FWD_dobot.close()
        BCWD_dobot.close()
        BCWD_Hap.close()
        RECV_Hap.close()
        context.term()
        sys.exit()
    except:
        pass

