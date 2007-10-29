#!/usr/bin/env python
#
#

import socket
import signal, os
import thread

HOST = ''
PORT = 8080

def sigint(signum, frame):
	s.close()
	print 'SIGINT received, socket closed.'
	exit

def handle_request(conn, addr):
	print 'Client connected', addr
	while 1:
		data = conn.recv(1024)
		if not data: break
		conn.send(data)
	conn.close()
	print 'Client disconnected', addr


signal.signal(signal.SIGINT, sigint);

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)
while 1:
	print 'Listening.'
	conn, addr = s.accept()
	thread.start_new_thread(handle_request, (conn, addr))
s.close();
