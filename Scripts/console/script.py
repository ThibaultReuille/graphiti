from Scripts import graphiti as og
from Scripts import std

from Scripts import nx

import sys
import argparse

import os.path
import glob
import fnmatch

import itertools

import random
import math

import json

class Script(object):
	def __init__(self, console):
		self.console = console

	def run(self, args):
		self.console.log("Error: run() method not implemented!")