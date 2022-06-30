#!/usr/bin/env python3

import sys  # To use sys.getsizeof()
import communication_python

# Handles datatypes, encoding/decoding, conversion from/to bytes automatically
# to make easy task of marshalling data between C and Python
from ctypes import *

import pandas
# Use select for efficient I/O non-blocking operation socket programming
import select

import timeit
# starttime1 = timeit.default_timer()
# print("The time difference is :", timeit.default_timer() - starttime1)

enable_haptics = True
path_to_write = '/home/joy/Documents/HapticTestbedLinux/Working_Testbed/Data_and_Plots/Timing_Vrep_side.csv'

""" Networking """
# Define sockets - UDP receive socket and destination socket
ReceiveSocketIP = "127.0.0.1"
ReceiveSocketPort = 18007
SendToSocketIP = "127.0.0.1"
SendToSocketPort = 18009
AddressToSendTo = ((SendToSocketIP, SendToSocketPort))
AddressToRecvAt = ((ReceiveSocketIP, ReceiveSocketPort))

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

""" Initialize pandas file to write data to """
Time_Data = pandas.DataFrame([['ID',
                               ' Timer_Start(s)',
                               ' Timer_Loop(s)',
                               ' MoveX']])
Time_Data.to_csv(path_to_write, mode='w', header=False, index=False)

""" Script to drive robot with feedback by reading messages
or without feedback by not reading messages"""
def PhaseI(commParameters):
    socketsend = commParameters[0]
    socketrecv = commParameters[1]
    outputs = commParameters[2]
    inputs = commParameters[3]
    is_error = commParameters[4]
    try:
        while True:
            # global ID, Fx
            ID, Timer_Start, Timer_Loop, Fx, Fy, Fz = 0,0,0,0,0,0
            Timer_Start = timeit.default_timer()

            # Wait for at least one of the sockets to be ready for processing
            readable, writable, exceptional = select.select(inputs, outputs, is_error)
            if readable:
                """ Receive block - Receive data at the bound socket data and 
                unpack into receiving data structure """

                # print("Readable socket ..... Reading")
                # Get Haptic position data # Received messagerecv in bytes
                # UDP packet contains message and address from which data received
                messagerecv, fromaddress = socketrecv.recvfrom(sizeof(Payload_recv))
                # print("Size received: %d\n" % sys.getsizeof(message))
                payload_in_msg = Payload_recv.from_buffer_copy(messagerecv)
                print("Received - ID=%d, (%f,%f,%f)" % (payload_in_msg.IDs,
                                                        payload_in_msg.moveDXs,
                                                        payload_in_msg.moveDYs,
                                                        payload_in_msg.moveDZs))
                ID = payload_in_msg.IDs
                Fx = payload_in_msg.moveDXs

                """ Send block - Pack data into sending structure and send data through UDP """
                if enable_haptics:
                    # PosDiff = 58
                    # msg_ID = payload_in_msg.IDs
                    # PosDiff_b = bytes(str(PosDiff), 'utf-8')
                    # feedback_data = (msg_ID, PosDiff)
                    # feedback_data = ",",join(feedback_data)
                    # feedback_data_b = bytes(feedback_data, 'utf-8')
                    msgnewpayload_out = Payload_send(payload_in_msg.IDs, 58)
                    # socketPUB.send_string("%s %s %s" % (topicPUB, msg_ID, PosDiff))
                    #socketPUB.sendto(feedback_data_b, AddressToSendTo)
                    # print("Sending data back")
                    socketsend.sendto(msgnewpayload_out, AddressToSendTo)

            Timer_Loop = timeit.default_timer() - Timer_Start
            Data_array = [ID, Timer_Start, Timer_Loop, Fx]
            Time_Data = pandas.DataFrame([Data_array])
            Time_Data.to_csv(path_to_write, mode='a', header=False, index=False)
    except KeyboardInterrupt:
        print("\nExiting.....")
        communication_python.cleanUpComms(socketrecv)
        sys.exit(0)


if __name__ == "__main__":
    commParameters = communication_python.Comm(AddressToRecvAt, AddressToSendTo)
    PhaseI(commParameters)
