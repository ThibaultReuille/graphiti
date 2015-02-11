from Scripts import graphiti as og
from Scripts import std

import sys

def start():

	global ctx

	og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)

	og.create_entity("graph")
	og.create_visualizer("network")

	og.register_script("test", "print('yes')")

	og.start()