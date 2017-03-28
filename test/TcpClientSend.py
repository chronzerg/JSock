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
    display("connected to " + str(address) + '\n')
    while True:
        message = sys.stdin.readline()
        size = conn.send(message)
        if not size:
            display("disconnected from " + str(address) + '\n')
            break
