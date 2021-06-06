#!/usr/bin/env python3

# Input haptic pos
# Set position of dobot
# Get position of dobot and calculate Position difference
# Output posDiff

# ss -tulpn -u -n -p to see udp buffer

enable_haptics = True

import sys
import math
import time
from ctypes import *
import socket
# Use select for efficient I/O non-blocking operation socket programming
import select

import timeit
# starttime1 = timeit.default_timer()
# print("The time difference is :", timeit.default_timer() - starttime1)

""" Networking """
# Create UDP socket for sending/publishing posDiff
UDPsocketPUB = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Define destination address and port
destIP = "127.0.0.1"
destPort = 18009
destAddress = ((destIP, destPort))

# Create UDP socket for receiving Haptic position input
# from the other side
UDPsocketSUB = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPsocketSUB.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF, 1024)
rcvbuf = UDPsocketSUB.getsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF)  #212992
UDPsocketSUB.settimeout(0.001)
print("rcvbufsize: %s \n" % rcvbuf)
print("Collecting position data from haptic device...")
print("Python UDP sockets: receive ", UDPsocketSUB, "send", UDPsocketPUB)

# Define address to receive at and bind to it
localIP = "127.0.0.1"
localPort = 18007
UDPsocketSUB.bind((localIP, localPort))

# Sockets from which we expect to read
inputs = [UDPsocketSUB]
# Sockets to which we expect to write
outputs = [UDPsocketPUB]
# Sockets Error, usually left empty
is_error = []

""" This class defines a C-like struct """
# For sending
class Payload_send(Structure):
    _fields_ = [("MsgIDfeedback", c_uint32),
                ("PosDiff", c_float)]

# For receiving
class Payload_recv(Structure):
    _fields_ = [("IDs", c_uint32),
                ("moveDXs", c_float),
                ("moveDYs", c_float),
                ("moveDZs", c_float)]

def GetPose():
    pose = device.pose()
    # xD = pose[0]
    # yD = pose[1]
    # zD = pose[2]
    # rHead = pose[3]
    # #position = [xD, yD, zD, rHead]
    # print(f"[%.3f,%.3f,%.3f,%.3f]\n" % (xD, yD, zD, rHead))
    # 175 0 75 0 - Home Position
    return pose

def cleanUpComms():
    status = UDPsocketSUB.close()

def cleanup():
    #device.close()
    sys.exit(0)

""" Script to drive robot 
with feedback by reading messages
or without feedback by not reading messages"""
def PhaseI():
    try:
        while True:
            # Wait for at least one of the sockets to be ready for processing
            readable, writable, exceptional = select.select(inputs, outputs, is_error)
            starttime1 = timeit.default_timer()

            if readable:
                print("Readable socket ..... Reading")
                # Get Haptic position data
                bytesAddressPair = UDPsocketSUB.recvfrom(sizeof(Payload_recv))

                # UDP packet contains message and address from which data received
                messagerecv = bytesAddressPair[0]  # type - 'bytes'
                fromaddress = bytesAddressPair[1]

                # or you can use shorthand
                # messagerecv, fromaddress = UDPsocketSUB.recvfrom(bufferSize)

                payload_in_msg = Payload_recv.from_buffer_copy(messagerecv)
                print("Received - ID=%d, (%f,%f,%f)" % (payload_in_msg.IDs,
                                                        payload_in_msg.moveDXs,
                                                        payload_in_msg.moveDYs,
                                                        payload_in_msg.moveDZs))

                if enable_haptics == True:
                    # PosDiff = 58
                    # msg_ID = payload_in_msg.IDs
                    # PosDiff_b = bytes(str(PosDiff), 'utf-8')
                    # feedback_data = (msg_ID, PosDiff)
                    # feedback_data = ",",join(feedback_data)
                    # feedback_data_b = bytes(feedback_data, 'utf-8')
                    msgnewpayload_out = Payload_send(payload_in_msg.IDs, 58)
                    # socketPUB.send_string("%s %s %s" % (topicPUB, msg_ID, PosDiff))
                    #socketPUB.sendto(feedback_data_b, destAddress)
                    # print("Sending data back")
                    UDPsocketPUB.sendto(msgnewpayload_out, destAddress)
    except KeyboardInterrupt:
        print("Exiting0")
        cleanUpComms()
        cleanup()

if __name__ == "__main__":
    PhaseI()
