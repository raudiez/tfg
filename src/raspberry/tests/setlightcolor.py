#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Test that change the color of all rgb bulvs.
"""

import time
from libs.hue import Bridge
username = 'hueusertest'
bridge = Bridge(device={'ip':'192.168.2.121'}, user={'name':username})

def main():
  numlights = bridge.light.getNumLights()
  oldstate = dict()
  for i in range(1,numlights+1):
    oldstate[i] = bridge.light.getLightState(i)
    bridge.light.setLightColor(i, 255, 45000, 255)
    print "Azul ", i
  time.sleep(2)
  for i in range(1,numlights+1):
    bridge.light.setLightOff(i)
    print "Off ", i
  time.sleep(2)
  for i in range(1,numlights+1):
    bridge.light.setLightOn(i)
    print "On ", i
  time.sleep(2)
  for i in range(1,numlights+1):
    bridge.light.setLightColor(i, oldstate[i]['bri'], oldstate[i]['hue'], oldstate[i]['sat'])
    print "Antiguo color ",i

main()