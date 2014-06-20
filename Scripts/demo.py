from Scripts import math
from Scripts import random

import sys
import datetime
import thread
import time
import fnmatch
import re
import os

from Scripts import rd
from Scripts import nx
from Scripts import std

# Disabling Sgraph commands for now
#SGRAPH_API_PATH = "../../sgraph-api"
#sys.path.insert(0, SGRAPH_API_PATH)
#import sgraph as sgraph

def clear_graph():
    for id in rd.get_node_ids():
        rd.remove_node(id)

def clear_colors():
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")
    for n in rd.get_node_ids():
        rd.set_node_attribute(n, "raindance:space:color", "vec4", "1.0 1.0 1.0 1.0")

def clear_icons():
    for n in rd.get_node_ids():
        rd.set_node_attribute(n, "raindance:space:icon", "string", "shapes/disk")

def clear_lod():
    for id in rd.get_node_ids():
        rd.set_node_attribute(id, "raindance:space:lod", "float", "1.0")
    for id in rd.get_link_ids():
        rd.set_link_attribute(id, "raindance:space:lod", "float", "1.0")

def add_random_graph():
    node_count = int(raw_input("Number of nodes : "))
    edge_count = int(raw_input("Number of edges : "))

    nodes = []
    for i in range(node_count):
        id = rd.add_node(str(i))
        rd.set_node_weight(id, float(1 + abs(math.sin(float(i)))))
        rd.set_node_attribute(id, "type", "string", str(i % 3))
        nodes.append(id)

    for j in range(edge_count):
        loop = True
        while loop:
            src = random.choice(nodes)
            dst = random.choice(nodes)
            if src != dst:
                loop = False
        id = rd.add_link(src, dst)
        rd.set_link_attribute(id, "type", "string", str(j % 3))
            
def remove_selected_node():
    if rd.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    id = rd.get_selected_node(0)
    rd.remove_node(id)

def randomize_node_activity():
    for id in rd.get_node_ids():
        activity = random.uniform(0.0, 5.0)
        rd.set_node_attribute(id, "raindance:space:activity", "float", str(activity))

def randomize_edge_activity():
    for id in rd.get_link_ids():
        activity = random.uniform(0.0, 2.0)
        rd.set_link_attribute(id, "raindance:space:activity", "float", str(activity))

def randomize_node_icons():
    icons = glob.glob("./Resources/Countries/*.png")
    print(icons)
    for id in rd.get_node_ids():
        icon = "countries/" + random.choice(icons).split("/")[-1][:2].lower()
        rd.set_node_attribute(id, "raindance:space:icon", "string", icon)

def reset_icons():
    for nid in rd.get_node_ids():
        rd.set_node_attribute(nid, "raindance:space:icon", "string", "shapes/disk")

def attribute_to_icon(attribute, fun = lambda a : "shapes/disk"):
    for nid in rd.get_node_ids():
        value = rd.get_node_attribute(nid, attribute)
        rd.set_node_attribute(nid, "raindance:space:icon", "string", fun(value))

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
    for nid in rd.get_node_ids():
        value = rd.get_node_attribute(nid, attribute)
        if value is None:
            rd.set_node_attribute(nid, "raindance:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            rd.set_node_attribute(nid, "raindance:space:lod", "float", "-1000")
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
    for eid in rd.get_link_ids():
        value = rd.get_link_attribute(eid, attribute)
        if value is None:
            rd.set_link_attribute(eid, "raindance:space:lod", "float", "-1000")
            continue
        fun_value = fun(value)
        if fun_value is None:
            rd.set_link_attribute(eid, "raindance:space:lod", "float", "-1000")
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
        rd.set_node_attribute(nid, "raindance:space:lod", "float", str(svalue))
    for eid in e_values.keys():
        svalue = (e_values[eid] - min) / (max - min)
        print(attribute + " : " + str(e_values[eid]) + " --> lod : " + str(svalue)) 
        rd.set_link_attribute(eid, "raindance:space:lod", "float", str(svalue))

    print("LOD range : " + str(min_value) + " to " + str(max_value))

def lock_unlock():
    if rd.count_selected_nodes() == 0:
        print ("Please select a node !")
        return

    selected = rd.get_selected_node(0)
    print("SELECTED : ", selected)
    locked = rd.get_node_attribute(selected, "raindance:space:locked")
    if locked:
        rd.set_node_attribute(selected, "raindance:space:locked", "bool", "False")
        rd.set_node_mark(selected, 0)
        print("Node " + str(selected) + " unlocked.")
    else:
        rd.set_node_attribute(selected, "raindance:space:locked", "bool", "True")
        rd.set_node_mark(selected, 2)
        print("Node " + str(selected) + " locked.")

def color_neighbors():
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    if rd.count_selected_nodes() == 0:
        print("Please select a node !")
        return
    
    selected = rd.get_selected_node(0)
    graph = std.load_nx_graph()
    neighbors = graph.neighbors(selected)
    rd.set_node_attribute(selected, "raindance:space:color", "vec3", "0.0 1.0 1.0")
    for node in neighbors:
        rd.set_node_attribute(node, "raindance:space:lod", "float", "1.0")
        rd.set_node_attribute(node, "raindance:space:color", "vec3", "0.0 1.0 1.0")

def show_high_degrees():
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * float(deg) / float(max_degree)

        color = rd.get_node_attribute(n[0], "raindance:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        color[3] = 1.0
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        rd.set_node_attribute(n[0], "raindance:space:color", "vec3", c)

def show_low_degrees():
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    max_degree = max(nx.degree(graph).values())
    for n in graph.nodes(data = True):
        deg = nx.degree(graph, n[0])
        tint = 0.3 + 0.9 * (1.0 - float(deg) / float(max_degree))

        color = rd.get_node_attribute(n[0], "raindance:space:color")
        color[0] = tint * color[0]
        color[1] = tint * color[1]
        color[2] = tint * color[2]
        c = str(color[0]) + " " + str(color[1]) + " " + str(color[2])

        rd.set_node_attribute(n[0], "raindance:space:color", "vec3", c)

def color_by_node_degree():
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    print("Building node degree table ...")
    edges = rd.get_link_ids()
    degree_table = dict()
    for eid in edges:
        nid1 = rd.get_link_node1(eid)
        nid2 = rd.get_link_node2(eid)
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
    for nid in rd.get_node_ids():
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
        rd.set_node_attribute(nid, "raindance:space:color", "vec4", std.vec4_to_str(m[t]))

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
        rd.set_node_attribute(n[0], "raindance:space:position", "vec3", position)

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
        rd.set_node_attribute(n[0], "raindance:space:position", "vec3", position)
        node_count += 1

def conic_layout():
    graph = std.load_nx_graph()

    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

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

        rd.set_node_attribute(n[0], "raindance:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))

def show_connected_components():

    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    graph = std.load_nx_graph()
    cc = nx.connected_components(graph)

    for list in cc:
        r = random.random()
        g = random.random()
        b = random.random()
        color = str(r) + " " + str(g) + " " + str(b) 
        
        for node in list:
            rd.set_node_attribute(node, "raindance:space:color", "vec3", color)

def color_nodes_by_nominal_attribute(attribute_name):

    rd.set_attribute("raindance:space:linkmode", "string", "edge_color")

    ids = rd.get_node_ids()
    colors = dict()

    for id in ids:
        type = rd.get_node_attribute(id, attribute_name)
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
        rd.set_node_attribute(id, "raindance:space:color", "vec3", color)

def color_nodes_by_type():

    rd.set_attribute("raindance:space:linkmode", "string", "edge_color")

    ids = rd.get_node_ids()
    colors = dict()

    for id in ids:
        type = rd.get_node_attribute(id, "type")
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
        rd.set_node_attribute(id, "raindance:space:color", "vec3", color)

def color_edges_by_type():

    rd.set_attribute("raindance:space:linkmode", "string", "edge_color")

    ids = rd.get_link_ids()
    colors = dict()

    for id in ids:
        type = rd.get_link_attribute(id, "type")
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
        rd.set_link_attribute(id, "raindance:space:color", "vec3", color)

def color_nodes_by_score():
    
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    ids = rd.get_node_ids()
    for id in ids:
        score = rd.get_node_attribute(id, "sgraph:score")
        if score is None:
            rd.set_node_attribute(id, "raindance:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < -50:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 50:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "0.0 1.0 0.0")
        else:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_infected():
    
    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    ids = rd.get_node_ids()
    for id in ids:
        score = rd.get_node_attribute(id, "sgraph:infected")
        if score is None:
            rd.set_node_attribute(id, "raindance:space:color", "vec4", "0.5 0.5 0.5 0.5")
        elif score < 0:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "1.0 0.0 0.0")
        elif score > 0:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "0.0 1.0 0.0")
        else:
            rd.set_node_attribute(id, "raindance:space:color", "vec3", "1.0 1.0 1.0")

def color_nodes_by_dga_score():

    rd.set_attribute("raindance:space:linkmode", "string", "node_color")

    ids = rd.get_node_ids()
    for id in ids:
        score = rd.get_node_attribute(id, "sgraph:dga:score")
        if score is None:
            rd.set_node_attribute(id, "raindance:space:color", "vec4", "0.5 0.5 0.5 0.5")
            continue
        else:
            # DGA score is between [0 : not DGA, 100 : DGA]
            sub = score / 100;
            rgb = [1.0, 1.0 - sub, 1.0 - sub, 1.0]
            rd.set_node_attribute(id, "raindance:space:color", "vec4", std.vec4_to_str(rgb))

def depth_circle_layout():
    
    radius = 100.0

    ids = rd.get_node_ids()
    zeros = []
    for id in ids:
        attribute = rd.get_node_attribute(id, "depth")
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

        rd.set_node_attribute(id, "raindance:space:position", "vec3", str(x) + " " + str(y) + " " + str(z))
        rd.set_node_attribute(id, "raindance:space:locked", "bool", "True")
        rd.set_node_attribute(id, "raindance:space:activity", "float", "2.0")
        rd.set_node_mark(id, 2)
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
    ids = rd.get_node_ids()
    for id in ids:
        geo = rd.get_node_attribute(id, "world:geolocation")
        if geo is not None:
           pos = globe_coordinates(geo[0], geo[1])
           rd.set_node_attribute(id, "raindance:space:position", "vec3", std.vec3_to_str(pos))
           rd.set_node_attribute(id, "raindance:space:locked", "bool", "true")

def randomize_timeline():
    ids = rd.get_node_ids()
    time = 1000
    for nid in ids:
        r = 1.0
        g = 0.0
        b = 0.0
        a = 1.0
        color = str(r) + " " + str(g) + " " + str(b) + " " + str(a)
        rd.send_command(time, "graph:set_node_attribute",
                        { "id" : int(random.choice(ids)), "type" : "vec4", "name" : "raindance:space:color", "value" : color }) 
        time += 500

def search_by_attribute(node_flag, edge_flag):
    pattern = raw_input("Expression : ")
    attribute = raw_input("Attribute : ")

    def f(t, id, match):
        if not match:
            if t == "node":
                rd.set_node_attribute(id, "raindance:space:lod", "float", "0.0")
            elif t == "edge":
                rd.set_link_attribute(id, "raindance:space:lod", "float", "0.0")

    std.regex_map(pattern, attribute, node_flag, edge_flag, f)

def multiply_colors(v, node_flag, edge_flag):
    def f(c, v):
        return [c[0] * v[0], c[1] * v[1], c[2] * v[2], c[3] * v[3]]
        

    if node_flag:
        for id in rd.get_node_ids():
            c = rd.get_node_attribute(id, "raindance:space:color")
            rd.set_node_attribute(id, "raindance:space:color", "vec4", std.vec4_to_str(f(c, v)))

    if edge_flag:
        for id in rd.get_link_ids():
            c = rd.get_link_attribute(id, "raindance:space:color1")
            rd.set_link_attribute(id, "raindance:space:color1", "vec4", std.vec4_to_str(f(c, v)))
            c = rd.get_link_attribute(id, "raindance:space:color2")
            rd.set_link_attribute(id, "raindance:space:color2", "vec4", std.vec4_to_str(f(c, v)))

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
            ["Dim Nodes", "demo.multiply_colors([0.8, 0.8, 0.8, 1.0], True, False)"],
            ["Dim Edges", "demo.multiply_colors([0.8, 0.8, 0.8, 1.0], False, True)"],
            ["Brighten Nodes", "demo.multiply_colors([1.2, 1.2, 1.2, 1.0], True, False)"],
            ["Brighten Edges", "demo.multiply_colors([1.2, 1.2, 1.2, 1.0], False, True)"],
            ["Dim Nodes Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 0.8], True, False)"],
            ["Dim Edges Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 0.8], False, True)"],
            ["Brighten Nodes Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 1.2], True, False)"],
            ["Brighten Edges Alpha", "demo.multiply_colors([1.0, 1.0, 1.0, 1.2], False, True)"],
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
        ]],
        ["Animation", [
            ["Start", "rd.set_attribute('raindance:space:animation', 'bool', 'True')"],
            ["Stop", "rd.set_attribute('raindance:space:animation', 'bool', 'False')"]
        ]],
        ["Security", [
            ["Umbrella Score", "demo.color_nodes_by_score()"],
            ["Infected Score", "demo.color_nodes_by_infected()"],
            ["Infected Score (Artsy)", "demo.color_nodes_by_nominal_attribute('sgraph:infected')"],
            ["DGA Score", "demo.color_nodes_by_dga_score()"],
        ]],
        ["Test / Debug", [
            ["Randomize Node Activity", "demo.randomize_node_activity()"],
            ["Randomize Edge Activity", "demo.randomize_edge_activity()"],
            ["Randomize Node Icons", "demo.randomize_node_icons()"],
            ["Randomize Timeline", "demo.randomize_timeline()"],
            ["Build Octree", "rd.set_attribute('raindance:space:octree:update', 'bool', 'True')"]
        ]],
    ]

    unreg_command = ""
    for script in Scripts:
        for s in script[1]:
            unreg_command += 'rd.unregister_script("' + s[0] + '")\n'

    for script in Scripts:
        reg_command = ""
        for s in script[1]:
            reg_command += 'rd.register_script("' + s[0] + '", "' + s[1] + '")\n'

        rd.register_script(script[0], unreg_command + "\n" + reg_command) 

    rd.register_script("==========", "pass")

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            rd.register_script('#started', 'std.load_json("' + sys.argv[2] + '")') 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    #wid = rd.add_window("Raindance", 1024, 728)
    #g = rd.create_entity("graph")
    #v = rd.create_view("graph:view", g)
    #c = rd.create_controller("graph:controller", g)
    #rd.bind_entity(g)
    #rd.send(g, { "message" : "Hello Graph!" })
    #rd.destroy_entity(g)

    rd.start("space")
