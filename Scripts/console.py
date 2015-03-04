from Scripts import graphiti as og
from Scripts import std
from Scripts import nx

import sys
import os.path
import glob
import itertools
import argparse

import random
import math

class Script(object):
	def __init__(self, console):
		self.console = console

	def run(self, args):
		print("Error: run() method not implemented!")

class Info(Script):
	def run(self, args):
		std.info()

class Load(Script):
	def run(self, args):
		if len(args) < 2:
			# TODO: og.console("Usage: {0} <filename>".format(args[0]))
			print("Usage: {0} <filename>".format(args[0]))
			return

		std.load_json(" ".join(args[1:]))

class Save(Script):
	def run(self, args):
		if len(args) != 2:
			# TODO: og.console("Usage: {0} <filename>".format(args[0]))
			print("Usage: {0} <filename>".format(args[0]))
			return

		if os.path.isfile(args[1]):
			print("Error: File already exists!")
			return
		
		std.save_json(args[1])

class Clear(Script):

	def clear_graph(self):
	    for id in og.get_node_ids():
	        og.remove_node(id)

	def clear_colors(self):
	    og.set_attribute("og:space:linkmode", "string", "node_color")
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:color", "vec4", "1.0 1.0 1.0 1.0")

	def clear_icons(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:icon", "string", "shapes/disk")

	def clear_activity(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:activity", "float", "0.0")
		for e in og.get_link_ids():
			og.set_link_attribute(e, "og:space:activity", "float", "0.0")

	def clear_lod(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:lod", "float", "1.0")
		for e in og.get_link_ids():
			og.set_link_attribute(e, "og:space:lod", "float", "1.0")

	def run(self, args):
		if len(args) == 1:
			self.clear_graph()
		elif len(args) == 2 and args[1] == "colors":
			self.clear_colors()
		elif len(args) == 2 and args[1] == "icons":
			self.clear_icons()
		elif len(args) == 2 and args[1] == "activity":
			self.clear_activity()
		elif len(args) == 2 and args[1] == "lod":
			self.clear_lod()
		else:
			print("Usage: {0} [colors|icons]".format(args[0]))
			# TODO : og.console()

class Color(Script):
	
	def __init__(self, console):
		super(Color, self).__init__(console)

		self.colors = {
			"red" : [ 1.0, 0.0, 0.0, 1.0 ],
			"green" : [ 0.0, 1.0, 0.0, 1.0 ],
			"blue" : [ 0.0, 0.0, 1.0, 1.0 ],
			"yellow" : [ 1.0, 1.0, 0.0, 1.0 ],
			"cyan" : [ 0.0, 1.0, 1.0, 1.0 ],
			"magenta" : [ 1.0, 0.0, 1.0, 1.0 ],
			"white" : [ 1.0, 1.0, 1.0, 1.0 ],
			"gray" : [ 0.5, 0.5, 0.5, 1.0 ],
			"black" : [ 0.0, 0.0, 0.0, 1.0 ]
		}

		self.color_map = None
		self.color_masks = {
			"rgba" : [ True, True, True, True ],
			"rgb" : [ True, True, True, False ],
			"alpha" : [ False, False, False, True ]
		} 

	def random_color(self):
		return [ random.random(), random.random(), random.random(), 1.0 ]

	def parse_color(self, s):
		if s in self.colors:
			return std.vec4_to_str(self.colors[s])
		else:
			return std.vec4_to_str(self.colors["black"])

	def lambda_assign(self, element_type, element_id, color):
		if element_type == "node":
			og.set_node_attribute(element_id, "og:space:color", "vec4", color)
		elif element_type == "edge":
			og.set_link_attribute(element_id, "og:space:color", "vec4", color)

	def lambda_by(self, element_type, element_id, attr, color_map):
		if element_type not in color_map:
			color_map[element_type] = dict()

		if element_type == "node":
			value = og.get_node_attribute(element_id, attr)
		elif element_type == "edge":
			value = og.get_link_attribute(element_id, attr)

		if value is None:
			color = std.vec4_to_str(self.colors["gray"])
		else:
			value = "{0}".format(value)
			if value not in color_map[element_type]:
				color_map[element_type][value] = self.random_color()
			color = std.vec4_to_str(color_map[element_type][value])

		if element_type == "node":
			og.set_node_attribute(element_id, "og:space:color", "vec4", color)
		elif element_type == "edge":
			og.set_link_attribute(element_id, "og:space:color", "vec4", color)

	def lambda_op(self, element_type, element_id, op, color_mask, factor):

		def calculate(op, v1, v2, mask):
			print(op, v1, v2, mask)
			if op == "add":
				r = [ v1[i] + v2[i] for i in xrange(4) ]
			elif op == "sub":
				r = [ v1[i] - v2[i] for i in xrange(4) ]
			elif op == "mul":
				r = [ v1[i] * v2[i] for i in xrange(4) ]
			elif op == "div":
				r = [ v1[i] / v2[i] for i in xrange(4) ]
			elif op == "set":
				r = v2
			else:
				print("Error: '{0}': Unknown operator!")
				return

			for i in xrange(4):
				if not mask[i]:
					r[i] = v1[i]
			return r

		if element_type == "node":
			color = og.get_node_attribute(element_id, "og:space:color")
			og.set_node_attribute(element_id, "og:space:color", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))
		elif element_type == "edge":
			color = og.get_link_attribute(element_id, "og:space:color1")
			og.set_link_attribute(element_id, "og:space:color1", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))
			color = og.get_link_attribute(element_id, "og:space:color2")
			og.set_link_attribute(element_id, "og:space:color2", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))

	def run(self, args):

		query = self.console.query
		if query is None:
			print("Error: Query is empty!")
			return

		if len(args) == 2:
			color = self.parse_color(args[1])
			if 'nodes' in query:
				[ self.lambda_assign("node", nid, color) for nid in query['nodes'] ]
			if 'edges' in query:
				[ self.lambda_assign("edge", eid, color) for eid in query['edges'] ]
		
		elif len(args) == 3 and args[1] == "by":
			attr = args[2]
			color_map = dict()
			if 'nodes' in query:
				[ self.lambda_by("node", nid, attr, color_map) for nid in query['nodes'] ]
			if 'edges' in query:
				[ self.lambda_by("edge", eid, attr, color_map) for eid in query['edges'] ]

		elif len(args) >= 4 and args[1] in [ "mul", "div", "add", "sub", "set" ]:
			if args[2] not in self.color_masks:
				print("Error: '{0}': Unknown color mask!".format(args[2]))
				return

			array = [ float(i) for i in " ".join(args[3:]).split() ]

			if len(array) == 1:
				factor = [ array[0], array[0], array[0], array[0] ]
			elif len(array) == 3:
				factor = [ array[0], array[1], array[2], 1.0 ]
			elif len(array) == 4:
				factor = [ array[0], array[1], array[2], array[3] ]
			else:
				print("Error: Can't parse color factor!")
				return

			if 'nodes' in query:
				[ self.lambda_op("node", nid, args[1], self.color_masks[args[2]], factor) for nid in query['nodes'] ]
			if 'edges' in query:
				[ self.lambda_op("edge", eid, args[1], self.color_masks[args[2]], factor) for eid in query['edges'] ]

class Layout(Script):
	
	def point(self):
		ids = og.get_node_ids()
		for nid in ids:
		    og.set_node_attribute(nid, "og:space:position", "vec3", "0.0 0.0 0.0")

	def sphere(self, args):
		radius = 20
		if len(args) == 2:
			radius = float(args[1])

		print(args, radius)

		ids = og.get_node_ids()
		for nid in ids:
		    r1 = random.random() * 2 * math.pi
		    r2 = random.random() * 2 * math.pi
		    r3 = 0.9 + 0.1 * random.random()
		    pos = [
		        radius * r3 * math.sin(r1) * math.cos(r2),
		        radius * r3 * math.cos(r1),
		        radius * r3 * math.sin(r1) * math.sin(r2)
		    ]
		    og.set_node_attribute(nid, "og:space:position", "vec3", std.vec3_to_str(pos))

	def cube(self):
	    ids = og.get_node_ids()
	    size = int(len(ids) ** (1.0 / 3.0))
	    count = 0
	    for nid in ids:
	        pos = [
	            count % size,
	            count / (size * size),
	            (count % (size * size)) / size
	        ]

	        for i in range(len(pos)):
	            pos[i] = 5 * (pos[i] - size / 2)

	        og.set_node_attribute(nid, "og:space:position", "vec3", std.vec3_to_str(pos))
	        count += 1

	def cone(self):
	    graph = std.load_nx_graph()

	    og.set_attribute("og:space:linkmode", "string", "node_color")

	    sorted_degrees = sorted(nx.degree(graph).values())
	    max_degree = sorted_degrees[-1]

	    degree_nodes = {}
	    for n in graph.nodes(data = True):
	        degree = nx.degree(graph, n[0])
	        if degree in degree_nodes:
	            degree_nodes[degree].append(n[0])
	        else:
	            degree_nodes[degree] = [n[0]]

	    max_radius = 30.0
	    max_height = 20
	    for n in graph.nodes(data = True):
	        degree = nx.degree(graph, n[0])

	        nodes = degree_nodes[degree]

	        radius = 1.0 + max_radius * float(1.0 - float(degree) / float(max_degree))
	        alpha = 2.0 * math.pi * random.random() #float(nodes.index(n[0])) / float(len(nodes)) 
	        # 3D
	        # beta = 2.0 * math.pi * random.random() #float(nodes.index(n[0])) / float(len(nodes)) 

	        x = radius * math.cos(alpha)
	        y = max_height * float(degree) / float(max_degree)
	        z = radius * math.sin(alpha)
	        # 3D
	        # x = radius * math.sin(alpha) * math.cos(beta)
	        # y = radius * math.sin(alpha) * math.sin(beta)
	        # z = radius * math.cos(alpha)

	        og.set_node_attribute(n[0], "og:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))


	def globe_coordinates(latitude, longitude, delta = 0.0):
	    globe_radius = 50
	    radLatitude = latitude * math.pi / 180.0;
	    radLongitude = longitude * math.pi / 180.0;

	    return [
	        (globe_radius + delta) * math.cos(radLatitude) * math.cos(radLongitude),
	        (globe_radius + delta) * math.sin(radLatitude),
	        - (globe_radius + delta) * math.cos(radLatitude) * math.sin(radLongitude)
	    ]

	def globe(self):
	    ids = og.get_node_ids()
	    for id in ids:
	        geo = og.get_node_attribute(id, "og:world:geolocation")
	        if geo is not None:
	           pos = self.globe_coordinates(geo[0], geo[1])
	           og.set_node_attribute(id, "og:space:position", "vec3", std.vec3_to_str(pos))
	           og.set_node_attribute(id, "og:space:locked", "bool", "true")


	def seeds(self):
	    radius = 100.0

	    ids = og.get_node_ids()
	    zeros = []
	    for id in ids:
	        attribute = og.get_node_attribute(id, "depth")
	        if attribute == None:
	            continue
	        else:
	            if attribute == 0:
	                zeros.append(id)

	    count = 0
	    if len(zeros) == 1:
	        radius = 0.0
	    for id in zeros:
	        angle = 2.0 * math.pi * float(count) / float(len(zeros))

	        x = radius * math.cos(angle)
	        y = 0.0
	        z = radius * math.sin(angle)

	        og.set_node_attribute(id, "og:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))
	        og.set_node_attribute(id, "og:space:locked", "bool", "True")
	        og.set_node_attribute(id, "og:space:activity", "float", "2.0")
	        og.set_node_attribute(id, "og:space:mark", "int", "2")
	        count += 1

	def usage(self, args):
		print("Usage: {0} [point|cube|sphere|cone|globe|seeds]".format(args[0]))

	def run(self, args):
		if len(args) == 2 and args[1] == "point":
			self.point()
		elif len(args) == 2 and args[1] == "cube":
			self.cube()
		elif len(args) >= 2 and args[1] == "sphere":
			self.sphere(args[1:])
		elif len(args) == 2 and args[1] == "cone":
			self.cone()
		elif len(args) == 2 and args[1] == "globe":
			self.globe()
		elif len(args) == 2 and args[1] == "seeds":
			self.seeds()
		else:
			self.usage(args)

class Camera(Script):
	def play(self, args):
		og.set_attribute('og:space:animation', 'bool', 'True')
	def stop(self, args):
		og.set_attribute('og:space:animation', 'bool', 'False')
	
	def run(self, args):
		if len(args) == 2 and args[1] == "play":
			self.play(args)
		elif len(args) == 2 and args[1] == "stop":
			self.stop(args)
		else:
			print("Usage: {0} [play|stop]".format(args[0]))	

class Topology(Script):

	def neighbors(self, args):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    if og.count_selected_nodes() == 0:
	        print("Please select a node !")
	        return
	    
	    selected = og.get_selected_node(0)
	    graph = std.load_nx_graph()
	    neighbors = graph.neighbors(selected)
	    og.set_node_attribute(selected, "graphiti:space:color", "vec3", "0.0 1.0 1.0")
	    for node in neighbors:
	        og.set_node_attribute(node, "graphiti:space:lod", "float", "1.0")
	        og.set_node_attribute(node, "graphiti:space:color", "vec3", "0.0 1.0 1.0")

	def connected_components(self, args):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    graph = std.load_nx_graph()
	    cc = nx.connected_components(graph)

	    for component in cc:
	        r = random.random()
	        g = random.random()
	        b = random.random()
	        color = str(r) + " " + str(g) + " " + str(b) 
	        
	        for node in component:
	            og.set_node_attribute(node, "graphiti:space:color", "vec3", color)

	def directions(self, args):
	    for id in og.get_link_ids():
	        og.set_link_attribute(id, "og:space:icon", "string", "styles/triangles")

	def connections(self, args):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    # Building node degree table ...
	    edges = og.get_link_ids()
	    degree_table = dict()
	    for eid in edges:
	        nid1 = og.get_link_node1(eid)
	        nid2 = og.get_link_node2(eid)
	        if nid1 not in degree_table:
	            degree_table[nid1] = { "in" : 0, "out" : 0 }
	        if nid2 not in degree_table:
	            degree_table[nid2] = { "in" : 0, "out" : 0 }
	        degree_table[nid1]["out"] += 1
	        degree_table[nid2]["in"] += 1

	    # Randomizing color map
	    m = dict()
	    m["isolated"] = [0.95, 0.98, 0.36, 1.0]
	    m["leaf"] = [0.06, 0.94, 0.61, 1.0]
	    m["source"] = [0.91, 0.18, 0.17, 1.0]
	    m["sink"] = [0.03, 0.65, 0.94, 1.0]
	    m["other"] = [0.77, 0.78, 0.75, 1.0]

	    # Coloring
	    for nid in og.get_node_ids():
	        if nid not in degree_table:
	            t = "isolated"
	        else:
	            deg = degree_table[nid]
	            if deg["in"] == 0 and deg["out"] == 1:
	                t = "leaf"
	            elif deg["in"] == 0 and deg["out"] > 1:
	                t = "source"
	            elif deg["in"] > 0 and deg["out"] == 0:
	                t = "sink"
	            else:
	                t = "other"
	        og.set_node_attribute(nid, "graphiti:space:color", "vec4", std.vec4_to_str(m[t]))

	def degrees_high(self):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    graph = std.load_nx_graph()
	    max_degree = max(nx.degree(graph).values())
	    for n in graph.nodes(data = True):
	        deg = nx.degree(graph, n[0])
	        tint = 0.3 + 0.9 * float(deg) / float(max_degree)

	        color = og.get_node_attribute(n[0], "graphiti:space:color")
	        color[0] = tint * color[0]
	        color[1] = tint * color[1]
	        color[2] = tint * color[2]
	        color[3] = 1.0
	        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

	        og.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

	def degrees_low(self):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    graph = std.load_nx_graph()
	    max_degree = max(nx.degree(graph).values())
	    for n in graph.nodes(data = True):
	        deg = nx.degree(graph, n[0])
	        tint = 0.3 + 0.9 * (1.0 - float(deg) / float(max_degree))

	        color = og.get_node_attribute(n[0], "graphiti:space:color")
	        color[0] = tint * color[0]
	        color[1] = tint * color[1]
	        color[2] = tint * color[2]
	        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

	        og.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

	def degrees(self, args):
		if len(args) == 1:
			self.degrees_high()
		elif len(args) == 2 and args[1] == "high":
			self.degrees_high()
		elif len(args) == 2 and args[1] == "low":
			self.degrees_low()
		else:
			print("Error: {0}: Wrong arguments!".format(args[0]))

	def get_degree_map(self):
	    degrees = dict()

	    for eid in og.get_link_ids():
	        bi = False
	        e_type = og.get_link_attribute(eid, "type")
	        if e_type is not None and "<->" in e_type:
	            bi = True

	        nid1 = og.get_link_node1(eid)
	        nid2 = og.get_link_node2(eid)

	        if nid1 not in degrees:
	            degrees[nid1] = { "in" : 0, "out" : 0 }
	        if nid2 not in degrees:
	            degrees[nid2] = { "in" : 0, "out" : 0 }
	        
	        if bi:
	            degrees[nid1]["in"] += 1
	            degrees[nid1]["out"] += 1
	            degrees[nid2]["in"] += 1
	            degrees[nid2]["out"] += 1
	        else:
	            degrees[nid1]["out"] += 1
	            degrees[nid2]["in"] += 1

	    return degrees

	def spn(self, args):
	    degree_map = self.get_degree_map()
	    source_map = dict()

	    for eid in og.get_link_ids():
	        src = og.get_link_node1(eid)
	        if src not in degree_map:
	            continue
	        
	        if degree_map[src]["in"] == 0 and degree_map[src]["out"] >= 0:
	            dst = og.get_link_node2(eid)
	            if src not in source_map:
	                source_map[src] = [(dst, eid)]
	            elif dst not in source_map[src]:
	                source_map[src].append((dst, eid))

	    for nid in og.get_node_ids():
	        og.set_node_attribute(nid, "og:space:lod", "float", "0.0")
	        
	    for eid in og.get_link_ids():
	        og.set_link_attribute(eid, "og:space:lod", "float", "0.0")

	    for source in source_map.keys():
	        og.set_node_attribute(source, "og:space:lod", "float", "1.0")

	        for successor in source_map[source]:
	            og.set_node_attribute(successor[0], "og:space:lod", "float", "1.0")
	            og.set_link_attribute(successor[1], "og:space:lod", "float", "1.0")

	def run(self, args):
		if len(args) == 2 and args[1] == "neighbors":
			self.neighbors(args)
		elif len(args) == 2 and args[1] == "cc":
			self.connected_components(args)
		elif len(args) == 2 and args[1] == "directions":
			self.directions(args)
		elif len(args) == 2 and args[1] == "connections":
			self.connections(args)
		elif len(args) >= 2 and args[1] == "degrees":
			self.degrees(args[1:])
		elif len(args) == 2 and args[1] == "spn":
			self.spn(args)
		else:
			print("Error: {0}: Wrong arguments!".format(args[0]))

class Test(Script):

	def randomize_timeline(self):
	    ids = og.get_node_ids()
	    time = 0
	    for nid in ids:
	        time += 500
	        og.send_command(time, "graph:set_node_attribute",
	                              {
	                                  "id" : int(random.choice(ids)),
	                                  "type" : "vec4",
	                                  "name" : "graphiti:space:color",
	                                  "value" : "1.0 0.0 0.0 1.0"
	                              }) 

	def randomize_activity(self):
	    for id in og.get_node_ids():
	        if random.uniform(0.0, 1.0) > 0.10:
	            continue
	        activity = random.uniform(0.0, 5.0)
	        og.set_node_attribute(id, "og:space:activity", "float", str(activity))
  	    for id in og.get_link_ids():
	        activity = random.uniform(0.0, 2.0)
	        og.set_link_attribute(id, "og:space:activity", "float", str(activity))

	def randomize_icons(self):
	    icons = glob.glob("./Assets/Countries/*.png")
	    print(icons)
	    for id in og.get_node_ids():
	        icon = "countries/" + random.choice(icons).split("/")[-1][:2].lower()
	        og.set_node_attribute(id, "og:space:icon", "string", icon)

	    icons = glob.glob("./Assets/SpaceView/EdgeStyles/*.png")
	    print(icons)
	    for id in og.get_link_ids():
	        icon = "styles/" + random.choice(icons).split("/")[-1][:-4].lower()
	        og.set_link_attribute(id, "og:space:icon", "string", icon)

	def randomize_lod(self):
	    for id in og.get_node_ids():
	        og.set_node_attribute(id, "og:space:lod", "float", str(random.random()))   
	    for id in og.get_link_ids():
	        og.set_link_attribute(id, "og:space:lod", "float", str(random.random()))   

	def randomize_node_size(self):
	    for id in og.get_node_ids():
	        activity = random.uniform(0.0, 5.0)
	        og.set_node_attribute(id, "og:space:size", "float", str(activity))

	def randomize_edge_width(self):
	    for id in og.get_link_ids():
	        width = random.uniform(0.0, 5.0)
	        og.set_link_attribute(id, "og:space:width", "float", str(width))

	def randomize(self, args):
		if len(args) == 2 and args[1] == "activity":
			self.randomize_activity()
		elif len(args) == 2 and args[1] == "timeline":
			self.randomize_timeline()
		elif len(args) == 2 and args[1] == "lod":
			self.randomize_lod()
		elif len(args) == 2 and args[1] == "icons":
			self.randomize_icons()
		elif len(args) == 3 and args[1] == "node" and args[2] == "size":
			self.randomize_node_size()
		elif len(args) == 3 and args[1] == "edge" and args[2] == "width":
			self.randomize_edge_width()
		else:
			print("Error: {0}: Wrong arguments!".format(args[0]))

	def debug(self, args):
	    flag = og.get_attribute("og:space:debug")
	    og.set_attribute("og:space:debug", "bool", str(not flag))

	def random_graph(self, args):
		node_count = 1000
		edge_count = 1000

		if len(args) >= 3:
			node_count = int(args[2])
		if len(args) >= 4:
			edge_count = int(args[3])

		nodes = list()
		for i in range(node_count):
		    id = og.add_node(str(i))
		    og.set_node_attribute(id, "type", "string", str(i % 3))
		    size = 1 + abs(math.sin(float(i)))
		    og.set_node_attribute(id, "og:space:size", "float", str(size))
		    nodes.append(id)

		for j in range(edge_count):
		    loop = True
		    while loop:
		        src = random.choice(nodes)
		        dst = random.choice(nodes)
		        if src != dst:
		            loop = False
		    id = og.add_link(src, dst)
		    og.set_link_attribute(id, "type", "string", str(j % 3))

	def add_neighbor(self, args):
		if og.count_selected_nodes() == 0:
		    print("Please select a node !")
		    return
		sid = og.get_selected_node(0)

		if len(args) >= 2:
		    label = args[1]
		    nid = og.add_node(label)
		    og.add_link(sid, nid)

		    if len(args) >= 3:
		    	og.set_node_attribute(nid, "type", "string", args[2])
		else:
			print("Error: {0}: Wrong arguments!".format(args[0]))

	def run(self, args):
		if len(args) > 2 and args[1] == "randomize":
			self.randomize(args[1:])
		elif len(args) >= 2 and args[1] == "debug":
			self.debug(args[1:])
		elif len(args) >= 2 and args[1] == "graph":
			self.random_graph(args)
		elif len(args) >= 3 and args[1] == "neighbor":
			self.add_neighbor(args[1:])
		else:
			print("Error: {0}: Wrong arguments!".format(args[0]))

class OpenDNS(Script):
	def score(self):
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    ids = og.get_node_ids()
	    for id in ids:
	        score = og.get_node_attribute(id, "sgraph:score")
	        if score is None:
	            og.set_node_attribute(id, "og:space:color", "vec4", "0.5 0.5 0.5 0.5")
	        elif score < -50:
	            og.set_node_attribute(id, "og:space:color", "vec3", "1.0 0.0 0.0")
	        elif score > 50:
	            og.set_node_attribute(id, "og:space:color", "vec3", "0.0 1.0 0.0")
	        else:
	            og.set_node_attribute(id, "og:space:color", "vec3", "1.0 1.0 1.0")

	def infected(self):
	    
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    ids = og.get_node_ids()
	    for id in ids:
	        score = og.get_node_attribute(id, "sgraph:infected")
	        if score is None:
	            og.set_node_attribute(id, "og:space:color", "vec4", "0.5 0.5 0.5 0.5")
	        elif score < 0:
	            og.set_node_attribute(id, "og:space:color", "vec3", "1.0 0.0 0.0")
	        elif score > 0:
	            og.set_node_attribute(id, "og:space:color", "vec3", "0.0 1.0 0.0")
	        else:
	            og.set_node_attribute(id, "og:space:color", "vec3", "1.0 1.0 1.0")

	def dga(self):

	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

	    ids = og.get_node_ids()
	    for id in ids:
	        score = og.get_node_attribute(id, "sgraph:dga:score")
	        if score is None:
	            og.set_node_attribute(id, "og:space:color", "vec4", "0.5 0.5 0.5 0.5")
	            continue
	        else:
	            # DGA score is between [0 : not DGA, 100 : DGA]
	            sub = score / 100;
	            rgb = [1.0, 1.0 - sub, 1.0 - sub, 1.0]
	            og.set_node_attribute(id, "og:space:color", "vec4", std.vec4_to_str(rgb))

	def run(self, args):
		if len(args) == 2 and args[1] == "score":
			self.score()
		elif len(args) == 2 and args[1] == "infected":
			self.infected()
		elif len(args) == 2 and args[1] == "dga":
			self.dga()



class Query(Script):
	def __init__(self, console):
		super(Query, self).__init__(console)

	def run(self, args):
		s = " ".join(args[1:]).strip()
		if s == "nodes":
			self.console.query = { "nodes" : og.get_node_ids() }
		elif s == "edges":
			self.console.query = { "edges" : og.get_link_ids() }
		elif s == "all":
			self.console.query = {
				"nodes" : og.get_node_ids(),
				"edges" : og.get_link_ids()
			}	
		else:
			print("Error: Invalid query!")
			return

		print("Query:")
		for key in self.console.query.keys():
			print(". {0}: {1} element(s).".format(key, len(self.console.query[key])))

class Set(Script):
	def __init__(self, console):
		super(Set, self).__init__(console)

	def run(self, args):
		if len(args) < 4:
			print("Usage: {0} <type> <name> <value>".format(args[0]))
			return

		if 'nodes' in self.console.query:
			[ og.set_node_attribute(nid, args[2], args[1], " ".join(args[3:])) for nid in self.console.query['nodes'] ]
		if 'edges' in self.console.query:
			[ og.set_link_attribute(eid, args[2], args[1], " ".join(args[3:])) for eid in self.console.query['edges'] ]

class Remove(Script):
	def __init__(self, console):
		super(Remove, self).__init__(console)

	def run(self, args):
		if 'nodes' in self.console.query:
			[ og.remove_node(nid) for nid in self.console.query['nodes'] ]
		if 'edges' in self.console.query:
			[ og.remove_node(eid) for eid in self.console.query['edges'] ]

class Map(Script):
	def __init__(self, console):
		super(Map, self).__init__(console)

	def attr_convert(self, src_type, src_value, dst_type, options):
		if src_type != dst_type:
			raise Exception("Mapping from {0} to {1} not supported!".format(src_type, dst_type))
		print(src_type, src_value, dst_type)
		if dst_type == "vec2":
			return std.vec2_to_str(src_value)
		elif dst_type == "vec3":
			return std.vec3_to_str(value)
		elif dst_type == "vec4":
			return std.vec4_to_str(value)
		else:
			if len(options) == 2 and options[0] == "--format":
				value = options[1].format(src_value)
				return value
			else:
				return "{0}".format(src_value)

	def lambda_map(self, element_type, element_id, src_type, src_name, dst_type, dst_name, options = None):
		print(element_type, element_id, src_type, src_name, dst_type, dst_name)

		if element_type == "node":
			source = og.get_node_attribute(element_id, src_name)
	 		target = self.attr_convert(src_type, source, dst_type, options)
	 		print("og.set_node_attribute({0}, {1}, {2}, {3})".format(element_id, dst_name, dst_type, target))
			og.set_node_attribute(element_id, dst_name, dst_type, target)
		elif element_type == "edge":
			source = og.get_link_attribute(element_id, src_name)
	 		target = self.attr_convert(src_type, source, dst_type, options)
			og.set_link_attribute(element_id, dst_name, dst_type, target)

	def run(self, args):
		if len(args) < 6 and args[3] == 'to':
			print("Usage: {0} <src type> <src attribute> to <dst type> <dst attribute> [options]".format(args[0]))
			return

		if 'nodes' in self.console.query:
			for nid in self.console.query['nodes']:
				self.lambda_map("node", nid, args[1], args[2], args[4], args[5], args[6:])				
		if 'edges' in self.console.query:
			for eid in self.console.query['edges']:
				self.lambda_map("edge", eid, args[1], args[2], args[4], args[5], args[6:])			
		
class Help(Script):
	def __init__(self, console):
		super(Help, self).__init__(console)

	def run(self, args):
		print("Avalailable commands:")
		print(", ".join(self.console.context['scripts'].keys()))

# ----- Callbacks -----

class Console(object):
	def __init__(self):
		self.context = {
			"scripts" : {
				"query" : Query(self),
				"info" : Info(self),
				"load" : Load(self),
				"save" : Save(self),
				"set" : Set(self),
				"remove" : Remove(self),
				"map" : Map(self),
				"clear" : Clear(self),
				"color" : Color(self),
				"layout" : Layout(self),
				"camera" : Camera(self),
				"topo" : Topology(self),
				"test" : Test(self),
				"help" : Help(self),

				"opendns" : OpenDNS(self)
			}
		}
		self.query = dict()

	def execute(self, command):
		args = command.split()
		if 'scripts' in self.context and args[0] in self.context['scripts']:
			self.context['scripts'][args[0]].run(args)
		else:
			# TODO: og.console("{0}: Command not found!".format(args[0]))
			print("{0}: Command not found!".format(args[0]))
