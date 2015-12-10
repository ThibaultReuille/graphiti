import script
from script import *

class Query(script.Script):
	def __init__(self, console):
		super(Query, self).__init__(console)


	def flip(self, entity_type):
		list1 = list()
		list2 = list()

		if entity_type in self.console.query:
			list1 = self.console.query[entity_type]
		
		if entity_type == "nodes":
			list2 = og.get_node_ids()
		elif entity_type == "edges":
			list2 = og.get_edge_ids()

		flipped = list()
		for oid in list2:
			if oid not in list1:
				flipped.append(oid)
		return flipped

	def run(self, args):

		if len(args) >= 2 and args[1] == "clear":
			self.console.query = dict()

		elif len(args) >= 2 and args[1] == "flip":

			if len(args) == 2 or (len(args) == 3 and args[2] == "all"):
				self.console.query = {
					'nodes' : self.flip("nodes"),
					'edges' : self.flip("edges")
				}
			elif len(args) == 3 and args[2] in ["nodes", "edges"]:
				flipped = self.flip(args[2])
				self.console.query = dict()
				self.console.query[args[2]] = flipped

		elif len(args) > 2 and args[1] == "run":

			s = " ".join(args[2:]).strip()
			if s == "nodes":
				self.console.query = { "nodes" : og.get_node_ids() }
			elif s == "edges":
				self.console.query = { "edges" : og.get_edge_ids() }
			elif s == "all":
				self.console.query = {
					"nodes" : og.get_node_ids(),
					"edges" : og.get_edge_ids()
				}	
			else:
				self.console.log("Error: Invalid query!")
				return
		else:
			self.console.log("Usage: {0} [clear|flip|run]".format(args[0]))
			return

		self.console.print_query()

class Select(script.Script):
	def __init__(self, console):
		super(Select, self).__init__(console)

	def run(self, args):
		if len(args) <= 1 or len(args) > 3:
			self.console.log("Usage: {0} <type> <pattern1> [pattern2]".format(args[0]))
			return

		found = list()

		if len(args) == 2:

			for nid in og.get_node_ids():
				label = og.get_node_label(nid)
				if label is None:
					continue
				if fnmatch.fnmatch(label, args[1]):
					found.append(nid)
			self.console.query = { 'nodes' : found }

		elif len(args) == 3:

			for eid in og.get_edge_ids():
				label1 = og.get_node_label(og.get_edge_node1(eid))
				label2 = og.get_node_label(og.get_edge_node2(eid))

				if label1 is None or label2 is None:
					continue

				if (
					fnmatch.fnmatch(label1, args[1]) and fnmatch.fnmatch(label2, args[2])
				) or (
					fnmatch.fnmatch(label2, args[1]) and fnmatch.fnmatch(label1, args[2])
				):
					found.append(eid)
			self.console.query = { 'edges' : found }

		self.console.print_query()

class Filter(script.Script):
	def __init__(self, console):
		super(Filter, self).__init__(console)
		self.attribute_cache = dict()

	def get_node_attribute(self, nid, attribute):
		if nid in self.attribute_cache:
			return self.attribute_cache[nid]

		if attribute == "label":
			return og.get_node_label(nid)
		else:
			return og.get_node_attribute(nid, attribute)

	def run(self, args):
		if len(args) <= 1 or len(args) > 4:
			self.console.log("Usage: {0} <attribute> <pattern1> [pattern2]".format(args[0]))
			return

		found = list()

		attribute = args[1]
		pattern1 = args[2]

		if len(args) == 3:

			self.attribute_cache = dict() # Clearing cache

			for nid in og.get_node_ids():
				value = self.get_node_attribute(nid, attribute)
				if value is None:
					continue
				if fnmatch.fnmatch(value, pattern1):
					found.append(nid)
			self.console.query = { 'nodes' : found }

		elif len(args) == 4:

			pattern2 = args[3]
			self.attribute_cache = dict() # Clearing cache

			for eid in og.get_edge_ids():
				value1 = self.get_node_attribute(og.get_edge_node1(eid), attribute)
				value2 = self.get_node_attribute(og.get_edge_node2(eid), attribute)

				if value1 is None or value2 is None:
					continue

				if (
					fnmatch.fnmatch(value1, pattern1) and fnmatch.fnmatch(value2, pattern2)
				) or (
					fnmatch.fnmatch(value2, pattern1) and fnmatch.fnmatch(value1, pattern2)
				):
					found.append(eid)
			self.console.query = { 'edges' : found }

		self.console.print_query()


