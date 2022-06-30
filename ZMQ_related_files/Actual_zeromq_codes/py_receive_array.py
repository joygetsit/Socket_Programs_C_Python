#
#   Client
#   Connects SUB socket to tcp://localhost:5556
#   Collects weather updates and finds avg temp in zipcode
#

import sys
import zmq
import numpy as np


#  Socket to talk to server
context = zmq.Context()
socket = context.socket(zmq.SUB)

print("Collecting updates from weather server...")
socket.connect("tcp://127.0.0.1:5556")

# Subscribe to zipcode, default is NYC, 10001
# zip_filter = sys.argv[1] if len(sys.argv) > 1 else "10001"

# Python 2 - ascii bytes to unicode str
#if isinstance(zip_filter, bytes):
#    zip_filter = zip_filter.decode('ascii')
socket.subscribe("")
print("Cnnec success")
# Process 5 updates
total_temp = 0
#for update_nbr in range(5):
while True:
   # msgr = socket.recv(flags=0,copy=True,track=False)

    #msg = socket.recv_string()
    #print("Received string: %s" % msg)
    try:
        msg2 = socket.recv_string()
        #msg2 = list(msg2.split(" "))
        print("Received string2: %s" % msg2)
        #print(msg2[1])

	#topic, msg = msgr.split()
	#print(msg)
	# msgre = np.frombuffer(msgr, dtype='int32')
	# print(msgre)
	#string = socket.recv_string()
	#zipcode, temperature, relhumidity = string.split()
	#print(temperature)
	#total_temp += int(temperature)

    except KeyboardInterrupt:
        socket.close()
        context.term()
#print("Average temperature for zipcode '%s' was %dF" % (
#      zip_filter, total_temp / (update_nbr+1)))

