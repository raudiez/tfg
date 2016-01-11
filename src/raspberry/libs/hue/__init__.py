#!/usr/bin/env python
# -*- coding: utf-8 -*-

class Bridge():

  def __init__(self, device, user):
    from config import Config
    from light import Light

    self.config = Config(device, user)
    self.light = Light(device, user)

    # After create Light object, search for new lights.
    self.light.findNewLights()