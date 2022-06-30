#!/usr/bin/python3
#
#   Client
#   Connects SUB socket to tcp://localhost:5556
#

import zmq
import time
from random import randint
import sys
#  Socket to talk to server
context = zmq.Context()

##
socketsend = context.socket(zmq.PUB)
socketsend.bind("tcp://127.0.0.1:5621")
##

## Sending haptic data to Dobot
socketD = context.socket(zmq.PUB)
socketD.bind("tcp://127.0.0.1:5556")
#socketD.bind("tcp://10.0.0.2:5556")
##

socket = context.socket(zmq.SUB)
print("Collecting updates from server...")
socket.connect("tcp://127.0.0.1:5620")

## Receive position difference from Dobot
socketrecD = context.socket(zmq.SUB)
socketrec.connect("tcp://127.0.0.1:5557")
#socketrecD.connect("tcp://10.0.0.1:5557")
##

topicfilter2 = "10203"
# socketrec.subscribe("")
socketrecD.setsockopt_string(zmq.SUBSCRIBE, topicfilter2)
##
# Subscribe to zipcode, default is NYC, 10001
# zip_filter = sys.argv[1] if len(sys.argv) > 1 else "10001"

# Python 2 - ascii bytes to unicode str
#if isinstance(zip_filter, bytes):
#    zip_filter = zip_filter.decode('ascii')
topicfilter = "BA"
# socket.subscribe("")
socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)
# socket.setsockopt(zmq.RCVTIMEO, 100)
print("Connection success")

count = 1

#while True:
while count < 10000000:
   # msgr = socket.recv(flags=0,copy=True,track=False)

    #msg = socket.recv_string()
    #print("Received string: %s" % msg)
    try:
        # msg2 = socket.recv(flags = zmq.NOBLOCK)
	# msg2 = socket.recv_string(zmq.NOBLOCK)
        [msg2, msg3] = socket.recv_multipart()
        # print("Hellohere")
        # print("[%s] %s" % (msg2, msg3))
        # print(type(msg3))
        msg3 = msg3.decode("utf-8")
        msg3 = msg3.split(",")
        msg3 = list(map(float, msg3))
        #print(sys.getsizeof(msg3))
        #msg3 = msg3.decode()
        print(msg3)
        listtostr = ' '.join(map(str, msg3))
        topic = "10400"
        socketD.send_string("%s %s" % (topic, listtostr))
        #time.sleep(0.01)

        #topic, msg_1, msg_2, msg_3 = list(msg2.split(" "))
        #print("Received string2: %s %s %s %s" % (topic, msg_1, msg_2, msg_3))
        ##
        #PosDiff = randint(0,10)
        #PosDiffbytes = bytes(str(PosDiff), 'utf-8')

        # Receive position difference from Dobot
        topic = "10500"
        ##
        recmsg = socketrecD.recv_string()
        # print("Hellothere3")
        topicr, msg_r = list(recmsg.split(" "))
        # print(recmsg)
        print("%s %s\n" % (topicr, msg_r))
        printf(type(msg_r))
        ##
        # print("hellothere2")
        #k = 5
        #topic = "BC"
        #topicb = bytes(topic, 'utf-8')
        #topicb.decode()
        socketsend.send_multipart([b"10203", b"89.114"])
        # print("Sent\n")
        #time.sleep(0.01)
        # socketsend.send_string("This is not so bad")
        socketsend.send_multipart([topicb, PosDiffbytes])
        #socketsend.send_string("%s %s" % (topic, PosDiff),zmq.NOBLOCK)

        # print("Hellohere3")

        #topic, msg = msgr.split()
        #print(msg)
        # msgre = np.frombuffer(msgr, dtype='int32')
        # print(msgre)
        #string = socket.recv_string()
        #zipcode, temperature, relhumidity = string.split()
        #print(temperature)
        #total_temp += int(temperature)
    except Exception as error:
        print("ERROR: {}".format(error))
        socket.close()
        socketrecD.close()
        socketD.close()
        socketsend.close()
        context.term()
    except KeyboardInterrupt:
        socketsend.close()
        socket.close()
        socketrecD.close()
        socketD.close()
        context.term()
        sys.exit()
    except:
        pass

