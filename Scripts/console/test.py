import script
from script import *

class Test(script.Script):

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
  	    for id in og.get_edge_ids():
	        activity = random.uniform(0.0, 2.0)
	        og.set_edge_attribute(id, "og:space:activity", "float", str(activity))

	def randomize_icons(self):
	    icons = glob.glob("./Assets/Countries/*.png")
	    for id in og.get_node_ids():
	        icon = "countries/" + random.choice(icons).split("/")[-1][:2].lower()
	        og.set_node_attribute(id, "og:space:icon", "string", icon)

	    icons = glob.glob("./Assets/SpaceView/EdgeStyles/*.png")
	    for id in og.get_edge_ids():
	        icon = "styles/" + random.choice(icons).split("/")[-1][:-4].lower()
	        og.set_edge_attribute(id, "og:space:icon", "string", icon)

	def randomize_lod(self):
	    for id in og.get_node_ids():
	        og.set_node_attribute(id, "og:space:lod", "float", str(random.random()))   
	    for id in og.get_edge_ids():
	        og.set_edge_attribute(id, "og:space:lod", "float", str(random.random()))   

	def randomize_node_size(self):
	    for id in og.get_node_ids():
	        activity = random.uniform(0.0, 5.0)
	        og.set_node_attribute(id, "og:space:size", "float", str(activity))

	def randomize_edge_width(self):
	    for id in og.get_edge_ids():
	        width = random.uniform(0.0, 5.0)
	        og.set_edge_attribute(id, "og:space:width", "float", str(width))


	def randomize_spheres(self):
		node_ids = og.get_node_ids()
		for s in range(0, og.count_nodes() / 100):
			sphere = og.add_sphere(str(s))
			for i in range(0, 1):
				og.tag_node(random.choice(node_ids), sphere)


	def randomize(self, args):
		if len(args) == 2 and args[1] == "activity":
			self.randomize_activity()
		elif len(args) == 2 and args[1] == "timeline":
			self.randomize_timeline()
		elif len(args) == 2 and args[1] == "lod":
			self.randomize_lod()
		elif len(args) == 2 and args[1] == "icons":
			self.randomize_icons()
		elif len(args) == 2 and args[1] == "spheres":
			self.randomize_spheres()
		elif len(args) == 3 and args[1] == "node" and args[2] == "size":
			self.randomize_node_size()
		elif len(args) == 3 and args[1] == "edge" and args[2] == "width":
			self.randomize_edge_width()
		else:
			self.console.log("Error: {0}: Wrong arguments!".format(args[0]))

	def debug(self, args):
	    flag = og.get_attribute("og:space:debug")
	    og.set_attribute("og:space:debug", "bool", str(not flag))

	def random_graph(self, args):
		node_count = 1000
		edge_count = 1000

		if len(args) >= 3:
			node_count = int(args[2])
			edge_count = node_count
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
		    id = og.add_edge(src, dst)
		    og.set_edge_attribute(id, "type", "string", str(j % 3))

	def add_neighbor(self, args):
		if og.count_selected_nodes() == 0:
		    self.console.log("Please select a node !")
		    return
		sid = og.get_selected_node(0)

		if len(args) >= 2:
		    label = args[1]
		    nid = og.add_node(label)
		    og.add_edge(sid, nid)

		    if len(args) >= 3:
		    	og.set_node_attribute(nid, "type", "string", args[2])
		else:
			self.console.log("Error: {0}: Wrong arguments!".format(args[0]))

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
			self.console.log("Error: {0}: Wrong arguments!".format(args[0]))