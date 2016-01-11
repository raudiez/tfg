#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Test that change the color of all rgb bulvs when a button is pushed.
"""

import time
import RPi.GPIO as GPIO
from libs.hue import Bridge
username = 'hueusertest'
bridge = Bridge(device={'ip':'192.168.2.121'}, user={'name':username})

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(17,GPIO.IN)
GPIO.setup(18,GPIO.IN)

button1=17
button2=18

RED=0
YELLOW=12750
GREEN=25500
BLUE=46920
PINK=56100

def main():
  while True:
    if not GPIO.input(button1):
      print "Button1 pressed."
      numlights = bridge.light.getNumLights()
      oldstate = dict()
      for i in range(1,numlights+1):
        oldstate[i] = bridge.light.getLightState(i)
      for i in range(1,4):
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j):
            bridge.light.setLightState(j, True, 254, BLUE, 254)
            print "Azul ",j
        time.sleep(1)
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j):
            bridge.light.setLightColor(j, oldstate[j]['bri'], oldstate[j]['hue'], oldstate[j]['sat'])
            print "Antiguo color ",j
        time.sleep(0.3)
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j) and not oldstate[j]['on']:
            bridge.light.setLightOff(j)
        time.sleep(1)
    if not GPIO.input(button2):
      print "Button2 pressed."
      numlights = bridge.light.getNumLights()
      oldstate = dict()
      for i in range(1,numlights+1):
        oldstate[i] = bridge.light.getLightState(i)
      for i in range(1,4):
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j):
            bridge.light.setLightState(j, True, 254, RED, 254)
            print "Azul ",j
        time.sleep(1)
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j):
            bridge.light.setLightColor(j, oldstate[j]['bri'], oldstate[j]['hue'], oldstate[j]['sat'])
            print "Antiguo color ",j
        time.sleep(0.3)
        for j in range(1,numlights+1):
          if bridge.light.isPhisicallyOn(j) and not oldstate[j]['on']:
            bridge.light.setLightOff(j)
        time.sleep(1)

try:
  main()
except (KeyboardInterrupt):
  print "FIN."
  GPIO.cleanup()