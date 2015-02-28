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

from Scripts import graphiti as og
from Scripts import nx
from Scripts import std
from Scripts import console

def add_random_graph():
    node_count = int(raw_input("Number of nodes : "))
    edge_count = int(raw_input("Number of edges : "))

    nodes = []
    for i in range(node_count):
        id = og.add_node(str(i))

        og.set_node_attribute(id, "type", "string", str(i % 3))

        size = 1 + abs(math.sin(float(i)))
        og.set_node_attribute(id, "og:space:size", "float", str(size))

        nodes.append(id)

    for j in range(edge_count):
        loop = True
        while loop:
            src = random.choice(nodes)
            dst = random.choice(nodes)
            if src != dst:
                loop = False
        id = og.add_link(src, dst)
        og.set_link_attribute(id, "type", "string", str(j % 3))

def add_neighbor():
    if og.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    id = og.get_selected_node(0)
    label = raw_input("New node label : ")
    t = raw_input("New node type : ")

    nid = og.add_node(label)
    og.set_node_attribute(nid, "type", "string", t)

    og.add_link(id, nid)
        
def remove_selected_node():
    if og.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    id = og.get_selected_node(0)
    og.remove_node(id)

def reset_icons():
    for nid in og.get_node_ids():
        og.set_node_attribute(nid, "graphiti:space:icon", "string", "shapes/disk")

def attribute_to_icon(attribute, fun = lambda a : "shapes/disk"):
    for nid in og.get_node_ids():
        value = og.get_node_attribute(nid, attribute)
        og.set_node_attribute(nid, "graphiti:space:icon", "string", fun(value))

def countries_to_icons():
    def convert_cc_to_icon(country_code):
        if country_code is None:
            return "shapes/forbidden"
        else:
            return "countries/" + country_code.lower()
    attribute_to_icon("cc", convert_cc_to_icon)

def attribute_to_lod(attribute, fun = lambda x : x):
    min = float("inf")
    max = float("-inf")
    min_value = None
    max_value = None

    n_values = dict()
    for nid in og.get_node_ids():
        value = og.get_node_attribute(nid, attribute)
        if value is None:
            og.set_node_attribute(nid, "graphiti:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            og.set_node_attribute(nid, "graphiti:space:lod", "float", "-1000")
            continue
        fvalue = float(fun_value)
        if fvalue > max:
            max = fvalue
            max_value = value
        if fvalue < min:
            min = fvalue
            min_value = value
        n_values[nid] = fvalue

    e_values = dict()
    for eid in og.get_link_ids():
        value = og.get_link_attribute(eid, attribute)
        if value is None:
            og.set_link_attribute(eid, "graphiti:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            og.set_link_attribute(eid, "graphiti:space:lod", "float", "-1000")
            continue
        fvalue = float(fun_value)
        if fvalue > max:
            max = fvalue
            max_value = value
        if fvalue < min:
            min = fvalue
            min_value = value
        e_values[eid] = fvalue

    for nid in n_values.keys():
        svalue = (n_values[nid] - min) / (max - min)
        print(attribute + " : " + str(n_values[nid]) + " --> lod : " + str(svalue)) 
        og.set_node_attribute(nid, "graphiti:space:lod", "float", str(svalue))
    for eid in e_values.keys():
        svalue = (e_values[eid] - min) / (max - min)
        print(attribute + " : " + str(e_values[eid]) + " --> lod : " + str(svalue)) 
        og.set_link_attribute(eid, "graphiti:space:lod", "float", str(svalue))

    print("LOD range : " + str(min_value) + " to " + str(max_value))

def search_by_attribute(node_flag, edge_flag):
    pattern = raw_input("Expression : ")
    attribute = raw_input("Attribute : ")

    def f(t, id, match):
        if not match:
            if t == "node":
                og.set_node_attribute(id, "graphiti:space:lod", "float", "0.0")
            elif t == "edge":
                og.set_link_attribute(id, "graphiti:space:lod", "float", "0.0")

    std.regex_map(pattern, attribute, node_flag, edge_flag, f)

console = console.Console()

def start():

    # ----- Initialization -----

    og.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
    og.create_entity("graph")
    og.create_visualizer("space")

    # ----- Console Callback ------

    og.register_script("#console", 'demo.console.execute')

    Scripts = [
        ["Edition", [
            ["Country Icons", "demo.countries_to_icons()"],
            ["Depth to LOD", "demo.attribute_to_lod('depth', lambda a: a)"],
            ["Add Random Graph", "demo.add_random_graph()"],
            ["Add Neighbor", "demo.add_neighbor()"]
        ]]
    ]

    unreg_command = ""
    for script in Scripts:
        for s in script[1]:
            unreg_command += 'og.unregister_script("' + s[0] + '")\n'

    for script in Scripts:
        reg_command = ""
        for s in script[1]:
            reg_command += 'og.register_script("' + s[0] + '", "' + s[1] + '")\n'
        og.register_script(script[0], unreg_command + "\n" + reg_command) 

    og.register_script("==========", "pass")

    # ----- Start -----

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            og.register_script('#started', 'load ' + sys.argv[2]) 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    og.start()
