#!/usr/bin/env python3

import socket

def Comm(localAddress,destAddress):
    a, a1 = create_tx_socket()
    b, b1, b2 = create_rx_socket(localAddress)
    return a, b, a1, b1, b2

def create_tx_socket():
    # Create UDP socket for sending/publishing posDiff
    UDPSocketSend = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    print("Python UDP sockets: send", UDPSocketSend)
    # Sockets to which we expect to write
    outputs = [UDPSocketSend]
    return UDPSocketSend, outputs

def create_rx_socket(localAddress):
    # Create UDP socket for receiving Haptic position input from the other side
    UDPsocketRecv = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    UDPsocketRecv.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1024)
    rcvbuf = UDPsocketRecv.getsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF)  # 212992
    UDPsocketRecv.settimeout(0.001)
    print("Receive Buffer Size: %s \n" % rcvbuf)
    print("Python UDP sockets: receive ", UDPsocketRecv)
    UDPsocketRecv.bind(localAddress)
    # Sockets from which we expect to read
    inputs = [UDPsocketRecv]
    # Sockets Error, usually left empty
    is_error = []
    return UDPsocketRecv, inputs, is_error

def cleanUpComms(socket):
    # Close the receiving socket for graceful application exit
    status = socket.close()