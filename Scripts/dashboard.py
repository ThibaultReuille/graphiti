import sys
import time
import threading
import random
import math
import pygeoip

from Scripts import graphiti as og
from Scripts import standard as std
from Scripts import console
        
shell = console.console.Console()

mutex = threading.Lock()
queue = list()
nodes = dict()
edges = dict()
job_id = None

t = 0
value = 50

gi = pygeoip.GeoIP('./Lib/GeoLiteCity.dat')

def on_idle():

    global queue
    global nodes
    global edges
    global t
    global job_id

    mutex.acquire()

    for element in queue:
        if og.count_nodes() > 1000:
            #og.register_script("#console", 'dashboard.shell.execute')
            og.remove_job(job_id)
            continue
        #print(element)

        # --- Space Document ---
        name = element['name']
        rr = element['rr']
        asn = element['geoip']['rr_asn']

        '''
        if asn is not None:
            asn = asn.split()[0][2:]
            if asn in nodes:

                t = time.time()
                r = 0.5 + math.cos(t)
                g = 0.5 + math.sin(t)
                b = 0.5 + math.sin(t + math.pi / 2)
                a = 0.5
                og.set_node_attribute(nodes[asn], 'og:space:color', 'vec4', "{0} {1} {2} {3}".format(r, g, b, a))
        '''
        
        if name not in nodes:
            nodes[name] = og.add_node(name)
            og.set_node_attribute(nodes[name], 'type', 'string', 'name')

        if rr not in nodes:
            nodes[rr] = og.add_node(rr)
            og.set_node_attribute(nodes[rr], 'type', 'string', 'ip')
        
        edge_key = "{0} -> {1}".format(name, rr)
        if edge_key not in edges:
            edges[edge_key] = og.add_edge(nodes[name], nodes[rr])
        

        # --- WorldMap + Globe Documents ---
        
        try:
            record = gi.record_by_addr(rr)
        except:
            continue
        if record is None:
            continue
        msg = {
            'function' : 'add',
            'latitude' : record['latitude'],
            'longitude' :  record['longitude'],
            'color.r' : random.uniform(0.3, 1.0),
            'color.g' : 0.0,
            'color.b' : 0.0,
            'color.a' : 0.75,
            'size' : 2.0
        }
        og.request(1, msg)
        og.request(2, msg)
        

    queue = list()
    
    mutex.release()

def on_tick():
    global t
    global value

    og.request(3, {
        "function" : "push",
        "id" : 0,
        "time" : t,
        "value" : value
    })
    
    value += 2 * random.uniform(-1.0, 1.0)
    t += 1.0

    if t > 300:
        og.request(3, { "function" : "clear", "id" : 0 })
        t = 0
        value = 50

def thread_zmq():

    import zmq
    import json

    ctx = zmq.Context()
    s = ctx.socket(zmq.SUB)
    s.connect('tcp://avalanche.r1.usw1.opendns.com:10101') # nlp-rank results
    s.setsockopt(zmq.SUBSCRIBE, '')

    while True:
        line = s.recv()
        jdata = json.loads(line)

        if random.uniform(0, 1.0) > 0.90:
            continue

        mutex.acquire()
        try:
            queue.append(jdata)
        except:
            pass
        mutex.release()

def on_started():
    #og.register_script("#console", 'dashboard.shell.execute')

    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            std.load_json(sys.argv[2])
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

    global nodes
    for nid in og.get_node_ids():
        label = og.get_node_label(nid)
        nodes[label] = nid

    response = og.request(3, { "function" : "add" })
    global timeseries
    timeseries = response['id']


def start():

    # ----- Initialization -----

    og.create_window("OpenGraphiti : Data Visualization Engine", 1024, 728)
    og.create_entity("graph")
    og.create_visualizer("space")
    #og.create_visualizer("network")
    og.create_visualizer("globe")
    og.create_visualizer("world")
    # TODO: og.create_visualizer("timeline") 
    og.create_visualizer("timeseries")
    og.create_visualizer("heightmap")
    og.create_visualizer("shell")
    og.create_visualizer("logo")

    # ----- Console Callbacks ------

    og.register_script('#idle', 'dashboard.on_idle()')
    global job_id
    job_id = og.add_job("#idle", 1.0)

    og.register_script('#tick', 'dashboard.on_tick()')
    og.add_job("#tick", 0.5)

    t = threading.Thread(target=thread_zmq, args=())
    t.daemon = True
    t.start()

    og.register_script('#started', 'dashboard.on_started()') 

    # -----

    og.start()
