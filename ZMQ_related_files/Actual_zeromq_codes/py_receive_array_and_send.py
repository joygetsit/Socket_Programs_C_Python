#
#   Client
#   Connects SUB socket to tcp://localhost:5556
#   Collects weather updates and finds avg temp in zipcode
#

import zmq
from random import randint

#  Socket to talk to server
context = zmq.Context()

##
socketsend = context.socket(zmq.PUB)
socketsend.bind("tcp://127.0.0.1:5557")
##

socket = context.socket(zmq.SUB)

print("Collecting updates from server...")
socket.connect("tcp://127.0.0.1:5556")

# Subscribe to zipcode, default is NYC, 10001
# zip_filter = sys.argv[1] if len(sys.argv) > 1 else "10001"

# Python 2 - ascii bytes to unicode str
#if isinstance(zip_filter, bytes):
#    zip_filter = zip_filter.decode('ascii')
topicfilter = "10202"
# socket.subscribe("")
socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)
# socket.setsockopt(zmq.RCVTIMEO, 100)
print("Connection success")

while True:
   # msgr = socket.recv(flags=0,copy=True,track=False)

    #msg = socket.recv_string()
    #print("Received string: %s" % msg)
    try:
        msg2 = socket.recv_string()
        #print("Hellohere")
        #print(msg2)

        topic, msg_1, msg_2, msg_3 = list(msg2.split(" "))
        print("Received string2: %s %s %s %s" % (topic, msg_1, msg_2, msg_3))
        # print(msg_2)

        ##
        PosDiff = randint(0,10)
        topic = "10203"
        #k = 5
        # socketsend.send_string("This is not so bad")
        socketsend.send_string("%s %s" % (topic, PosDiff))

        ##
        # print("Hellohere3")

        #topic, msg = msgr.split()
        #print(msg)
        # msgre = np.frombuffer(msgr, dtype='int32')
        # print(msgre)
        #string = socket.recv_string()
        #zipcode, temperature, relhumidity = string.split()
        #print(temperature)
        #total_temp += int(temperature)

    except KeyboardInterrupt:
        socketsend.close()
        socket.close()
        context.term()
#print("Average temperature for zipcode '%s' was %dF" % (
#      zip_filter, total_temp / (update_nbr+1)))

