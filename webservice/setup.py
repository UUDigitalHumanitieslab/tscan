#! /usr/bin/env python
# -*- coding: utf8 -*-

from __future__ import print_function

import os
from setuptools import setup



setup(
    name = "tscan",
    version = "0.9.9", #also change in ../configure.ac and ../codemeta.json!
    author = "Martijn van der Klis, Ko van der Sloot, Maarten van Gompel",
    author_email = "m.h.vanderklis@uu.nl",
    description = ("Webservice for T-scan"),
    license = "AGPL",
    keywords = "rest nlp computational_linguistics rest",
    url = "https://github.com/proycon/tscan",
    packages=['tscanservice'],
    long_description="Feature extraction on dutch texts",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Topic :: Internet :: WWW/HTTP :: WSGI :: Application",
        "Topic :: Text Processing :: Linguistic",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3.3", #3.0, 3.1 and 3.2 are not supported by flask
        "Programming Language :: Python :: 3.4", #3.0, 3.1 and 3.2 are not supported by flask
        "Programming Language :: Python :: 3.5", #3.0, 3.1 and 3.2 are not supported by flask
        "Operating System :: POSIX",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    ],
    package_data = {'tscanservice':['*.wsgi','*.yml'] },
    include_package_data=True,
    install_requires=['CLAM >= 3.0']
)
