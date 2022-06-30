
import zmq
import time
from random import randrange
import numpy as np

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:5556")

count = 1

while count < 10:
    count += 1
    #msg = np.random.randint(1,5,(1,3))
    #topic = 5667
    #socket.send_string("%s" % topic)
    
    time.sleep(1)
    r = [45.829,28.1092,7.00183]
    listtostr = ' '.join(map(str,r))
    socket.send_string("%s" % listtostr)
    #msg = "HEllo"
    #print("%d %s" % (topic, msg))
    # msg = msg.tobytes()
    #zipcode = randrange(1, 100000)
    #temperature = randrange(-80, 135)
    #relhumidity = randrange(10, 60)
    #socket.send("%d %s" % (topic, msg))
    #time.sleep(1)
    #print(msg)
    # socket.send_string("%i %i %i" % (zipcode, temperature, relhumidity))
socket.close()
context.term()
