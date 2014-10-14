from Scripts import graphiti as og
from Scripts import std

import sys

ctx = dict()
views = dict()

def on_started():
	global ctx

	'''
	if len(sys.argv) == 3:
		if sys.argv[2].endswith(".json"):
			std.load_json(sys.argv[2]) 
		else:
			print("Unrecognized format <'" + sys.argv[2] + "'> !")
	'''

	print(ctx['g1'])
	og.bind_entity(ctx['g1'])
	nid = og.add_node("node1")
	og.set_node_attribute(nid, "og:space:color", "vec4", "1.0 0.0 0.0 1.0")


	print(ctx['g2'])
	og.bind_entity(ctx['g2'])
	nid = og.add_node("node2")
	og.set_node_attribute(nid, "og:world:geolocation", "vec2", "48.8742, 2.3470")
	og.set_node_attribute(nid, "og:world:color", "vec4", "1.0 1.0 0.0 1.0")

def start():

	global ctx

	og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)

	ctx['g1'] = og.create_entity("graph")
	print(ctx['g1'] )
	views["space"] = og.create_visualizer("space")

	ctx['g2'] = og.create_entity("graph")
	views["world"] = og.create_visualizer("world")

	#ts = og.create_entity("time_series", { "size" : 1000 })
	#views['line_chart'] = og.create_view("line_chart")

	og.register_script('#started', 'dashboard.on_started()')

	og.start()