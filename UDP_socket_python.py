#!/usr/bin/env python3

"""
Script with feedback (sending back messages) or without feedback
"""

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
import random

enable_haptics = True
path_to_write = 'Timing_Teleoperator.csv'
columns = ['ID', 'Timer_Start(s)', 'Timer_Loop(s)', 'MoveX']

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
    _fields_ = [("returnID",        c_uint32),
                ("returnPosition",  c_float * 3),
                ("DataR1",          c_float),
                ("DataR2",          c_double * 4)]
d = Payload_send()

# For receiving
class Payload_recv(Structure):
    _fields_ = [("IDs",         c_uint32),
                ("Position",    c_float),
                ("Data1",       c_float),
                ("Data2",       c_double * 8)]


""" Initialize pandas file to write data to """
Time_Data = pandas.DataFrame([columns])
Time_Data.to_csv(path_to_write, mode='w', header=False, index=False)


def PhaseI(params):
    socketsend, socketrecv, outputs, inputs, is_error = params
    try:
        while True:
            ID = position = 0
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
                ID = payload_in_msg.IDs
                position = payload_in_msg.Position

                """ Send block - Pack data into sending structure and send data through UDP """
                if enable_haptics:
                    # msgnewpayload_out = Payload_send(payload_in_msg.IDs, 60, 0, 60)
                    d.returnID = 34
                    d.returnPosition = type(d.returnPosition)(*[2, random.random(), 1.2])
                    d.DataR1 = 44.44
                    d.DataR2 = type(d.DataR2)(*[5, random.random(), 9.9, 1.2])
                    # msgnewpayload_out = Payload_send(ID, payload_in_msg.Data1, 4)
                    # msgnewpayload_out = d
                    socketsend.sendto(d, AddressToSendTo)

                    # Automatic print by reading buffer
                    # for field_name, field_type in d._fields_:
                    #     print(field_name, getattr(d, field_name))
                    # or
                    # Manual print by writing each field name
                    print(f'Received - ID = {payload_in_msg.IDs}')
                    # str = 'The {} are {}, {}, {}, and {}'.format('numbers', *payload_in_msg.Data2)
                    print(f'A: {payload_in_msg.Position}',
                          f'B: {payload_in_msg.Data1}')
                    print(*payload_in_msg.Data2)
                    print(f'TX back: {d.returnID} '
                          f'{d.returnPosition[1]} '
                          f'{d.DataR1}')

            Timer_Loop = timeit.default_timer() - Timer_Start
            Data_array = [ID, Timer_Start, Timer_Loop, position]
            Time_Data = pandas.DataFrame([Data_array])
            Time_Data.to_csv(path_to_write, mode='a', header=False, index=False)
            print("Time: ", Timer_Start, Timer_Loop*1000000, "\n")
    except KeyboardInterrupt:
        print("\nExiting.....")
        communication_python.cleanUpComms(socketrecv)
        sys.exit(0)


if __name__ == "__main__":
    commParameters = communication_python.Comm(AddressToRecvAt, AddressToSendTo)
    PhaseI(commParameters)
