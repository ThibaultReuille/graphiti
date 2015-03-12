import script
from script import *

class Camera(script.Script):
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
			self.console.log("Usage: {0} [play|stop]".format(args[0]))	