/*
 * NOTE : In order to run this script when OpenGraphiti starts, you need to add some code at the end of your graphiti.html.
 * Add :
 * <script async src='path/to/this/file.js'></script>
 */

var example = function()
{
	console.log("Running example script ...");

	n1 = Module.addNode("A");
	n2 = Module.addNode("B");
	n3 = Module.addNode("C");

	e1 = Module.addEdge(n1, n2); // A -> B
	e2 = Module.addEdge(n2, n3); // B -> C
	e3 = Module.addEdge(n3, n1); // C -> A

	Module.setNodeAttribute(n1, "og:space:color", "vec4", "1.0 0.0 0.0 1.0"); // RED
	Module.setNodeAttribute(n2, "og:space:color", "vec4", "0.0 1.0 0.0 1.0"); // GREEN
	Module.setNodeAttribute(n3, "og:space:color", "vec4", "0.0 0.0 1.0 1.0"); // BLUE

	Module.setEdgeAttribute(e1, "og:space:activity", "float", "1.0");
	Module.setEdgeAttribute(e2, "og:space:activity", "float", "2.0");
	Module.setEdgeAttribute(e3, "og:space:activity", "float", "3.0");	
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

var main = function()
{
	hideEmscriptenUI();

	Module.create();
	Module.initialize();
	Module.createWindow('OpenGraphiti : Data Visualization Engine', window.innerWidth - 10, window.innerHeight - 10);
	Module.createEntity('graph');
	Module.createView('space');

	Module.registerScript('#started', 'alert("Welcome in OpenGraphiti !");');
	Module.registerScript('Example', 'example();') 

	Module.start();
	Module.destroy();
}

Module.postRun = [main];