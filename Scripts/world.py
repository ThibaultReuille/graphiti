from Scripts import math
from Scripts import random

import sys
import datetime
import thread
import time
import fnmatch
import re
import os
import glob

from Scripts import graphiti
from Scripts import nx
from Scripts import std

cities = [
    {
        "name" : "Paris",
        "geolocation": [48.8742, 2.3470],
        "color" : [1.0, 1.0, 0.0, 1.0],
        "size" : 1.0
    },
    {
        "name" : "San Francisco",
        "geolocation" : [37.7750, -122.4183],
        "color" : [1.0, 0.0, 0.0, 1.0],
        "size" : 2.0
    },
]

def on_started():
    global cities

    dico = {}

    for city in cities:
        nid = graphiti.add_node(city["name"])
        graphiti.set_node_attribute(nid, "og:world:geolocation", "vec2", std.vec2_to_str(city["geolocation"]))
        graphiti.set_node_attribute(nid, "og:world:color", "vec4", std.vec4_to_str(city["color"]))
        graphiti.set_node_attribute(nid, "og:world:size", "float", str(city["size"]))
        dico[city['name']] = nid

    eid = graphiti.add_edge(dico['Paris'], dico['San Francisco'])
    graphiti.set_edge_attribute(eid, "og:world:color", "vec4", "0.0 1.0 0.0 1.0")

def start():

    Scripts = [
        ["Edition", [
            ["Info", "std.info()"],
            ["Save", "std.save_json(raw_input('Output filename : '))"],
            ["-", "pass"],
        ]],
    ]

    unreg_command = ""
    for script in Scripts:
        for s in script[1]:
            unreg_command += 'graphiti.unregister_script("' + s[0] + '")\n'

    for script in Scripts:
        reg_command = ""
        for s in script[1]:
            reg_command += 'graphiti.register_script("' + s[0] + '", "' + s[1] + '")\n'

        graphiti.register_script(script[0], unreg_command + "\n" + reg_command) 

    graphiti.register_script("==========", "pass")

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            graphiti.register_script('#started', 'std.load_json("' + sys.argv[2] + '")') 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    # graphiti.register_script("#started", "world.on_started()")

    graphiti.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
    graphiti.create_entity("graph")
    graphiti.create_visualizer("world")
    graphiti.start()
