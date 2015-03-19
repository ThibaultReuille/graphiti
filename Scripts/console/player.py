import script
from script import *

class Play(script.Script):
	
	def run(self, args):
		if len(args) == 2:
			if  args[1] == "camera":
				og.set_attribute('og:space:animation', 'bool', 'True')
			elif args[1] == "physics":
				og.set_attribute('og:space:physics', 'bool', 'True')

class Stop(script.Script):
	
	def run(self, args):
		if len(args) == 2:
			if  args[1] == "camera":
				og.set_attribute('og:space:animation', 'bool', 'False')
			elif args[1] == "physics":
				og.set_attribute('og:space:physics', 'bool', 'False')
