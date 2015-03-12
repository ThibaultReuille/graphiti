import script
from script import *

class Info(script.Script):
	def run(self, args):
		self.console.log("{0} nodes, {1} edges.".format(og.count_nodes(), og.count_edges()))


class Load(script.Script):
	def run(self, args):
		if len(args) < 2:
			self.console.log("Usage: {0} <filename>".format(args[0]))
			return

		std.load_json(" ".join(args[1:]))

class Save(script.Script):
	def run(self, args):
		if len(args) != 2:
			self.console.log("Usage: {0} <filename>".format(args[0]))
			return
		if os.path.isfile(args[1]):
			self.console.log("Error: File already exists!")
			return
		
		std.save_json(args[1])
		self.console.log("File saved in '{0}'.".format(args[1]))

class Clear(script.Script):

	def clear_graph(self):
	    for id in og.get_node_ids():
	        og.remove_node(id)

	def clear_colors(self):
	    og.set_attribute("og:space:edgemode", "string", "node_color")
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:color", "vec4", "1.0 1.0 1.0 1.0")

	def clear_icons(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:icon", "string", "shapes/disk")

	def clear_activity(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:activity", "float", "0.0")
		for e in og.get_edge_ids():
			og.set_edge_attribute(e, "og:space:activity", "float", "0.0")

	def clear_lod(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "og:space:lod", "float", "1.0")
		for e in og.get_edge_ids():
			og.set_edge_attribute(e, "og:space:lod", "float", "1.0")

	def run(self, args):
		if len(args) == 2 and args[1] == "graph":
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
			self.console.log("Usage: {0} [graph|colors|icons|activity|lod]".format(args[0]))

class Set(script.Script):
	def __init__(self, console):
		super(Set, self).__init__(console)

	def run(self, args):
		if len(args) < 4:
			self.console.log("Usage: {0} <type> <name> <value>".format(args[0]))
			return

		for key in self.console.query.keys():
			entity_type = key[:-1] # TODO : Hack!! find a better way to do this. This removes the ending 's'
			for entity_id in self.console.query[key]:
				self.console.api.set_attribute(entity_type, entity_id, args[2], args[1], " ".join(args[3:]))

class Get(script.Script):
	def __init__(self, console):
		super(Get, self).__init__(console)

	def run(self, args):
		if len(args) < 2:
			self.console.log("Usage: {0} <name>".format(args[0]))
			return

		for key in self.console.query.keys():
			entity_type = key[:-1] # TODO : Hack!! find a better way to do this. This removes the ending 's'
			
			result = dict()
			for entity_id in self.console.query[key]:
				result[entity_id] = self.console.api.get_attribute(entity_type, entity_id, args[1])
			self.console.log("{0}: {1}".format(key, json.dumps(result)))

class Remove(script.Script):
	def __init__(self, console):
		super(Remove, self).__init__(console)

	def run(self, args):
		if 'edges' in self.console.query:
			[ og.remove_edge(eid) for eid in self.console.query['edges'] ]
		if 'nodes' in self.console.query:
			[ og.remove_node(nid) for nid in self.console.query['nodes'] ]

class Map(script.Script):
	def __init__(self, console):
		super(Map, self).__init__(console)

	def attr_convert(self, src_type, src_value, dst_type, options):
		if src_type != dst_type:
			raise Exception("Mapping from {0} to {1} not supported!".format(src_type, dst_type))
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
		if element_type == "node":
			source = og.get_node_attribute(element_id, src_name)
	 		target = self.attr_convert(src_type, source, dst_type, options)
	 		self.console.log("og.set_node_attribute({0}, {1}, {2}, {3})".format(element_id, dst_name, dst_type, target))
			og.set_node_attribute(element_id, dst_name, dst_type, target)
		elif element_type == "edge":
			source = og.get_edge_attribute(element_id, src_name)
	 		target = self.attr_convert(src_type, source, dst_type, options)
			og.set_edge_attribute(element_id, dst_name, dst_type, target)

	def run(self, args):
		if len(args) < 6 and args[3] == 'to':
			self.console.log("Usage: {0} <src type> <src attribute> to <dst type> <dst attribute> [options]".format(args[0]))
			return

		if 'nodes' in self.console.query:
			for nid in self.console.query['nodes']:
				self.lambda_map("node", nid, args[1], args[2], args[4], args[5], args[6:])				
		if 'edges' in self.console.query:
			for eid in self.console.query['edges']:
				self.lambda_map("edge", eid, args[1], args[2], args[4], args[5], args[6:])	