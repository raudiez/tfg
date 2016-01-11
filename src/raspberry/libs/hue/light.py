#!/usr/bin/env python
# -*- coding: utf-8 -*-

from libs.rest import RestObject

class Light:

  def __init__(self, bridge, user):
    self.bridge = bridge
    self.user = user

  def get(self, resource={'which':'all'}):
    """
    @summary: Get all lights, get new lights, or get a specific light as
              determined by the resource object.
    """
    rest_obj = RestObject()
    services = {
      'all':{'service':'lights'},
      'new':{'service':'lights/new'}
    }
    if (isinstance(resource['which'], int)):
      resource['id'] = resource['which']
      resource['which'] = 'one'
    if (resource['which'] == 'one'):
      services['one'] = {'service':'lights/{id}'.format(id=resource['id'])}
    service = services[resource['which']]['service']
    path = 'api/{username}/{service}'.format(username=self.user['name'], service=service)
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.get(url)
    if service == 'lights':
      lights = []
      for (k, v) in content.items():
        v['id'] = int(k)
        lights.append(v)
      content = lights
    return dict(resource=content)

  def getNumLights(self):
    """
    @summary: Get num of Lights connected.
    @return: Num of lights.
    """
    rest_obj = RestObject()
    path = 'api/{username}/lights'.format(username=self.user['name'])
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.get(url)
    lights = []
    for (k, v) in content.items():
      v['id'] = int(k)
      lights.append(v)
    return len(lights)

  def getLightState(self, light):
    """
    @summary: Get ONLY 'state' of determined light.
    @return: Light 'state' in dict format.
    """
    return self.get({'which':light})['resource']['state']

  def isPhisicallyOn(self, light):
    """
    @summary: Get ONLY 'reachable' of determined light. A reachable
              light means that light is physically turned on.
    @return: Light 'reacheble' (boolean).
    """
    return self.get({'which':light})['resource']['state']['reachable']

  def findNewLights(self):
    """
    @summary: Search for new lights.
    """
    rest_obj = RestObject()
    path = 'api/{username}/lights'.format(username=self.user['name'])
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.post(url)
    return dict(resource=content)

  def update(self, resource):
    """
    @summary: Rename lights, or set a light's state, as determined by the\
              resource object.
    """
    rest_obj = RestObject()
    if (resource['data'].has_key('attr')):
      service = 'lights/{id}'.format(id=resource['which'])
      data = resource['data']['attr']
    elif (resource['data'].has_key('state')):
      service = 'lights/{id}/state'.format(id=resource['which'])
      data = resource['data']['state']
    else:
      raise Exception('Unknown data type.')
    path = 'api/{username}/{service}'.format(username=self.user['name'], service=service)
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.put(url, data)
    return dict(resource=content)

  def setLightColor(self, light, bri, hue, sat):
    """
    @summary: Set light color. It uses self.update.
    @params: light -> Light number to change color.
             bri -> Color brightness.
             hue -> Hue color num.
             sat -> Color saturation.
    """
    resource = {
      'which':light,
      'data':{'state':{'bri':bri,'hue':hue,'sat':sat}}
    }
    return self.update(resource)

  def setLightState(self, light, on, bri, hue, sat):
    """
    @summary: Set light color. It uses self.update.
    @params: light -> Light number to change color.
             on -> On/Off state of the light.
             bri -> Color brightness.
             hue -> Hue color num.
             sat -> Color saturation.
    """
    resource = {
      'which':light,
      'data':{'state':{'on':on,'bri':bri,'hue':hue,'sat':sat}}
    }
    return self.update(resource)

  def setLightOn(self, light):
    """
    @summary: Set light ON. It uses self.update.
    @params: light -> Light number to turn on.
    """
    resource = {
      'which':light,
      'data':{'state':{"on":True}}
    }
    return self.update(resource)

  def setLightOff(self, light):
    """
    @summary: Set light OFF. It uses self.update.
    @params: light -> Light number to turn off.
    """
    resource = {
      'which':light,
      'data':{'state':{"on":False}}
    }
    return self.update(resource)
