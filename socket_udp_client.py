"""
https://gist.github.com/marcom04/22860f1168330605cac3c448982b0393
Basic echo between Python client and C server via socket using Python ctypes
Use above link to clean code and modularize it
"""

import random
# Handles datatypes, encoding/decoding, conversion from/to bytes automatically
# to make easy task of marshalling data between C and Python
from ctypes import *

import time

# To use sys.getsizeof()
import sys

''' Communication setup '''
import socket

# Use select for efficient I/O non-blocking operation socket programming
import select

# Define sockets - here UDP receive socket and destination socket
ReceiveSocketIP = "127.0.0.1"
ReceiveSocketPort = 20006
SendToSocketIP = "127.0.0.1"
SendToSocketPort = 20011

# Define destination address and port i.e. address to send data to
AddressToSendTo = ((SendToSocketIP, SendToSocketPort))

# Create a UDP socket at client side to receive at
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Create a UDP socket to send data from
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind receive UDP socket, whose address is defined at the top, to receive data here
UDPClientSocket.bind((ReceiveSocketIP, ReceiveSocketPort))

# Sockets from which we expect to read
inputs = [UDPClientSocket]
# Sockets to which we expect to write
outputs = [UDPServerSocket]
# Error
is_error = []
# Outgoing message queues (socket:Queue)
message_queues = {}
''' Communication setup '''

print("UDP server up and listening")


""" This class defines a C-like struct """
# For sending
class Payload_send(Structure):
    _fields_ = [("IDrecv", c_uint32),
                ("PosDiff", c_float)]
# For receiving
class Payload_recv(Structure):
    _fields_ = [("IDrecv", c_uint32),
                ("X", c_float),
                ("Y", c_float),
                ("Z", c_float)]
""" This class defines a C-like struct """

i = 0

while True:
    # Wait for at least one of the sockets to be ready for processing
    readable, writable, exceptional = select.select(inputs, outputs, is_error)
    try:
        if readable:
            """ Receive block - Receive data at the bound socket data and 
            unpack into receiving data structure """
            bytesAddressPair = UDPClientSocket.recvfrom(sizeof(Payload_recv))

            # UDP packet contains message and address from which data received
            message = bytesAddressPair[0]  # type - 'bytes'
            address = bytesAddressPair[1]
            print("Size received: %d\n" % sys.getsizeof(message))
            # or you can use shorthand
            #### # Define buffer size
            #### bufferSize = 1024 #Should be defined at the top when used
            # message, address = UDPClientSocket.recvfrom(bufferSize)
            # print(message)

            """ Receive data from buffer and store it"""
            payload_in = Payload_recv.from_buffer_copy(message)
            print("Received ID=%d, X=%f, Y=%f, Z=%f" % (payload_in.IDrecv,
                                                        payload_in.X,
                                                        payload_in.Y,
                                                        payload_in.Z))
            # Converts bytes to string - "".format(data)
            clientIP = "Client IP Address:{}\n".format(address)
            print(clientIP)
            """ Receive block - Receive data at the bound socket data and 
            unpack into receiving data structure """

        else:
            """ Send block - Pack data into sending structure and send data through UDP """
            # Define data according to data structure defined for sending
            print("i: %d\n" % i)
            msgnewpayload_out = Payload_send(i, 35.8211)
            UDPServerSocket.sendto(msgnewpayload_out, AddressToSendTo)
            i += 1
            time.sleep(1)
            """ Send block - Pack data into sending structure and send data through UDP """
    except KeyboardInterrupt:
        UDPClientSocket.close()
        sys.exit(0)
