import script
from script import *

class Topology(script.Script):

	def neighbors(self, args):
		new_neighbors = list()
		if 'nodes' in self.console.query:
			graph = std.load_nx_graph()
			for nid in self.console.query['nodes']:
				for neighbor in graph.neighbors(nid):
					if neighbor not in self.console.query['nodes']:
						new_neighbors.append(neighbor)

			for nn in new_neighbors:
				self.console.query['nodes'].append(nn)
		self.console.print_query()

	def connected_components(self, args):
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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
	    for id in og.get_edge_ids():
	        og.set_edge_attribute(id, "og:space:icon", "string", "styles/triangles")

	def connections(self, args):
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

	    # Building node degree table ...
	    edges = og.get_edge_ids()
	    degree_table = dict()
	    for eid in edges:
	        nid1 = og.get_edge_node1(eid)
	        nid2 = og.get_edge_node2(eid)
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
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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
			self.console.log("Error: {0}: Wrong arguments!".format(args[0]))

	def get_degree_map(self):
	    degrees = dict()

	    for eid in og.get_edge_ids():
	        bi = False
	        e_type = og.get_edge_attribute(eid, "type")
	        if e_type is not None and "<->" in e_type:
	            bi = True

	        nid1 = og.get_edge_node1(eid)
	        nid2 = og.get_edge_node2(eid)

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

	    for eid in og.get_edge_ids():
	        src = og.get_edge_node1(eid)
	        if src not in degree_map:
	            continue
	        
	        if degree_map[src]["in"] == 0 and degree_map[src]["out"] >= 0:
	            dst = og.get_edge_node2(eid)
	            if src not in source_map:
	                source_map[src] = [(dst, eid)]
	            elif dst not in source_map[src]:
	                source_map[src].append((dst, eid))

	    for nid in og.get_node_ids():
	        og.set_node_attribute(nid, "og:space:lod", "float", "0.0")
	        
	    for eid in og.get_edge_ids():
	        og.set_edge_attribute(eid, "og:space:lod", "float", "0.0")

	    for source in source_map.keys():
	        og.set_node_attribute(source, "og:space:lod", "float", "1.0")

	        for successor in source_map[source]:
	            og.set_node_attribute(successor[0], "og:space:lod", "float", "1.0")
	            og.set_edge_attribute(successor[1], "og:space:lod", "float", "1.0")

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
			self.console.log("Error: {0}: Wrong arguments!".format(args[0]))
