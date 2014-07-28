## UI
* Mouse Icon
	* Change to "Select" mode. Clicking on nodes selects them.
* Diagonal bar
	Change to "Markation" mode. Clicking on nodes marks them with a Checkmark icon.

### Nodes
* Three-colored dots Icon
	* Clicking here changes which nodes are visible. By default, it is set to "All."
		* All - All nodes are visible.
		* Colors + Marks - Only node colors and marks are visible.
		* Colors + Labels
		* Colors
		* Marks + Labels
		* Marks
		* Labels
		* Off - Nodes are invisible
* Size slider
	* Changes the size of the nodes
* Activity Checkbox
	* Display activity, for nodes which are set with activity attributes

### Edges
* Icon
	* Changes the textures and visisbility of edges. Default is set to
	"Lines,"
	* Lines
		* Lines - displays all edges as plain lines.
		* Wide lines - displays wide lines. This is necessary to see edges which
		are loaded with textures.
* Size slider
	* Changes the width of the edges
* Activity Checkbox
	* Display activity, for edges which are set with activity attributes

### Physics
* Temperature slider
	* Increases the temperature, or "speed" at which the nodes are repelled and
	attracted to each other.
* Play button
	* Starts the physics engine. Nodes which are connected are attracted to one
	another, while nodes which are not connected are repelled from one another.

### Filters
* Node LOD checkbox
	* Display the LOD attributes of nodes.
* Edge LOD checkbox
	* Display the LOD attributes of edges.

## Menu
### Main
* Clear graph
	* Removes the loaded graph
* Clear Colors
	* Clears all node and edge colors
* Clear LOD
	* LOD stands for "Level of Detail." It is a number between 0 and 1 which determines
	the visibility of nodes and edges based on certain traits. Essentially, clearing the
	LOD will turn off the visual filtering applied by operations like searching for
	attributes, edge activity, etc.
* Node Type
	* Color the nodes by their "type" attribute
* Edge Type
	* Color the edges by their "type" attribute
* Country Icons
	* Show country flag icons on nodes which have a country code attribute
* Color attribute
	* Color nodes by an attribute which is typed into the console
* Depth to LOD
	* If nodes/edges have a depth attr, it maps depth on LOD value, and rescales between
	0 and 1. Basically, this means it will apply the visual LOD effects on a gradiant
	scale, which is derived from the depth attribute.
* Search by Node Attribute
	* Search the graph for certain attributes. Will invoke a prompt at the console
	which first asks for the string you wish to search for, and then the attribute
	to do the search on.

	e.g.
	```
	Expression : Google
	Attribute : label
	```
	will search for any nodes with the label "Google" and color them all the
	same color.
* Search by Edge Attribute
	* Same as above, but for edges.

### Colors
* Dim Nodes
* Dim Nodes
* Brighten Edges
* Brighten Edges
* Dim Nodes Alpha
* Dim Nodes Alpha
* Brighten Edges Alpha
* Brighten Edges Alpha

### Arrangement
* Lock / Unlock
	* Locks position. I.e., the physics engine does not move it.
* Point Layout
	* Condenses all nodes into a point in the center.
* Sphere Layout
	* Lays out the nodes as a sphere.
* Cube Layout
* Conic Layout
* Depth Circle Layout
	* Lays out the graph so that nodes with a "depth" attribute of 0 will be
	placed on the edge of a circle and become locked in place.
* Globe Layout
	* If there's geo-location data on a node, puts it on a Globe.

### Topology
* Neighbors
	* Colors the neighbors ([adjacent nodes](http://en.wikipedia.org/wiki/Glossary_of_graph_theory#Adjacency_and_degree)) of the selected node.
* Connected components
	* Colors the [connected components](http://en.wikipedia.org/wiki/Glossary_of_graph_theory#Connectivity) of the graph.
* High Degrees
	* Dims lower-degree nodes to increase visibility of high-degree nodes.
* Low Degrees
	* Dims higher-degree nodes to increase visibility of lower-degree nodes.
* Node connections
	* Colors nodes by their degree
* Detect SPN
	* Finds and colors [source nodes](http://en.wikipedia.org/wiki/Glossary_of_graph_theory#Weighted_graphs_and_networks) which have common children.

### Animation
* Start
	* Starts a camera rotation around the selected node.
* Stop
	* Stops the animation.

### Test / Debug
Lots of randomization of the graph, mostly for playing around.

* Randomize Node Activity
	* Randomizes the activity attribute of nodes.
* Randomize Node Icons
* Randomize Node Size
* Randomize Edge Activity
* Randomize Edge Icons
* Randomize Edge Width
* Randomize Timeline

