#!/usr/bin/env python

"""
Compiles the client library and SWIG wrapper.
"""

from distutils.core import setup, Extension

client_module = Extension('_client', sources=['client_wrap.cxx', 'client.cpp'])

setup(name = 'client',
    version = '0.1',
    author      = "Allen Porter <allen@thebends.org>",
    description = """Mach RPC client library""",
    ext_modules = [client_module],
    py_modules = ["client"])
