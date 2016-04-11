#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import urllib
import urllib2

"""
Class that implements REST petitions through HTTP methods.
"""
class RestObject:

  def _request(self, url, data, method, content_type):
    """
    @summary: Generic method to do REST petitions.
    @params: url -> URL of the petition.
             data -> For POST or PUT, data to use.
             method -> GET, POST, PUT o DELETE, HTTP method to use.
             content_type -> Content-Type of the data.
    @return: Returns response in JSON format.
    """
    if ((data or isinstance(data, dict)) and method not in ('GET','DELETE')):
      # "data" is defined or is an instance of "dict" type.
      data = json.dumps(data)
      if (method == 'PUT'):
        url_opener = urllib2.build_opener(urllib2.HTTPHandler)
        request = urllib2.Request(url, data=data)
        request.add_header('Content-Type', content_type)
        request.get_method = lambda: 'PUT'
        conn = url_opener.open(request)
      else: # (method == 'POST')
        headers = {'Content-Type': content_type}
        try:
          request = urllib2.Request(url, data, headers)
          conn = urllib2.urlopen(request)
        except TypeError:
          # POST without body.
          data = urllib.urlencode(data, 1)
          request = urllib2.Request(url, data, headers)
          conn = urllib2.urlopen(request)
    elif (method == 'DELETE'):
      url_opener = urllib2.build_opener(urllib2.HTTPHandler)
      request = urllib2.Request(url)
      request.get_method = lambda: 'DELETE'
      conn = url_opener.open(request)
    else: # (method == 'GET')
      request = urllib2.Request(url)
      conn = urllib2.urlopen(request)
    response = conn.read()
    conn.close()
    try:
      content = json.loads(response)
    except:
      content = response
    return content

  def get(self, url, data=None, content_type='application/json'):
    return self._request(url, data, 'GET', content_type)

  def post(self, url, data={}, content_type='application/json'):
    return self._request(url, data, 'POST', content_type)

  def put(self, url, data={}, content_type='application/json'):
    return self._request(url, data, 'PUT', content_type)

  def delete(self, url, data=None, content_type='application/json'):
    return self._request(url, data, 'DELETE', content_type)