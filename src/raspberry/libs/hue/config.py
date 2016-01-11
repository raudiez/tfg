#!/usr/bin/env python
# -*- coding: utf-8 -*-

from libs.rest import RestObject

class Config:

  def __init__(self, bridge, user):
    self.bridge = bridge
    self.user = user

  def isConnected(self):
    """
    @summary: See if Hue is connected.
    @return: Boolean which determines if there is connection stablished, and string with connection message.
    """
    rest_obj = RestObject()
    path = 'api/{username}'.format(username=self.user['name'])
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.get(url)
    content = dict(resource=content)
    if 'lights' in content['resource']:
      return True
    elif 'error' in content['resource'][0]:
      error = content['resource'][0]['error']
      if error['type'] == 1:
        return False

  def createUser(self, user):
    """
    @summary: Create an user on the Hue Bridge.
    @param: user -> String with username to create.
    @return: Info about operation.
    """
    rest_obj = RestObject()
    url = 'http://{bridge_ip}/api'.format(bridge_ip=self.bridge['ip'])
    resource = {'devicetype': user, 'username': user}
    content = rest_obj.post(url, resource)
    return dict(resource=content)

  def deleteUser(self, user):
    """
    @summary: Delete an user from Hue Bridge.
    @param: user -> String with username to delete.
    @return: Info about operation.
    """
    rest_obj = RestObject()
    service = 'config/whitelist/{id}'.format(id=user)
    path = 'api/{username}/{service}'.format(username=self.user['name'], service=service)
    url = 'http://{bridge_ip}/{path}'.format(bridge_ip=self.bridge['ip'], path=path)
    content = rest_obj.delete(url)
    return dict(resource=content)
