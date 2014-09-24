/*
 * NOTE : In order to run this script when OpenGraphiti starts, you need to add some code at the end of your graphiti.html.
 * Add :
 * <script async src='path/to/this/file.js'></script>
 */

var loadJSON = function(file, jdata)
{
	var nodes = {};
	var edges = {};

	var node_attributes =
	{
    	"og:space:position" : "vec3",
    	"og:space:color" : "vec4",
   		"og:space:locked" : "bool",
    	"og:space:lod": "float",
    	"og:space:activity" : "float",
    	"og:space:mark" : "int",
		"og:space:size" : "float"
	};

	var edge_attributes =
	{
	    "og:space:activity" : "float",
    	"og:space:color1" : "vec4",
    	"og:space:color2" : "vec4",
    	"og:space:width" : "float",
    	"og:space:lod" : "float"
    };

    Module.setAttribute("og:space:title", "string", file.name);

    if (jdata[""])

	for (id in jdata["nodes"])
	{
		var node = jdata["nodes"][id];
		var nid = Module.addNode(node["label"]);

		for (attribute in node)		
			if (attribute in node_attributes)
			{
				var name = attribute;
				var type = node_attributes[name];
				var value = type.indexOf("vec") == 0 ? node[name].join(" ") : node[attribute].toString();
				// console.log(name, type, value);
				Module.setNodeAttribute(nid, attribute, type, value);
			}
		nodes[node["id"]] = nid;
	}

	for (id in jdata["edges"])
	{
		var edge = jdata["edges"][id]
		var src = nodes[edge["src"]];
		var dst = nodes[edge["dst"]];
		var eid = Module.addEdge(src, dst);

		for (attribute in edge)		
			if (attribute in edge_attributes)
			{
				var name = attribute;
				var type = edge_attributes[name];
				var value = type.indexOf("vec") == 0 ? edge[name].join(" ") : edge[attribute].toString();
				// console.log(name, type, value);
				Module.setEdgeAttribute(eid, attribute, type, value);
			}

		edges[edge["id"]] = eid;
	}
}

var hideEmscriptenUI = function()
{
	for (i = 0; i < 5; i++)
	{
		document.body.children[i].style.display = 'None';
	}

	document.getElementById('output').style.display ='None';
	document.getElementById('canvas').style.display ='block';	
}

var setupDragAndDrop = function()
{
	var holder = document.getElementById('canvas');
	var state = document.getElementById('status');

	if (typeof window.FileReader === 'undefined')
	{
		state.className = 'fail';
	}
	else
	{
	  state.className = 'success';
	  state.innerHTML = 'File API & FileReader available';
	}
	 
	holder.ondragover = function ()
	{
		holder.style.opacity = 0.5;
		return false;
	};
	holder.ondragend = function ()
	{
		holder.style.opacity = 1.0;
		console.log("dragend");
		return false;
	};
	holder.ondrop = function (e)
	{
		holder.style.opacity = 1.0;
		holder.style.border = "none";
	  	e.preventDefault();

	  	var file = e.dataTransfer.files[0];
	  	console.log(file);
	  	var reader = new FileReader();
	  	reader.onload = function (event)
	  	{
	    	console.log(event.target);

		  	var jdata = JSON.parse(event.target.result);
	  		loadJSON(file, jdata);
	  	};

	  	reader.readAsText(file);

	  	return false;
	};
}

var main = function()
{
	hideEmscriptenUI();

	setupDragAndDrop();

	Module.create();
	Module.initialize();
	Module.createWindow('OpenGraphiti : Data Visualization Engine', window.innerWidth - 10, window.innerHeight - 10);

	
	Module.createEntity('graph');
	Module.createView('space');
	Module.registerScript('Start Animation', 'Module.setAttribute("og:space:animation", "bool", "true");'); 
	Module.registerScript('Stop Animation', 'Module.setAttribute("og:space:animation", "bool", "false");'); 

	Module.start();
	Module.destroy();
}

Module.postRun = [main];