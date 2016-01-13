#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Main program of the alert system.
"""

import locale
import os
import re
import RPi.GPIO as GPIO
import socket
import sys
import time

from libs.hue import Bridge

##########################
# Configurable zone:
USERNAME = 'hueusertest'
HUE_IP = '192.168.2.121'
DING=17
INTERCOM=18
# End of configurable zone.
###########################

locale.setlocale(locale.LC_TIME, "es_ES.utf8")
bridge = Bridge(device={'ip':HUE_IP}, user={'name':USERNAME})
#GPIO setup:
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(DING,GPIO.IN)
GPIO.setup(INTERCOM,GPIO.IN)
#Used colors:
#RED=0
#YELLOW=12750
GREEN=25500
BLUE=46920

# UDP socket configuration
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 8080))

def linkUserConfig():
  created = False
  print 'Press the button on the Hue bridge'
  while not created:
    response = bridge.config.createUser(username)['resource']
    if 'error' in response[0]:
      if response[0]['error']['type'] != 101:
        print 'Unhandled error creating configuration on the Hue'
        sys.exit(response)
    else:
      created = True

def dingAlert():
  numlights = bridge.light.getNumLights()
  oldstate = dict()
  for i in range(1,numlights+1):
    oldstate[i] = bridge.light.getLightState(i)
  for i in range(1,4):
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightState(j, True, 254, BLUE, 254)
    time.sleep(1)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightColor(j, oldstate[j]['bri'], oldstate[j]['hue'], oldstate[j]['sat'])
    time.sleep(0.3)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j) and not oldstate[j]['on']:
        bridge.light.setLightOff(j)
    time.sleep(1)

def intercomAlert():
  numlights = bridge.light.getNumLights()
  oldstate = dict()
  for i in range(1,numlights+1):
    oldstate[i] = bridge.light.getLightState(i)
  for i in range(1,4):
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightState(j, True, 254, GREEN, 254)
    time.sleep(1)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightColor(j, oldstate[j]['bri'], oldstate[j]['hue'], oldstate[j]['sat'])
    time.sleep(0.3)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j) and not oldstate[j]['on']:
        bridge.light.setLightOff(j)
    time.sleep(1)

def sendAlertToAndroid(alert_str, client_address):
  data = time.strftime("%d/%b %H:%M:%S")+" - "+alert_str
  print "Connection from: ", client_address[0]
  print req
  # Look in the first line of the request for a valid command
  # The command should be 'http://server/getAlert'
  match = re.match('GET /getAlert', req)
  if match:
    sock.sendto(data,(client_address[0],8081))
  else:
    # If there was no recognised command then return a 404 (page not found)
    print "Returning 404"
    sock.sendto("HTTP/1.1 404 Not Found\r\n",(client_address,8081))

def main():
  while True:
    if not bridge.config.isConnected():
      print 'Unauthorized user'
      linkUserConfig()
    req, client_address = sock.recvfrom(1024) # get the request, 1kB max
    if not GPIO.input(INTERCOM):
      cad = "Intercom alert."
      print cad
      sendAlertToAndroid(cad,client_address)
      intercomAlert()

    if not GPIO.input(DING):
      cad = "Ding alert."
      print cad
      sendAlertToAndroid(cad,client_address)
      dingAlert()

    bridge.light.findNewLights()


try:
  main()
except (KeyboardInterrupt):
  print "Program ended by user."
  sock.close()
  GPIO.cleanup()