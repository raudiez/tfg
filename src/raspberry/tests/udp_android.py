#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import re
import os
import time
import locale

locale.setlocale(locale.LC_TIME, "es_ES.utf8")

# UDP socket configuration
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 8080))

def main():
  # Loop forever, listening for requests:
  while True:
    data = time.strftime("%d/%b %H:%M:%S")+" Alerta:  ola k ase"
    req, client_address = sock.recvfrom(1024) # get the request, 1kB max
    print "Connection from: ", client_address[0]
    print req
    # Look in the first line of the request for a valid command
    # The command should be 'http://server/getAlert'
    match = re.match('GET /getAlert', req)
    if match:
      print "sending data..."
      sock.sendto(data,(client_address[0],8081))
    else:
      # If there was no recognised command then return a 404 (page not found)
      print "Returning 404"
      sock.sendto("HTTP/1.1 404 Not Found\r\n",(client_address,8081))


try:
  main()
  sock.close()
except (KeyboardInterrupt):
  sock.close()
  print "FIN."