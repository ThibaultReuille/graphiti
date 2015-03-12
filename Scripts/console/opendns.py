import script
from script import *

class OpenDNS(script.Script):

	def score(self):
	
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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
	    
	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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

	    og.set_attribute("graphiti:space:edgemode", "string", "node_color")

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