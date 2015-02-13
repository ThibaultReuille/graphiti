from Scripts import graphiti as og
from Scripts import std

import sys

def start():

	# --- Load JSON dataset if defined ---
    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            og.register_script('#started', 'std.load_json("' + sys.argv[2] + '")') 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")	

    # ---
    
	og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
	og.create_entity("graph")
	og.create_visualizer("network")

	#og.register_script("test", "print('yes')")

	og.start()