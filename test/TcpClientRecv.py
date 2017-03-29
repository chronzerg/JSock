#!/usr/bin/env python

import socket
import sys

def display(message):
    sys.stdout.write(message)
    sys.stdout.flush()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('127.0.0.1', 1234))
s.listen(0)

while True:
    conn, address = s.accept()
    message = "connected to " + str(address) + '\n'
    while True:
        display(message)
        message = conn.recv(4096)
        if not message:
            display("disconnected from " + str(address) + '\n')
            break
