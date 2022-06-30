
import zmq
import time
from random import randint

context = zmq.Context()
SENDsocket = context.socket(zmq.PUB)
SENDsocket.bind("tcp://127.0.0.1:5800")
topicSEND = "10600"
##
RECVsocket = context.socket(zmq.SUB)
RECVsocket.connect("tcp://127.0.0.1:5780")
topicRECV = "10400"
# socketrec.subscribe("")
RECVsocket.setsockopt_string(zmq.SUBSCRIBE, topicRECV)
##

count = 1

while count < 1000:
    count += 1
    DecoyHapticData = randint(150,160), randint(170,180), randint(40,50)
    print(DecoyHapticData)
    #msg = np.random.randint(1,5,(1,3))

    time.sleep(1)
    # r = [45.829,28.1092,7.00183]
    listtostr = ' '.join(map(str,DecoyHapticData))
    SENDsocket.send_string("%s %s" % (topicSEND, listtostr))

    ##
    recmsg = RECVsocket.recv_string()
    topicr, msg_r = list(recmsg.split(" "))
    print("%s %s" % (topicr, msg_r))
    ##
    #print("hellothere2")


    #msg = "HEllo"
    #print("%d %s" % (topic, msg))
    # msg = msg.tobytes()
    #time.sleep(1)
    #print(msg)

SENDsocket.close()
RECVsocket.close()
context.term()

