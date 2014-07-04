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

# Disabling Sgraph commands for now
#SGRAPH_API_PATH = "../../sgraph-api"
#sys.path.insert(0, SGRAPH_API_PATH)
#import sgraph as sgraph

def clear_graph():
    for id in graphiti.get_node_ids():
        graphiti.remove_node(id)

def clear_colors():
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")
    for n in graphiti.get_node_ids():
        graphiti.set_node_attribute(n, "graphiti:space:color", "vec4", "1.0 1.0 1.0 1.0")

def clear_icons():
    for n in graphiti.get_node_ids():
        graphiti.set_node_attribute(n, "graphiti:space:icon", "string", "shapes/disk")

def clear_lod():
    for id in graphiti.get_node_ids():
        graphiti.set_node_attribute(id, "graphiti:space:lod", "float", "1.0")
    for id in graphiti.get_link_ids():
        graphiti.set_link_attribute(id, "graphiti:space:lod", "float", "1.0")

def add_random_graph():
    node_count = int(raw_input("Number of nodes : "))
    edge_count = int(raw_input("Number of edges : "))

    nodes = []
    for i in range(node_count):
        id = graphiti.add_node(str(i))
        graphiti.set_node_weight(id, float(1 + abs(math.sin(float(i)))))
        graphiti.set_node_attribute(id, "type", "string", str(i % 3))
        nodes.append(id)

    for j in range(edge_count):
        loop = True
        while loop:
            src = random.choice(nodes)
            dst = random.choice(nodes)
            if src != dst:
                loop = False
        id = graphiti.add_link(src, dst)
        graphiti.set_link_attribute(id, "type", "string", str(j % 3))
            
def remove_selected_node():
    if graphiti.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    id = graphiti.get_selected_node(0)
    graphiti.remove_node(id)

def randomize_node_activity():
    for id in graphiti.get_node_ids():
        activity = random.uniform(0.0, 5.0)
        graphiti.set_node_attribute(id, "graphiti:space:activity", "float", str(activity))

def randomize_edge_activity():
    for id in graphiti.get_link_ids():
        activity = random.uniform(0.0, 2.0)
        graphiti.set_link_attribute(id, "graphiti:space:activity", "float", str(activity))

def randomize_edge_width():
    for id in graphiti.get_link_ids():
        width = random.uniform(0.0, 5.0)
        graphiti.set_link_attribute(id, "graphiti:space:width", "float", str(width))

def randomize_node_icons():
    icons = glob.glob("./Resources/Countries/*.png")
    print(icons)
    for id in graphiti.get_node_ids():
        icon = "countries/" + random.choice(icons).split("/")[-1][:2].lower()
        graphiti.set_node_attribute(id, "graphiti:space:icon", "string", icon)

def randomize_edge_icons():
    icons = glob.glob("./Resources/SpaceView/EdgeStyles/*.png")
    print(icons)
    for id in graphiti.get_link_ids():
        icon = "styles/" + random.choice(icons).split("/")[-1][:-4].lower()
        graphiti.set_link_attribute(id, "graphiti:space:icon", "string", icon)

def reset_icons():
    for nid in graphiti.get_node_ids():
        graphiti.set_node_attribute(nid, "graphiti:space:icon", "string", "shapes/disk")

def attribute_to_icon(attribute, fun = lambda a : "shapes/disk"):
    for nid in graphiti.get_node_ids():
        value = graphiti.get_node_attribute(nid, attribute)
        graphiti.set_node_attribute(nid, "graphiti:space:icon", "string", fun(value))

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
    for nid in graphiti.get_node_ids():
        value = graphiti.get_node_attribute(nid, attribute)
        if value is None:
            graphiti.set_node_attribute(nid, "graphiti:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            graphiti.set_node_attribute(nid, "graphiti:space:lod", "float", "-1000")
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
    for eid in graphiti.get_link_ids():
        value = graphiti.get_link_attribute(eid, attribute)
        if value is None:
            graphiti.set_link_attribute(eid, "graphiti:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            graphiti.set_link_attribute(eid, "graphiti:space:lod", "float", "-1000")
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
        graphiti.set_node_attribute(nid, "graphiti:space:lod", "float", str(svalue))
    for eid in e_values.keys():
        svalue = (e_values[eid] - min) / (max - min)
        print(attribute + " : " + str(e_values[eid]) + " --> lod : " + str(svalue)) 
        graphiti.set_link_attribute(eid, "graphiti:space:lod", "float", str(svalue))

    print("LOD range : " + str(min_value) + " to " + str(max_value))

def lock_unlock():
    if graphiti.count_selected_nodes() == 0:
        print ("Please select a node !")
        return

    selected = graphiti.get_selected_node(0)
    print("SELECTED : ", selected)
    locked = graphiti.get_node_attribute(selected, "graphiti:space:locked")
    if locked:
        graphiti.set_node_attribute(selected, "graphiti:space:locked", "bool", "False")
        graphiti.set_node_mark(selected, 0)
        print("Node " + str(selected) + " unlocked.")
    else:
        graphiti.set_node_attribute(selected, "graphiti:space:locked", "bool", "True")
        graphiti.set_node_mark(selected, 2)
        print("Node " + str(selected) + " locked.")

def color_neighbors():
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    if graphiti.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    
    selected = graphiti.get_selected_node(0)
    graph = std.load_nx_graph()
    neighbors = graph.neighbors(selected)
    graphiti.set_node_attribute(selected, "graphiti:space:color", "vec3", "0.0 1.0 1.0")
    for node in neighbors:
        graphiti.set_node_attribute(node, "graphiti:space:lod", "float", "1.0")
        graphiti.set_node_attribute(node, "graphiti:space:color", "vec3", "0.0 1.0 1.0")

def show_high_degrees():
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * float(deg) / float(max_degree)

        color = graphiti.get_node_attribute(n[0], "graphiti:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        color[3] = 1.0
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        graphiti.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

def show_low_degrees():
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * (1.0 - float(deg) / float(max_degree))

        color = graphiti.get_node_attribute(n[0], "graphiti:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        graphiti.set_node_attribute(n[0], "graphiti:space:color", "vec3", c)

def color_by_node_degree():
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    print("Building node degree table ...")
    edges = graphiti.get_link_ids()
    degree_table = dict()
    for eid in edges:
        nid1 = graphiti.get_link_node1(eid)
        nid2 = graphiti.get_link_node2(eid)
        if nid1 not in degree_table:
            degree_table[nid1] = { "in" : 0, "out" : 0 }
        if nid2 not in degree_table:
            degree_table[nid2] = { "in" : 0, "out" : 0 }
        degree_table[nid1]["out"] += 1
        degree_table[nid2]["in"] += 1

    print("Randomizing color map ...")
    m = dict()
    m["isolated"] = std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])
    m["leaf"] = std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])
    m["source"] = std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])
    m["sink"] = std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])
    m["other"] = std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])
    print(m)

    print("Coloring ...")
    for nid in graphiti.get_node_ids():
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
        graphiti.set_node_attribute(nid, "graphiti:space:color", "vec4", std.vec4_to_str(m[t]))

def sphere_layout(radius):
    graph = std.load_nx_graph()
    for n in graph.nodes(data = True):
        r1 = random.random() * 2 * math.pi
        r2 = random.random() * 2 * math.pi
        r3 = 0.9 + 0.1 * random.random()

        x = radius * r3 * math.sin(r1) * math.cos(r2)
        y = radius * r3 * math.cos(r1)
        z = radius * r3 * math.sin(r1) * math.sin(r2)

        position = str(x) + " " + str(y) + " " + str(z)
        graphiti.set_node_attribute(n[0], "graphiti:space:position", "vec3", position)

def cube_layout():
    graph = std.load_nx_graph()
    node_count = graph.number_of_nodes()

    size = int(node_count ** (1.0 / 3.0))

    node_count = 0
    for n in graph.nodes(data = True):
        x = node_count % size
        y = node_count / (size * size)
        z = (node_count % (size * size)) / size

        x = 5 * (x - size / 2)
        y = 5 * (y - size / 2)
        z = 5 * (z - size / 2)

        position = str(x) + " " + str(y) + " " + str(z)
        graphiti.set_node_attribute(n[0], "graphiti:space:position", "vec3", position)
        node_count += 1

def conic_layout():
    graph = std.load_nx_graph()

    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    sorted_degrees = sorted(nx.degree(graph).values())
    max_degree = sorted_degrees[-1]

    degree_nodes = {}
    for n in graph.nodes(data = True):
        degree = nx.degree(graph, n[0])
        if degree in degree_nodes:
            degree_nodes[degree].append(n[0])
        else:
            degree_nodes[degree] = [n[0]]

    max_radius = 30.0
    max_height = 20
    for n in graph.nodes(data = True):
        degree = nx.degree(graph, n[0])

        nodes = degree_nodes[degree]

        radius = 1.0 + max_radius * float(1.0 - float(degree) / float(max_degree))
        alpha = 2.0 * math.pi * random.random() #float(nodes.index(n[0])) / float(len(nodes)) 
        # 3D
        # beta = 2.0 * math.pi * random.random() #float(nodes.index(n[0])) / float(len(nodes)) 

        x = radius * math.cos(alpha)
        y = max_height * float(degree) / float(max_degree)
        z = radius * math.sin(alpha)
        # 3D
        # x = radius * math.sin(alpha) * math.cos(beta)
        # y = radius * math.sin(alpha) * math.sin(beta)
        # z = radius * math.cos(alpha)

        graphiti.set_node_attribute(n[0], "graphiti:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))

def show_connected_components():

    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    cc = nx.connected_components(graph)

    for list in cc:
        r = random.random()
        g = random.random()
        b = random.random()
        color = str(r) + " " + str(g) + " " + str(b) 
        
        for node in list:
            graphiti.set_node_attribute(node, "graphiti:space:color", "vec3", color)

def color_nodes_by_nominal_attribute(attribute_name):

    graphiti.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = graphiti.get_node_ids()
    colors = dict()

    for id in ids:
        type = graphiti.get_node_attribute(id, attribute_name)
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
        graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", color)

def color_nodes_by_type():

    graphiti.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = graphiti.get_node_ids()
    colors = dict()

    for id in ids:
        type = graphiti.get_node_attribute(id, "type")
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
        graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", color)

def color_edges_by_type():

    graphiti.set_attribute("graphiti:space:linkmode", "string", "edge_color")

    ids = graphiti.get_link_ids()
    colors = dict()

    for id in ids:
        type = graphiti.get_link_attribute(id, "type")
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
        graphiti.set_link_attribute(id, "graphiti:space:color", "vec3", color)

def color_nodes_by_score():
    
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = graphiti.get_node_ids()
    for id in ids:
        score = graphiti.get_node_attribute(id, "sgraph:score")
        if score is None:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < -50:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 50:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "0.0 1.0 0.0")
        else:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_infected():
    
    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = graphiti.get_node_ids()
    for id in ids:
        score = graphiti.get_node_attribute(id, "sgraph:infected")
        if score is None:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < 0:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 0:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "0.0 1.0 0.0")
        else:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_dga_score():

    graphiti.set_attribute("graphiti:space:linkmode", "string", "node_color")

    ids = graphiti.get_node_ids()
    for id in ids:
        score = graphiti.get_node_attribute(id, "sgraph:dga:score")
        if score is None:
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec4", "0.5 0.5 0.5 0.5")
            continue
        else:
            # DGA score is between [0 : not DGA, 100 : DGA]
            sub = score / 100;
            rgb = [1.0, 1.0 - sub, 1.0 - sub, 1.0]
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec4", std.vec4_to_str(rgb))

def depth_circle_layout():
    
    radius = 100.0

    ids = graphiti.get_node_ids()
    zeros = []
    for id in ids:
        attribute = graphiti.get_node_attribute(id, "depth")
        if attribute == None:
            continue
        else:
            if attribute == 0:
                zeros.append(id)

    count = 0
    if len(zeros) == 1:
        radius = 0.0
    for id in zeros:
        angle = 2.0 * math.pi * float(count) / float(len(zeros))

        x = radius * math.cos(angle)
        y = 0.0
        z = radius * math.sin(angle)

        graphiti.set_node_attribute(id, "graphiti:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))
        graphiti.set_node_attribute(id, "graphiti:space:locked", "bool", "True")
        graphiti.set_node_attribute(id, "graphiti:space:activity", "float", "2.0")
        graphiti.set_node_mark(id, 2)
        count += 1

def globe_coordinates(latitude, longitude, delta = 0.0):
    globe_radius = 50
    radLatitude = latitude * math.pi / 180.0;
    radLongitude = longitude * math.pi / 180.0;

    return [
        (globe_radius + delta) * math.cos(radLatitude) * math.cos(radLongitude),
        (globe_radius + delta) * math.sin(radLatitude),
        - (globe_radius + delta) * math.cos(radLatitude) * math.sin(radLongitude)
    ]

def globe_layout():
    ids = graphiti.get_node_ids()
    for id in ids:
        geo = graphiti.get_node_attribute(id, "world:geolocation")
        if geo is not None:
           pos = globe_coordinates(geo[0], geo[1])
           graphiti.set_node_attribute(id, "graphiti:space:position", "vec3", std.vec3_to_str(pos))
           graphiti.set_node_attribute(id, "graphiti:space:locked", "bool", "true")

def randomize_timeline():
    ids = graphiti.get_node_ids()
    time = 1000
    for nid in ids:
        r = 1.0
        g = 0.0
        b = 0.0
        a = 1.0
        color = str(r) + " " + str(g) + " " + str(b) + " " + str(a)
        graphiti.send_command(time, "graph:set_node_attribute",
                        { "id" : int(random.choice(ids)), "type" : "vec4", "name" : "graphiti:space:color", "value" : color }) 
        time += 500

def search_by_attribute(node_flag, edge_flag):
    pattern = raw_input("Expression : ")
    attribute = raw_input("Attribute : ")

    def f(t, id, match):
        if not match:
            if t == "node":
                graphiti.set_node_attribute(id, "graphiti:space:lod", "float", "0.0")
            elif t == "edge":
                graphiti.set_link_attribute(id, "graphiti:space:lod", "float", "0.0")

    std.regex_map(pattern, attribute, node_flag, edge_flag, f)

def multiply_colors(v, node_flag, edge_flag):
    def f(c, v):
        return [c[0] * v[0], c[1] * v[1], c[2] * v[2], c[3] * v[3]]
        

    if node_flag:
        for id in graphiti.get_node_ids():
            c = graphiti.get_node_attribute(id, "graphiti:space:color")
            graphiti.set_node_attribute(id, "graphiti:space:color", "vec4", std.vec4_to_str(f(c, v)))

    if edge_flag:
        for id in graphiti.get_link_ids():
            c = graphiti.get_link_attribute(id, "graphiti:space:color1")
            graphiti.set_link_attribute(id, "graphiti:space:color1", "vec4", std.vec4_to_str(f(c, v)))
            c = graphiti.get_link_attribute(id, "graphiti:space:color2")
            graphiti.set_link_attribute(id, "graphiti:space:color2", "vec4", std.vec4_to_str(f(c, v)))

def calculate_degree_map():
    degrees = dict()

    for eid in graphiti.get_link_ids():
        bi = False
        e_type = graphiti.get_link_attribute(eid, "type")
        if e_type is not None and "<->" in e_type:
            bi = True

        nid1 = graphiti.get_link_node1(eid)
        nid2 = graphiti.get_link_node2(eid)

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

    for eid in graphiti.get_link_ids():
        src = graphiti.get_link_node1(eid)
        if src not in degree_map:
            continue
        
        if degree_map[src]["in"] == 0 and degree_map[src]["out"] >= 0:
            dst = graphiti.get_link_node2(eid)
            if src not in source_map:
                source_map[src] = [(dst, eid)]
            elif dst not in source_map[src]:
                source_map[src].append((dst, eid))
    
    colors = [
        std.vec4_to_str(std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])),     # Source
        std.vec4_to_str(std.random_vec4([0, 1], [0, 1], [0, 1], [1, 1])),     # Source Successors
        std.vec4_to_str(std.random_vec4([0, 1], [0, 1], [0, 1], [0.1, 0.5]))  # Others
    ]

    for nid in graphiti.get_node_ids():
        graphiti.set_node_attribute(nid, "graphiti:space:lod", "float", "0.0")
        graphiti.set_node_attribute(nid, "graphiti:space:color", "vec4", colors[2])
        
    for eid in graphiti.get_link_ids():
        graphiti.set_link_attribute(eid, "graphiti:space:lod", "float", "0.0")

    for source in source_map.keys():
        graphiti.set_node_attribute(source, "graphiti:space:lod", "float", "1.0")
        graphiti.set_node_attribute(source, "graphiti:space:color", "vec4", colors[0])

        for successor in source_map[source]:
            graphiti.set_node_attribute(successor[0], "graphiti:space:lod", "float", "1.0")
            graphiti.set_node_attribute(successor[0], "graphiti:space:color", "vec4", colors[1])
            graphiti.set_link_attribute(successor[1], "graphiti:space:lod", "float", "1.0")

    print("SPN detection results :")
    print(source_map)

def start():

    Scripts = [
        ["Edition", [
            ["Info", "std.info()"],
            ["Save", "std.save_json(raw_input('Output filename : '))"],

            ["-", "pass"],

            ["Clear Graph", "demo.clear_graph()"],
            ["Clear Colors", "demo.clear_colors()"],
            ["Clear Icons", "demo.clear_icons()"],
            ["Clear LOD", "demo.clear_lod()"],
            ["Add Random Graph", "demo.add_random_graph()"],
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
        ["Arrangement", [
            ["Lock / Unlock", "demo.lock_unlock()"],
            ["Point Layout", "demo.sphere_layout(0)"],
            ["Sphere Layout", "demo.sphere_layout(20)"],
            ["Cube Layout", "demo.cube_layout()"],
            ["Conic Layout", "demo.conic_layout()"],
            ["Depth Circle Layout", "demo.depth_circle_layout()"],
            ["Globe Layout", "demo.globe_layout()"],
        ]],
        ["Topology", [
            ["Neighbors", "demo.color_neighbors()"],
            ["Connected Components", "demo.show_connected_components()"],
            ["High Degrees", "demo.show_high_degrees()"],
            ["Low Degrees", "demo.show_low_degrees()"],
            ["Node Connections", "demo.color_by_node_degree()"],
            ["Detect SPN", "demo.detect_spn()"]
        ]],
        ["Animation", [
            ["Start", "graphiti.set_attribute('graphiti:space:animation', 'bool', 'True')"],
            ["Stop", "graphiti.set_attribute('graphiti:space:animation', 'bool', 'False')"]
        ]],
        ["Security", [
            ["Umbrella Score", "demo.color_nodes_by_score()"],
            ["Infected Score", "demo.color_nodes_by_infected()"],
            ["DGA Score", "demo.color_nodes_by_dga_score()"],
        ]],
        ["Test / Debug", [
            ["Randomize Node Activity", "demo.randomize_node_activity()"],
            ["Randomize Node Icons", "demo.randomize_node_icons()"],
            ["Randomize Edge Activity", "demo.randomize_edge_activity()"],
            ["Randomize Edge Icons", "demo.randomize_edge_icons()"],
            ["Randomize Edge Width", "demo.randomize_edge_width()"],
            ["Randomize Timeline", "demo.randomize_timeline()"],
            ["Build Octree", "graphiti.set_attribute('graphiti:space:octree:update', 'bool', 'True')"]
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

    # TODO : Add entity support in python scripts
    #wid = graphiti.add_window("OpenGraphiti : Data Visualization Engine", 1024, 728)
    #g = graphiti.create_entity("graph")
    #v = graphiti.create_view("graph:view", g)
    #c = graphiti.create_controller("graph:controller", g)
    #graphiti.bind_entity(g)
    #graphiti.send(g, { "message" : "Hello Graph!" })
    #graphiti.destroy_entity(g)

    graphiti.start("space")
