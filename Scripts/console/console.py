import script
from script import *

import edition
import layout
import query
import player
import test
import graph
import opendns

class Color(script.Script):
	
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
			"black" : [ 0.0, 0.0, 0.0, 1.0 ],
			"orange" : [ 1.0, 0.4, 0.0, 1.0 ],
			"purple" : [ 0.5, 0, 0.5, 1.0],
			"pink" : [ 1.0, 0.75, 0.79, 1.0 ],
			"brown" : [ 0.64, 0.16, 0.16, 1.0 ]
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
			og.set_edge_attribute(element_id, "og:space:color", "vec4", color)

	def lambda_by(self, element_type, element_id, attr, color_map):
		if element_type not in color_map:
			color_map[element_type] = dict()

		if element_type == "node":
			value = og.get_node_attribute(element_id, attr)
		elif element_type == "edge":
			value = og.get_edge_attribute(element_id, attr)

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
			og.set_edge_attribute(element_id, "og:space:color", "vec4", color)

	def lambda_op(self, element_type, element_id, op, color_mask, factor):

		def calculate(op, v1, v2, mask):
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
				self.console.log("Error: '{0}': Unknown operator!")
				return

			for i in xrange(4):
				if not mask[i]:
					r[i] = v1[i]
			return r

		if element_type == "node":
			color = og.get_node_attribute(element_id, "og:space:color")
			og.set_node_attribute(element_id, "og:space:color", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))
		elif element_type == "edge":
			color = og.get_edge_attribute(element_id, "og:space:color1")
			og.set_edge_attribute(element_id, "og:space:color1", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))
			color = og.get_edge_attribute(element_id, "og:space:color2")
			og.set_edge_attribute(element_id, "og:space:color2", "vec4", std.vec4_to_str(calculate(op, color, factor, color_mask)))

	def run(self, args):

		query = self.console.query
		if query is None:
			self.console.log("Error: Query is empty!")
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
				self.console.log("Error: '{0}': Unknown color mask!".format(args[2]))
				return

			array = [ float(i) for i in " ".join(args[3:]).split() ]

			if len(array) == 1:
				factor = [ array[0], array[0], array[0], array[0] ]
			elif len(array) == 3:
				factor = [ array[0], array[1], array[2], 1.0 ]
			elif len(array) == 4:
				factor = [ array[0], array[1], array[2], array[3] ]
			else:
				self.console.log("Error: Can't parse color factor!")
				return

			if 'nodes' in query:
				[ self.lambda_op("node", nid, args[1], self.color_masks[args[2]], factor) for nid in query['nodes'] ]
			if 'edges' in query:
				[ self.lambda_op("edge", eid, args[1], self.color_masks[args[2]], factor) for eid in query['edges'] ]
	
class Help(script.Script):
	def __init__(self, console):
		super(Help, self).__init__(console)

	def run(self, args):
		self.console.log("Avalailable commands:")
		self.console.log(", ".join(self.console.context['scripts'].keys()))
		
class Quit(script.Script):
	def __init__(self, console):
		super(Quit, self).__init__(console)
		
	def run(self, args):
		self.console.log("Terminating OpenGraphiti...")
		og.quit()

class Native(script.Script):
	def __init__(self, console):
		super(Native, self).__init__(console)

	def run(self, args):
		exec(" ".join(args[1:]))
		
# ----- Callbacks -----

class OpenGraphiti(object):

	def __init__(self):

		self.ids = {
			"node" : og.get_node_ids,
			"edge" : og.get_edge_ids
		}

		self.setters = {
			"graph" : og.set_attribute,
			"node" : og.set_node_attribute,
			"edge" : og.set_edge_attribute,
		}

		self.getters = {
			"graph" : og.get_attribute,
			"node" : og.get_node_attribute,
			"edge" : og.get_edge_attribute,
		}

	def get_ids(self, entity_type):
		if entity_type in self.ids:
			return self.ids[entity_type]()
		raise Exception("{0}: Unknown entity type!".format(entity_type))

	def set_attribute(self, entity_type, entity_id, attr_name, attr_type, attr_value):
		if entity_type in self.setters:
			return self.setters[entity_type](entity_id, attr_name, attr_type, attr_value)
		raise Exception("{0}: Unknown entity type!".format(entity_type))

	def get_attribute(self, entity_type, entity_id, attr_name):
		if entity_type in self.getters:
			return self.getters[entity_type](entity_id, attr_name)
		raise Exception("{0}: Unknown entity type!".format(entity_type))

class Console(object):
	def __init__(self):
		self.context = {
			"scripts" : {
				"info" : edition.Info(self),
				"load" : edition.Load(self),
				"save" : edition.Save(self),
				"set" : edition.Set(self),
				"get" : edition.Get(self),
				"remove" : edition.Remove(self),
				"map" : edition.Map(self),
				"clear" : edition.Clear(self),

				"find" : query.Find(self),
				"filter" : query.Filter(self),
				"query" : query.Query(self),

				"layout" : layout.Layout(self),
				"play" : player.Play(self),
				"stop" : player.Stop(self),

				"topo" : graph.Topology(self),

				"test" : test.Test(self),

				"help" : Help(self),
				"color" : Color(self),
				
				"quit" : Quit(self),

				"opendns" : opendns.OpenDNS(self),

				"py" : Native(self)
			}
		}
		self.query = dict()
		self.api = OpenGraphiti()

	def log(self, text):
		og.console({ 'log' : text })

	def print_query(self):
		s = "Entities: "
		key_count = 0
		for key in self.query.keys():
			if key_count > 0:
				s += ", "
			s += "#{0}={1}".format(key, len(self.query[key]))
			key_count += 1
		self.log(s)

	def execute(self, command):
		args = command.split()
		if 'scripts' in self.context and args[0] in self.context['scripts']:
			self.context['scripts'][args[0]].run(args)
		else:
			# TODO: og.console("{0}: Command not found!".format(args[0]))
			self.log("{0}: Command not found!".format(args[0]))
