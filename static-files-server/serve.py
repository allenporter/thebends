#!/usr/bin/python2.4

"""Static file server
"""

__author__ = 'allen@thebends.org (Allen Porter)'

import os
import wsgiref.handlers
import cgi
import mimetypes

from google.appengine.ext import webapp

ROOT = os.path.abspath('.')

class StaticFiles(webapp.RequestHandler):
  def get(self):
    request_path = os.environ['PATH_INFO']
    full_path = os.path.normpath('%s/%s' % (ROOT, request_path)) 
    if not full_path.startswith(ROOT):
      self._NotFound()
      return

    # Directory listing is not support, just look for an index.html file only.
    if os.path.isdir(full_path):
      full_path = full_path + '/index.html'

    if not os.path.isfile(full_path):
      self._NotFound()
      return

    file = open(full_path)

    (type, encoding) = mimetypes.guess_type(full_path)
    data = file.read()
    self.response.headers['Content-Type'] = type
    self.response.out.write(data)

  def _NotFound(self):
    self.response.headers['Content-Type'] = 'text/plain'
    self.response.out.write('Not found')

application = webapp.WSGIApplication(
    [ ( '/.*', StaticFiles ) ],
    debug=True)

wsgiref.handlers.CGIHandler().run(application)
