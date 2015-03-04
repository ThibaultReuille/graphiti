import sys

from Scripts import graphiti as og
from Scripts import console
        
console = console.Console()

def start():

    # ----- Initialization -----

    og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
    og.create_entity("graph")
    og.create_visualizer("space")

    # ----- Console Callback ------

    og.register_script("#console", 'demo.console.execute')

    # ----- Start -----

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            og.register_script('#started', 'load ' + sys.argv[2]) 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    og.start()
