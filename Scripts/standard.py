from Scripts import graphiti
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
    { 'name' : "type",                         'type' : "string" },
    { 'name' : "depth",                        'type' : "float" },
    { 'name' : "label",                        'type' : "string"},

    { 'name' : "sgraph:score",                 'type' : "float" },
    { 'name' : "sgraph:infected",              'type' : "int" },
    { 'name' : "sgraph:dga:score",             'type' : "float" },
    { 'name' : "sgraph:dga:perplexity",        'type' : "float" },
    { 'name' : "sgraph:dga:entropy",           'type' : "float" },
    { 'name' : "sgraph:first_seen",            'type' : "string" },
    { 'name' : "sgraph:last_seen",             'type' : "string" },

    { 'name' : "world:geolocation",            'type' : "vec2" },
    { 'name' : "world:country",                 'type' : "string" },

    { 'name' : "og:space:position",     'type' : "vec3" },
    { 'name' : "og:space:color",        'type' : "vec4" },
    { 'name' : "og:space:locked",       'type' : "bool" },
    { 'name' : "og:space:lod",          'type' : "float" },
    { 'name' : "og:space:activity",     'type' : "float" },
    { 'name' : "og:space:mark",         'type' : "int" },
    { 'name' : "og:space:size",         'type' : "float" },
    # TODO : { 'name' : "og:space:icon",         'type' : "string" },

    { 'name' : "og:network:position",     'type' : "vec3" },
    { 'name' : "og:network:color",        'type' : "vec4" },
    { 'name' : "og:network:locked",       'type' : "bool" },
    { 'name' : "og:network:lod",          'type' : "float" },
    { 'name' : "og:network:activity",     'type' : "float" },
    { 'name' : "og:network:mark",         'type' : "int" },
    { 'name' : "og:network:size",         'type' : "float" },
    # TODO : { 'name' : "og:network:icon",         'type' : "string" },

    { 'name' : "og:world:geolocation",  'type' : "vec2" },
    { 'name' : "og:world:color",        'type' : "vec4" },
    { 'name' : "og:world:size",         'type' : "float" }
]

edge_attributes = [
    { 'name' : "type",               'type' : 'string' },
    { 'name' : "depth",              'type' : 'float' },

    { 'name' : "sgraph:first_seen",  'type' : 'string' },
    { 'name' : "sgraph:last_seen",   'type' : 'string' },

    { 'name' : "og:space:activity", 'type' : 'float' },
    { 'name' : "og:space:color1",   'type' : "vec4" },
    { 'name' : "og:space:color2",   'type' : "vec4" },
    { 'name' : "og:space:width",    'type' : "float" },
    { 'name' : "og:space:lod",      'type' : "float" },
    # TODO : { 'name' : "og:space:icon", 'type' : "string" }

    { 'name' : "og:network:color1",   'type' : "vec4" },
    { 'name' : "og:network:color2",   'type' : "vec4" },
    { 'name' : "og:network:width",    'type' : "float" },
    # TODO { 'name' : "og:network:activity", 'type' : 'float' },
    # TODO { 'name' : "og:network:lod",      'type' : "float" },
    # TODO : { 'name' : "og:space:icon", 'type' : "string" }

    { 'name' : "og:world:color",    'type' : "vec4" }
]

def info():
    print (str(graphiti.count_nodes()) + " nodes, " + str(graphiti.count_edges()) + " edges.")

def get_attribute_info(attribute):
    t = type(attribute)

    if (t is bool) or (t is int) or (t is float):
        return (t.__name__, str(attribute))
    if t is unicode:
        return ("string", attribute.encode("utf-8"))
    elif t is list:
        l = len(attribute)
        count = sum(type(i) is float for i in attribute)
        if l != count or (l == count and (l < 2 or l > 4)):
            return None
        return ("vec" + str(l), filter(lambda x : not(x in "[,]"), str(attribute)))
    else:
        return None


def prepare_node(node):
    if 'label' not in node:
        node['label'] = node['id']
    if node['depth'] == 0:
        node['og:space:activity'] = 1.0
    
    try: node['sgraph:infected'] = node['investigate']['categorization']['status']
    except: pass
    
    try: node['sgraph:dga:score'] = node['investigate']['security']['dga_score']
    except: pass

    return node

def prepare_edge(edge):
    if 'id' not in edge:
        edge['id'] = "{0} -> {1}".format(edge['src'], edge['dst'])
    if "investigate" in edge:
        for ee in edge['investigate']:
            if ee['type'] == "co-occurrence":
                edge['og:space:activity'] = 4 * ee['score']
                break
    return edge

def load_json(json_filename):
    nodes = {}
    edges = {}
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
                graphiti.set_attribute("raindance:space:title", "string", data["meta"]["title"])

    if "attributes" in data:
        for key in data["attributes"].keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(data["attributes"][key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(data["attributes"][key]) + "'!")
                continue
            graphiti.set_attribute(key, att_info[0], att_info[1])

    print(". Loading nodes ...")
    for n in data["nodes"]:
        n = prepare_node(n)

        label = ""
        if "label" in n:
            label = n["label"].encode("utf-8")
        
        nid = graphiti.add_node(label)
        nodes[n["id"]] = nid

        for key in n.keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(n[key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(n[key]) + "'!")
                continue
            graphiti.set_node_attribute(nid, key, att_info[0], att_info[1])

    print(". Loading edges ...")
    for e in data["edges"]:
        e = prepare_edge(e)

        if "src" in e:
            eid = graphiti.add_edge(nodes[e["src"]], nodes[e["dst"]])
        else:
            eid = graphiti.add_edge(nodes[e['source']], nodes[e['target']])
        edges[e["id"]] = eid

        for key in e.keys():
            if key in reserved_attributes:
                continue
            att_info = get_attribute_info(e[key])
            if att_info is None:
                print("Error: Couldn't parse key '" + key + "' with value '" + str(n[key]) + "'!")
                continue
            graphiti.set_edge_attribute(eid, key, att_info[0], att_info[1])

    if "timeline" in data:
        print(". Loading timeline ...")
        for c in data["timeline"]:
            # TODO : Get rid of this translation phase when possible.
            if c[1].startswith("graph:"):
                if c[1] in ["graph:remove_node", "graph:set_node_attribute"]:
                    c[2]["id"] = nodes[c[2]["id"]]
                elif c[1] in ["graph:remove_edge", "graph:set_edge_attribute"]:
                    c[2]["id"] = edges[c[2]["id"]]
                elif c[1] in ["graph:add_edge"]:
                    c[2]["src"] = nodes[c[2]["src"]]
                    c[2]["dst"] = nodes[c[2]["dst"]]
            graphiti.send_command(c[0], c[1], c[2])

    print("Done.")

def save_json(filename):
    
    graph = {}
    graph["meta"] = dict()
    graph["nodes"] = list()
    graph["edges"] = list()

    global node_attributes
    global edge_attributes
    
    for id in graphiti.get_node_ids():
        node = dict()
        node["id"] = id
        node["label"] = graphiti.get_node_label(id)

        for attribute in node_attributes:
            name = attribute['name']
            value = graphiti.get_node_attribute(id, name)
            if value is None:
                continue
            node[name] = value

        graph["nodes"].append(node)

    for id in graphiti.get_edge_ids():
        edge = dict()
        edge["id"] = id
        edge["src"] = graphiti.get_edge_node1(id)
        edge["dst"] = graphiti.get_edge_node2(id)

        for attribute in edge_attributes:
            name = attribute['name']
            value = graphiti.get_edge_attribute(id, name)
            if value is None:
                continue
            edge[name] = value

        graph["edges"].append(edge)

    with open(filename, 'w') as outfile:
        json.dump(graph, outfile, indent=True, sort_keys=True)

def load_nx_graph():

    global node_attributes
    global edge_attributes
    
    graph = nx.Graph()
    
    print(graphiti.get_node_ids())

    for id in graphiti.get_node_ids():
        graph.add_node(id)
        graph.node[id]['label'] = graphiti.get_node_label(id)

        for a in node_attributes:
            attribute = graphiti.get_node_attribute(id, a['name'])
            if not(attribute is None):
                value = str(attribute)
                if a['type'] == "vec2" or a['type'] == "vec3":
                    value = filter(lambda x: not (x in "[,]"), value)
                graph.node[id][a['name']] = value

    for id in graphiti.get_edge_ids():
        node1 = graphiti.get_edge_node1(id)
        node2 = graphiti.get_edge_node2(id)
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
        for nid in graphiti.get_node_ids():
            value = None

            if attribute == "label":
                value = graphiti.get_node_label(nid)
            elif attribute == "mark":
                value = graphiti.get_node_mark(nid)
            elif attribute == "weight":
                value = graphiti.get_node_weight(nid)
            else:
                value = graphiti.get_node_attribute(nid, attribute)

            if value is None:
                f("node", nid, None)
            else:
                f("node", nid, r.match(str(value)))

    if edge_flag:
        for eid in graphiti.get_edge_ids():
            value = None

            if attribute == "node1":
                value = graphiti.get_edge_node1(eid)
            elif attribute == "node2":
                value = graphiti.get_edge_node2(eid)
            else:
                value = graphiti.get_edge_attribute(eid, attribute)

            if value is None:
                f("edge", eid, None)
            else:
                value_str = str(value)
                f("edge", eid, r.match(value_str))

