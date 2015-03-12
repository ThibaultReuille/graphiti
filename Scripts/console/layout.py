import script
from script import *

class Layout(script.Script):
	
	def point(self):
		ids = og.get_node_ids()
		for nid in ids:
		    og.set_node_attribute(nid, "og:space:position", "vec3", "0.0 0.0 0.0")

	def sphere(self, args):
		radius = 20
		if len(args) == 2:
			radius = float(args[1])

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

	    og.set_attribute("og:space:edgemode", "string", "node_color")

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
		self.console.log("Usage: {0} [point|cube|sphere|cone|globe|seeds]".format(args[0]))

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
