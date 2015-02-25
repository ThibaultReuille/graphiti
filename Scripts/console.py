from Scripts import graphiti as og
from Scripts import std

import sys
import os.path

class Script(object):
	def run(self, args):
		print("Error: run() method not implemented!")

class Info(Script):
	def run(self, args):
		std.info()

class Load(Script):
	def run(self, args):
		if len(args) != 2:
			# TODO: og.console("Usage: {0} <filename>".format(args[0]))
			print("Usage: {0} <filename>".format(args[0]))
			return

		std.load_json(args[1])

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
	    og.set_attribute("graphiti:space:linkmode", "string", "node_color")
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "graphiti:space:color", "vec4", "1.0 1.0 1.0 1.0")

	def clear_icons(self):
	    for n in og.get_node_ids():
	        og.set_node_attribute(n, "graphiti:space:icon", "string", "shapes/disk")

	def run(self, args):
		if len(args) == 1:
			self.clear_graph()
		elif len(args) == 2 and args[1] == "colors":
			self.clear_colors()
		elif len(args) == 2 and args[1] == "icons":
			self.clear_icons()
		else:
			print("Usage: {0} [colors|icons]".format(args[0]))
			# TODO : og.console()

class Layout(Script):
	def cube(self):
		pass

	def sphere(self):
		pass

	def cone(self):
		pass

	def globe(self):
		pass

	def seeds(self):
		pass

	def usage(self, args):
		print("Usage: {0} [cube|sphere|cone|globe|seeds]".format(args[0]))

	def run(self, args):
		if len(args) != 2:
			self.usage(args)
			return

		if args[1] == "cube":
			self.cube()
		elif args[1] == "sphere":
			self.sphere()
		elif args[1] == "cone":
			self.cone()
		elif args[1] == "globe":
			self.globe()
		elif args[1] == "seeds":
			self.seeds()
		else:
			self.usage(args)

class Test(Script):
	def run(self, args):
		pass

# ----- Callbacks -----


class Console(object):
	def __init__(self):
		self.context = {
			"scripts" : {
				"info" : Info(),
				"load" : Load(),
				"save" : Save(),
				"clear" : Clear(),
				"layout" : Layout(),
				"test" : Test()
			}
		}

	def execute(self, command):
		args = command.split()
		if 'scripts' in self.context and args[0] in self.context['scripts']:
			self.context['scripts'][args[0]].run(args)
		else:
			# TODO: og.console("{0}: Command not found!".format(args[0]))
			print("{0}: Command not found!".format(args[0]))
