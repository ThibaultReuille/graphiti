from Scripts import rd
from Scripts import nx

import re
import fnmatch
import json
import random

def vec2_to_str(v):
    return str(v[0]) + " " + str(v[1])
def vec3_to_str(v):
    return str(v[0]) + " " + str(v[1]) + " " + str(v[2])
def vec4_to_str(v):
    return str(v[0]) + " " + str(v[1]) + " " + str(v[2]) + " " + str(v[3])

def random_vec4(r1, r2, r3, r4):
    return [
        r1[0] + random.random() * (r1[1] - r1[0]),  
        r2[0] + random.random() * (r2[1] - r2[0]),  
        r3[0] + random.random() * (r3[1] - r3[0]),  
        r4[0] + random.random() * (r4[1] - r4[0])
        ]

reserved_attributes = [ "id", "label", "src", "dst" ]

graph_attributes = [
    { 'name' : "raindance:space:title",        'type' : "string" }
]

node_attributes = [
    { 'name' : "type",                         'type' : 'string' },
    { 'name' : "depth",                        'type' : "float" },

    { 'name' : "sgraph:score",                 'type' : "float" },
    { 'name' : "sgraph:infected",              'type' : "int" },
    { 'name' : "sgraph:dga:score",             'type' : "float" },
    { 'name' : "sgraph:dga:perplexity",        'type' : "float" },
    { 'name' : "sgraph:dga:entropy",           'type' : "float" },
    { 'name' : "sgraph:first_seen",            'type' : 'string' },
    { 'name' : "sgraph:last_seen",             'type' : 'string' },

    { 'name' : "world:geolocation",            'type' : "vec2" },
    { 'name' : "world:country",                 'type' : "string" },

    { 'name' : "raindance:space:position",     'type' : "vec3" },
    { 'name' : "raindance:space:color",        'type' : "vec3" },
    { 'name' : "raindance:space:locked",       'type' : "bool" },
    { 'name' : "raindance:space:activity",      'type' : 'float' },
    { 'name' : "raindance:world:geolocation",  'type' : "vec2" },
    { 'name' : "raindance:world:color",        'type' : "vec4" },
    { 'name' : "raindance:world:size",         'type' : "float" },
    { 'name' : "raindance:particles:position", 'type' : "vec3" }
]

edge_attributes = [
    { 'name' : "type",                     'type' : 'string' },
    { 'name' : "depth",                    'type' : 'float' },

    { 'name' : "sgraph:first_seen",        'type' : 'string' },
    { 'name' : "sgraph:last_seen",         'type' : 'string' },

    { 'name' : "raindance:space:activity", 'type' : 'float' },
    { 'name' : "raindance:space:color1",   'type' : "vec4" },
    { 'name' : "raindance:space:color2",   'type' : "vec4" },
    { 'name' : "raindance:world:color",    'type' : "vec4" }
]

def info():
    print (str(rd.count_nodes()) + " nodes, " + str(rd.count_links()) + " links.")

def get_attribute_info(attribute):
    t = type(attribute)

    if (t is bool) or (t is int) or (t is float):
        return (t.__name__, str(attribute))
    if t is unicode:
        return ("string", attribute)
    elif t is list:
        l = len(attribute)
        count = sum(type(i) is float for i in attribute)
        if l != count or (l == count and (l < 2 or l > 4)):
            return None
        return ("vec" + str(l), filter(lambda x : not(x in "[,]"), str(attribute)))
    else:
        return None

def load_json(json_filename):
    rd_nodes = {}
    rd_links = {}
    global node_attributes
    global edge_attributes

    print ("Loading \"" + json_filename + "\" ...")

    import json
    with open(json_filename, "r") as infile:
        data = json.load(infile)

    print("Loading meta information ...")
    # TODO : Find a more generic way of doing this
    if "meta" in data:
        if "title" in data["meta"].keys():
            if data["meta"]["title"] is not None:
                rd.set_attribute("raindance:space:title", "string", data["meta"]["title"])

    if "attributes" in data:
        for key in data["attributes"].keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(data["attributes"][key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(n[key]) + "'!")
                continue
            rd.set_attribute(key, att_info[0], att_info[1])

    print(". Loading nodes ...")
    for n in data["nodes"]:
        label = ""
        if "label" in n:
            label = n["label"]
        nid = rd.add_node(label)
        rd_nodes[n["id"]] = nid

        # TODO : Should disappear
        if "raindance:model:weight" in n:
            rd.set_node_weight(nid, float(n["raindance:model:weight"]))
        if "raindance:model:mark" in n:
            rd.set_node_mark(nid, int(n["raindance:model:mark"]))

        for key in n.keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(n[key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(n[key]) + "'!")
                continue
            rd.set_node_attribute(nid, key, att_info[0], att_info[1])

    print(". Loading edges ...")
    for e in data["edges"]:
        eid = rd.add_link(rd_nodes[e["src"]], rd_nodes[e["dst"]])
        rd_links[e["id"]] = eid

        for key in e.keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(e[key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(n[key]) + "'!")
                continue
            rd.set_link_attribute(eid, key, att_info[0], att_info[1])


    if "timeline" in data:
        print(". Loading timeline ...")
        for c in data["timeline"]:
            print(c)
            rd.send_command(c[0], c[1], c[2])

    print("Done.")

def save_json(filename):
    
    graph = {}
    graph["meta"] = dict()
    graph["nodes"] = list()
    graph["edges"] = list()

    global node_attributes
    global edge_attributes
    
    print("Saving graph into '" + filename + "' ...")

    for id in rd.get_node_ids():
        node = dict()
        node["id"] = id
        node["label"] = rd.get_node_label(id)

        weight = rd.get_node_weight(id)
        if weight is not None:
            node["raindance:model:weight"] = weight
        mark = rd.get_node_mark(id)
        if mark is not None:
            node["raindance:model:mark"] = mark

        for attribute in node_attributes:
            name = attribute['name']
            value = rd.get_node_attribute(id, name)
            if value is None:
                continue
            node[name] = value

        graph["nodes"].append(node)

    for id in rd.get_link_ids():
        edge = dict()
        edge["id"] = id
        edge["src"] = rd.get_link_node1(id)
        edge["dst"] = rd.get_link_node2(id)

        for attribute in edge_attributes:
            name = attribute['name']
            value = rd.get_link_attribute(id, name)
            if value is None:
                continue
            edge[name] = value

        graph["edges"].append(edge)

    with open(filename, 'w') as outfile:
        json.dump(graph, outfile, indent=True, sort_keys=True)
    print("Done.")

def load_nx_graph():

    global node_attributes
    global edge_attributes
    
    graph = nx.Graph()
    
    print(rd.get_node_ids())

    for id in rd.get_node_ids():
        graph.add_node(id)
        graph.node[id]['label'] = rd.get_node_label(id)
        # graph.node[id]['raindance:model:type'] = rd.get_node_type(id)   <-- TODO ?
        graph.node[id]['raindance:model:mark'] = int(rd.get_node_mark(id))
        graph.node[id]['raindance:model:weight'] = float(rd.get_node_weight(id))

        for a in node_attributes:
            attribute = rd.get_node_attribute(id, a['name'])
            if not(attribute is None):
                value = str(attribute)
                if a['type'] == "vec2" or a['type'] == "vec3":
                    value = filter(lambda x: not (x in "[,]"), value)
                graph.node[id][a['name']] = value

    for id in rd.get_link_ids():
        node1 = rd.get_link_node1(id)
        node2 = rd.get_link_node2(id)
        graph.add_edge(node1, node2)
    
    return graph

def regex_map(expression, attribute, node_flag, edge_flag, f, translate = True):

    if translate:
        expression = fnmatch.translate(expression)
    r = re.compile(expression)
    if r is None:
        print("Invalid expression : <" + expression + "> !")
        return            

    if node_flag:
        for nid in rd.get_node_ids():
            value = None

            if attribute == "label":
                value = rd.get_node_label(nid)
            elif attribute == "mark":
                value = rd.get_node_mark(nid)
            elif attribute == "weight":
                value = rd.get_node_weight(nid)
            else:
                value = rd.get_node_attribute(nid, attribute)

            if value is None:
                f("node", nid, None)
            else:
                f("node", nid, r.match(str(value)))

    if edge_flag:
        for eid in rd.get_link_ids():
            value = None

            if attribute == "node1":
                value = rd.get_link_node1(eid)
            elif attribute == "node2":
                value = rd.get_link_node2(eid)
            else:
                value = rd.get_link_attribute(eid, attribute)

            if value is None:
                f("edge", eid, None)
            else:
                value_str = str(value)
                f("edge", eid, r.match(value_str))

