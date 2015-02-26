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

def set_lod(value):
    for id in og.get_node_ids():
        og.set_node_attribute(id, "graphiti:space:lod", "float", str(value))
    for id in og.get_link_ids():
        og.set_link_attribute(id, "graphiti:space:lod", "float", str(value))

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



def show_edge_direction():
    for id in og.get_link_ids():
        og.set_link_attribute(id, "og:space:icon", "string", "styles/triangles")



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

def lock_unlock():
    if og.count_selected_nodes() == 0:
        print ("Please select a node !")
        return

    selected = og.get_selected_node(0)
    print("SELECTED : ", selected)
    locked = og.get_node_attribute(selected, "graphiti:space:locked")
    if locked:
        og.set_node_attribute(selected, "graphiti:space:locked", "bool", "False")
        og.set_node_attribute(selected, "og:space:mark", "int", "0");
        print("Node " + str(selected) + " unlocked.")
    else:
        og.set_node_attribute(selected, "graphiti:space:locked", "bool", "True")
        og.set_node_attribute(selected, "og:space:mark", "int", "2");
        print("Node " + str(selected) + " locked.")

def color_neighbors():
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    if og.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    
    selected = og.get_selected_node(0)
    graph = std.load_nx_graph()
    neighbors = graph.neighbors(selected)
    og.set_node_attribute(selected, "graphiti:space:color", "vec3", "0.0 1.0 1.0")
    for node in neighbors:
        og.set_node_attribute(node, "graphiti:space:lod", "float", "1.0")
        og.set_node_attribute(node, "graphiti:space:color", "vec3", "0.0 1.0 1.0")

def show_high_degrees():
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * float(deg) / float(max_degree)

        color = og.get_node_attribute(n[0], "graphiti:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        color[3] = 1.0
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        og.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

def show_low_degrees():
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * (1.0 - float(deg) / float(max_degree))

        color = og.get_node_attribute(n[0], "graphiti:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        og.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

def color_by_node_degree():
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    print("Building node degree table ...")
    edges = og.get_link_ids()
    degree_table = dict()
    for eid in edges:
        nid1 = og.get_link_node1(eid)
        nid2 = og.get_link_node2(eid)
        if nid1 not in degree_table:
            degree_table[nid1] = { "in" : 0, "out" : 0 }
        if nid2 not in degree_table:
            degree_table[nid2] = { "in" : 0, "out" : 0 }
        degree_table[nid1]["out"] += 1
        degree_table[nid2]["in"] += 1

    print("Randomizing color map ...")
    m = dict()
    m["isolated"] = [0.95, 0.98, 0.36, 1.0]
    m["leaf"] = [0.06, 0.94, 0.61, 1.0]
    m["source"] = [0.91, 0.18, 0.17, 1.0]
    m["sink"] = [0.03, 0.65, 0.94, 1.0]
    m["other"] = [0.77, 0.78, 0.75, 1.0]

    print(m)

    print("Coloring ...")
    for nid in og.get_node_ids():
        if nid not in degree_table:
            t = "isolated"
        else:
            deg = degree_table[nid]
            if deg["in"] == 0 and deg["out"] == 1:
                t = "leaf"
            elif deg["in"] == 0 and deg["out"] > 1:
                t = "source"
            elif deg["in"] > 0 and deg["out"] == 0:
                t = "sink"
            else:
                t = "other"
        og.set_node_attribute(nid, "graphiti:space:color", "vec4", std.vec4_to_str(m[t]))

def show_connected_components():

    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    cc = nx.connected_components(graph)

    for list in cc:
        r = random.random()
        g = random.random()
        b = random.random()
        color = str(r) + " " + str(g) + " " + str(b) 
        
        for node in list:
            og.set_node_attribute(node, "graphiti:space:color", "vec3", color)

def color_nodes_by_nominal_attribute(attribute_name):

    og.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = og.get_node_ids()
    colors = dict()

    for id in ids:
        type = og.get_node_attribute(id, attribute_name)
        if type == None:
            print("Node " + str(id) + " has no <" + attribute_name + "> attribute !")
        color = None
        if type in colors:
            color = colors[type]
        else:
            r = random.random()
            g = random.random()
            b = random.random()
            color = str(r) + " " + str(g) + " " + str(b)
            colors[type] = color
        og.set_node_attribute(id, "graphiti:space:color", "vec3", color)

def color_nodes_by_type():

    og.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = og.get_node_ids()
    colors = dict()

    for id in ids:
        type = og.get_node_attribute(id, "type")
        if type == None:
            print("Node " + str(id) + " has no type attribute !")
        color = None
        if type in colors:
            color = colors[type]
        else:
            r = random.random()
            g = random.random()
            b = random.random()
            color = str(r) + " " + str(g) + " " + str(b)
            colors[type] = color
        og.set_node_attribute(id, "graphiti:space:color", "vec3", color)

def color_edges_by_type():

    og.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = og.get_link_ids()
    colors = dict()

    for id in ids:
        type = og.get_link_attribute(id, "type")
        if type == None:
            print("Edge " + str(id) + " has no type attribute !")
        color = None
        if type in colors:
            color = colors[type]
        else:
            r = random.random()
            g = random.random()
            b = random.random()
            color = str(r) + " " + str(g) + " " + str(b)
            colors[type] = color
        og.set_link_attribute(id, "graphiti:space:color", "vec3", color)

def color_nodes_by_score():
    
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = og.get_node_ids()
    for id in ids:
        score = og.get_node_attribute(id, "sgraph:score")
        if score is None:
            og.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < -50:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 50:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "0.0 1.0 0.0")
        else:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_infected():
    
    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = og.get_node_ids()
    for id in ids:
        score = og.get_node_attribute(id, "sgraph:infected")
        if score is None:
            og.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < 0:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 0:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "0.0 1.0 0.0")
        else:
            og.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_dga_score():

    og.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = og.get_node_ids()
    for id in ids:
        score = og.get_node_attribute(id, "sgraph:dga:score")
        if score is None:
            og.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
            continue
        else:
            # DGA score is between [0 : not DGA, 100 : DGA]
            sub = score / 100;
            rgb = [1.0, 1.0 - sub, 1.0 - sub, 1.0]
            og.set_node_attribute(id, "graphiti:space:color", "vec4", std.vec4_to_str(rgb))

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

def multiply_colors(v, node_flag, edge_flag):
    def f(c, v):
        return [c[0] * v[0], c[1] * v[1], c[2] * v[2], c[3] * v[3]]
        

    if node_flag:
        for id in og.get_node_ids():
            c = og.get_node_attribute(id, "graphiti:space:color")
            og.set_node_attribute(id, "graphiti:space:color", "vec4", std.vec4_to_str(f(c, v)))

    if edge_flag:
        for id in og.get_link_ids():
            c = og.get_link_attribute(id, "graphiti:space:color1")
            og.set_link_attribute(id, "graphiti:space:color1", "vec4", std.vec4_to_str(f(c, v)))
            c = og.get_link_attribute(id, "graphiti:space:color2")
            og.set_link_attribute(id, "graphiti:space:color2", "vec4", std.vec4_to_str(f(c, v)))

def calculate_degree_map():
    degrees = dict()

    for eid in og.get_link_ids():
        bi = False
        e_type = og.get_link_attribute(eid, "type")
        if e_type is not None and "<->" in e_type:
            bi = True

        nid1 = og.get_link_node1(eid)
        nid2 = og.get_link_node2(eid)

        if nid1 not in degrees:
            degrees[nid1] = { "in" : 0, "out" : 0 }
        if nid2 not in degrees:
            degrees[nid2] = { "in" : 0, "out" : 0 }
        
        if bi:
            degrees[nid1]["in"] += 1
            degrees[nid1]["out"] += 1
            degrees[nid2]["in"] += 1
            degrees[nid2]["out"] += 1
        else:
            degrees[nid1]["out"] += 1
            degrees[nid2]["in"] += 1

    return degrees

def detect_spn():
    degree_map = calculate_degree_map()
    source_map = dict()

    for eid in og.get_link_ids():
        src = og.get_link_node1(eid)
        if src not in degree_map:
            continue
        
        if degree_map[src]["in"] == 0 and degree_map[src]["out"] >= 0:
            dst = og.get_link_node2(eid)
            if src not in source_map:
                source_map[src] = [(dst, eid)]
            elif dst not in source_map[src]:
                source_map[src].append((dst, eid))

    for nid in og.get_node_ids():
        og.set_node_attribute(nid, "graphiti:space:lod", "float", "0.0")
        
    for eid in og.get_link_ids():
        og.set_link_attribute(eid, "graphiti:space:lod", "float", "0.0")

    for source in source_map.keys():
        og.set_node_attribute(source, "graphiti:space:lod", "float", "1.0")

        for successor in source_map[source]:
            og.set_node_attribute(successor[0], "graphiti:space:lod", "float", "1.0")
            og.set_link_attribute(successor[1], "graphiti:space:lod", "float", "1.0")

    print("SPN detection results :")
    print(source_map)

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
            ["Clear LOD", "demo.set_lod(1.0)"],
            ["Zero LOD", "demo.set_lod(0.0)"],
            ["Node Type", "demo.color_nodes_by_type()"],
            ["Edge Type", "demo.color_edges_by_type()"],   
            ["Country Icons", "demo.countries_to_icons()"],
            ["Color Attribute", "demo.color_nodes_by_nominal_attribute(raw_input('Attribute : '))"],
            ["Depth to LOD", "demo.attribute_to_lod('depth', lambda a: a)"],
            ["Search by Node Attribute", "demo.search_by_attribute(True, False)"],
            ["Search by Link Attribute", "demo.search_by_attribute(False, True)"],
        ]],
        ["Colors", [
            ["Dim Nodes", "demo.multiply_colors([0.7, 0.7, 0.7, 1.0], True, False)"],
            ["Dim Edges", "demo.multiply_colors([0.7, 0.7, 0.7, 1.0], False, True)"],
            ["Brighten Nodes", "demo.multiply_colors([1.3, 1.3, 1.3, 1.0], True, False)"],
            ["Brighten Edges", "demo.multiply_colors([1.3, 1.3, 1.3, 1.0], False, True)"],
            ["Dim Nodes Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 0.7], True, False)"],
            ["Dim Edges Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 0.7], False, True)"],
            ["Brighten Nodes Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 1.3], True, False)"],
            ["Brighten Edges Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 1.3], False, True)"],
        ]],
        ["Topology", [
            ["Neighbors", "demo.color_neighbors()"],
            ["Connected Components", "demo.show_connected_components()"],
            ["Edge Directions", "demo.show_edge_direction()"],
            ["High Degrees", "demo.show_high_degrees()"],
            ["Low Degrees", "demo.show_low_degrees()"],
            ["Node Connections", "demo.color_by_node_degree()"],
            ["Detect SPN", "demo.detect_spn()"]
        ]],
        ["Animation", [
            ["Start", "og.set_attribute('og:space:animation', 'bool', 'True')"],
            ["Stop", "og.set_attribute('og:space:animation', 'bool', 'False')"]
        ]],
        ["Security", [
            ["Umbrella Score", "demo.color_nodes_by_score()"],
            ["Infected Score", "demo.color_nodes_by_infected()"],
            ["DGA Score", "demo.color_nodes_by_dga_score()"],
        ]],
        ["Test / Debug", [
            ["Add Random Graph", "demo.add_random_graph()"],
            ["Add Neighbor", "demo.add_neighbor()"],
            ["Debug On/Off", "demo.show_debug()"]
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
