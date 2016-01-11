#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Test that, if user not exist, create it.
"""

import sys
from libs.hue import Bridge
username = 'otrousuariomas'
bridge = Bridge(device={'ip':'192.168.2.121'}, user={'name':username})

def createConfig():
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

def main():
  if bridge.config.isConnected():
    print 'Connected to the Hub'
  else:
    print 'Unauthorized user'
    createConfig()
    main()

main()