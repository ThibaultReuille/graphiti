import sys

from Scripts import graphiti as og
from Scripts import console
        
shell = console.console.Console()

def start():

    # ----- Initialization -----

    og.create_window("OpenGraphiti : Data Visualization Engine", 1024, 728)
    og.create_entity("graph")
    og.create_visualizer("space")
    og.create_visualizer("shell")
    og.create_visualizer("logo")

    # ----- Console Callback ------

    og.register_script("#console", 'demo.shell.execute')

    # ----- Start -----

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            og.register_script('#started', 'load ' + sys.argv[2]) 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    og.start()
