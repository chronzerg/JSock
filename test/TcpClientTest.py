#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('127.0.0.1', 5445))
s.listen(0)

while True:
    conn, address = s.accept()
    message = "connected to", address
    while True:
        print message
        message = conn.recv(4096)
        if not message:
            print "disconnected from", address
            break
