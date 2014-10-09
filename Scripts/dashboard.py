from Scripts import graphiti as og
from Scripts import std

import sys

graph = None
ts = None
views = dict()

def on_started():
	global graph
	global ts

	'''
	if len(sys.argv) == 3:
		if sys.argv[2].endswith(".json"):
			std.load_json(sys.argv[2]) 
		else:
			print("Unrecognized format <'" + sys.argv[2] + "'> !")
	'''

	og.bind_entity(graph)
	nid = og.add_node("test")
	og.set_node_attribute(nid, "og:space:color", "vec4", "1.0 0.0 0.0 1.0")
	og.set_node_attribute(nid, "og:world:geolocation", "vec2", "48.8742, 2.3470")
	og.set_node_attribute(nid, "og:world:color", "vec4", "1.0 1.0 0.0 1.0")

def start():

	og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)

	graph = og.create_entity("graph")
	views["space"] = og.create_visualizer("space")
	views["world"] = og.create_visualizer("world")

	#ts = og.create_entity("time_series", { "size" : 1000 })
	#views['line_chart'] = og.create_view("line_chart")

	og.register_script('#started', 'dashboard.on_started()')

	og.start()