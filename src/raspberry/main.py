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
import threading
import time

from libs.hue import Bridge

##########################
# Configurable zone:
USERNAME = 'ucahueuser'
DING=17
INTERCOM=18
LANG='es'
# End of configurable zone.
###########################

# GPIO setup:
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(DING,GPIO.IN)
GPIO.setup(INTERCOM,GPIO.IN)

# Used colors:
RED=0
BLUE=46920

# Global vars without initial value:
data=""
bridge=None
sock=None

# Command to discover Philips Hue bridge's IP.
cmd = 'repeat_scan=true ; while ($repeat_scan); do hue_ip=$(ip route show|grep \'src\'|egrep -o \'([0-9]{1,3}\.){3}[0-9]{1,3}/[0-9]{1,2}\'|xargs arp-scan|grep -i "Philips Lighting"|awk \'{print $1}\') ; if [ "$hue_ip" != "" ]; then repeat_scan=false ; echo $hue_ip ; sleep 2 ; fi ; done'

# Messages vars:
MSG_LINK = ""
MSG_LINK_ES ="Presiona el botón del puente Hue."
MSG_LINK_EN ="Press the button on the Hue bridge."
MSG_DING = ""
MSG_DING_ES = "Alguien llama al timbre."
MSG_DING_EN = "Somebody rings the bell."
MSG_INTERCOM = ""
MSG_INTERCOM_ES = "Alguien llama al telefonillo."
MSG_INTERCOM_EN = "Somebody calls the intercom."

def linkUserConfig():
  global data
  created = False
  print MSG_LINK
  data = MSG_LINK
  while not created:
    response = bridge.config.createUser(USERNAME)['resource']
    if 'error' in response[0]:
      if response[0]['error']['type'] != 101:
        print 'Unhandled error creating configuration on the Hue'
        sys.exit(response)
    else:
      created = True

def lightAlert(alert_color):
  global bridge
  if not bridge.config.isConnected():
    print 'Unauthorized user'
    linkUserConfig()
  bridge.light.findNewLights()
  numlights = bridge.light.getNumLights()
  oldstate = dict()
  for i in range(1,numlights+1):
    oldstate[i] = bridge.light.getLightState(i)
  for i in range(1,4):
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightOn(j)
        bridge.light.setLightColor(j, 254, alert_color, 254)
    time.sleep(1)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j):
        bridge.light.setLightState(j, oldstate[j]['on'], oldstate[j]['bri'], oldstate[j]['hue'], oldstate[j]['sat'], oldstate[j]['xy'])
    time.sleep(0.3)
    for j in range(1,numlights+1):
      if bridge.light.isPhisicallyOn(j) and not oldstate[j]['on']:
        bridge.light.setLightOff(j)
    time.sleep(1)

def sendAlertToAndroid():
  while True:
    global data
    req, client_address = sock.recvfrom(1024) # get the request, 1kB max
    # Look in the first line of the request for a valid command
    # The command should be 'http://server/getAlert'
    match = re.match('GET /getAlert', req)
    if match:
      if data != "":
        sock.sendto(data,(client_address[0],8081))
        data = ""
      else:
        sock.sendto("",(client_address[0],8081))
    else:
      # If there was no recognised command then return a 404 (page not found)
      print "Returning 404"
      sock.sendto("HTTP/1.1 404 Not Found\r\n",(client_address,8081))

def main():
  global data
  global bridge
  global sock

  # UDP socket configuration:
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.bind(('', 8080))

  # Gets Hue bridge's IP and create an Bridge object.
  HUE_IP = os.popen(cmd).read().strip()
  bridge = Bridge(device={'ip':HUE_IP}, user={'name':USERNAME})

  # Sets message vars.
  if LANG == 'es':
    locale.setlocale(locale.LC_TIME, "es_ES.utf8")
    MSG_LINK = MSG_LINK_ES
    MSG_DING = MSG_DING_ES
    MSG_INTERCOM = MSG_INTERCOM_ES
  else:
    locale.setlocale(locale.LC_TIME, "")
    MSG_LINK = MSG_LINK_EN
    MSG_DING = MSG_DING_EN
    MSG_INTERCOM = MSG_INTERCOM_EN

  t = threading.Thread(target=sendAlertToAndroid)
  t.start()
  while True:
    data = ""
    if not GPIO.input(INTERCOM):
      cad = MSG_INTERCOM
      data = time.strftime("%d/%b %H:%M:%S")+" - "+cad
      lightAlert(RED)

    if not GPIO.input(DING):
      cad = MSG_DING
      data = time.strftime("%d/%b %H:%M:%S")+" - "+cad
      lightAlert(BLUE)

try:
  main()
except (KeyboardInterrupt):
  print "Program ended by user."
  sock.close()
  GPIO.cleanup()