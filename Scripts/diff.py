from Scripts import graphiti
from Scripts import nx
from Scripts import std
from Scripts import demo

import sys

colors = {
    "removed": "1.0 0.4 0.4",
    "added": "0.7 1.0 0.4",
    "modified": "1.0 1.0 0.4",
    "same": "1.0 1.0 1.0"
}

def color_diff():
    global colors
    for n in graphiti.get_node_ids():
        s = graphiti.get_node_attribute(n, 'diffstatus')
        graphiti.set_node_attribute(n, "graphiti:space:color", "vec3", colors[s])
    for e in graphiti.get_link_ids():
        s = graphiti.get_link_attribute(e, 'diffstatus')
        try:
            graphiti.set_link_attribute(e, "graphiti:space:color", "vec3", colors[s])
        except KeyError:
            print("link: {}".format(e))
            print("diffstatus: {}".format(s))
            sys.exit(-1)

def search_by_attribute(node_flag, edge_flag):
    pattern = raw_input("Expression : ")
    attribute = raw_input("Attribute : ")

    def f(t, id, match):
        if not match:
            if t == "node":
                graphiti.set_node_attribute(id, "graphiti:space:lod", "float", "0.0")
            elif t == "edge":
                graphiti.set_link_attribute(id, "graphiti:space:lod", "float", "0.0")

    std.regex_map(pattern, attribute, node_flag, edge_flag, f, False)

def start():
    Scripts = [
        ["Edition", [
            ["Info", "std.info()"],
            ["Color Diff", 'diff.color_diff()'],

            ["-", "pass"],

            ["Clear Graph", "demo.clear_graph()"],
            ["Clear Colors", "demo.clear_colors()"],
            ["Clear Icons", "demo.clear_icons()"],
            ["Clear LOD", "demo.clear_lod()"],
            ["Node Type", "demo.color_nodes_by_type()"],
            ["Edge Type", "demo.color_edges_by_type()"],
            ["Country Icons", "demo.countries_to_icons()"],
            ["Depth to LOD", "demo.attribute_to_lod('depth', lambda a: a)"],
            ["Search by Node Attribute", "diff.search_by_attribute(True, False)"],
            ["Search by Link Attribute", "diff.search_by_attribute(False, True)"],
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
        ]]
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
            graphiti.register_script('#started', 'std.load_json("' + sys.argv[2] + '"); diff.color_diff()') 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    graphiti.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
    graphiti.create_entity("graph")
    graphiti.create_view("space")
    graphiti.start()
