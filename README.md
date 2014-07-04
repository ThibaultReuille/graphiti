OpenGraphiti
============

 OpenGraphiti is an open source 2D and 3D data visualization engine for data scientists to visualize semantic networks and to work with them. It offers an easy-to-use API with several associated libraries to create custom-made datasets. It leverages the power of GPUs to process and explore the data and sits on a homemade 3D engine.

Like any good visualization tool, data is required. In addition, the data set must be formatted in such a way that OpenGraphiti can apply the algorithms to affect the spatial representation and interconnectivity of the data nodes and edges.

0 - Prerequisites
----------------
- OS X 10.9.x (Mavericks)
- <a href="http://python.org">Python 2.7.x</a>
- <a href="https://networkx.github.io/">NetworkX 1.8.1</a>
- <a href="http://git-scm.com/">git</a>
- <a href="https://developer.apple.com/xcode/">Xcode</a> Command Line Tools

Optional (if the web version is required)

- <a href="https://github.com/kripken/emscripten/wiki">Emscripten LLVM to JavaScript compiler</a>
- <a href="https://github.com/opendns/semanticnet">semanticnet</a> - Used to create semantic graphs in JSON

###0.1 - Preparing Your Directory
It is recommended that a separate directory be created to house the visualization files. For the purposes of this document we will use "Visualization" as the directory name.

    $ mkdir Visualization

###0.2 - Installing NetworkX
To install the NetworkX library, use <code>pip</code>.

    $ pip install networkx
*Note: If you are unable to find the correct version, a tarball is included in the <code>graphiti/lib/</code> folder.*
###0.3 (*optional*) - Installing Emscripten
To install Emscripten it is recommended that you use the Portable Emscripten SDK. It is identical to the NSIS installer, except that it does not interact with the Windows registry, which allows Emscripten to be used on a computer without administrative privileges, and gives the ability to migrate the installation from one location (directory or computer) to another by just copying/zipping up the directory contents.

If you want to use the Portable Emscripten SDK, the initial setup process is as follows:

1. Download and unzip the portable SDK package to a directory of your choice. This directory will contain the Emscripten SDK.
2. Open a command prompt to the directory of the SDK.
3. Run <code>./emsdk update</code>. This will fetch the latest registry of available tools.
4. Run <code>./emsdk install latest</code>. This will download and install the latest SDK tools.
5. Run <code>./emsdk activate latest</code>. This will set up ~/.emscripten to point to the SDK.

Whenever you change the location of the Portable SDK (e.g. take it to another computer), re-run step 5.
###0.4 (*optional*) - Downloading <code>semanticnet</code>
The <code>semanticnet</code> python library can be used to create semantic graphs in JSON.  Created datasets can then be visualized with OpenGraphiti. To download the library:

	$ git clone https://github.com/opendns/semanticnet/semanticnet.git
*Note: For more information on how to utilize the <code>semanticnet</code> library, please visit <a href="https://github.com/opendns/semanticnet">https://github.com/opendns/semanticnet</a>. For sample python scripts leveraging the <code>semanticnet</code> library, please visit <a href="https://github.com/opendns/semanticnet/examples/">https://github.com/opendns/semanticnet/examples</a>.*

You are now ready to download the required files for OpenGraphiti.

1. Obtaining Installation Files
-------------------
Change directories into the Visualization folder.

    $ cd Visualization

The first repository to download is Raindance, the 3D Engine written in C++11 (LLVM compatible) using OpenGL ES, GLUT, GLM and OpenCL.

    $ git clone https://github.com/opendns/raindance.git   -----MAYBE CHANGE LATER-----

After Raindance has successfully downloaded to your local machine, download the OpenGraphiti repository.

    $ git clone https://github.com/opendns/graphiti.git   -----MAYBE CHANGE LATER-----

With the required files downloaded, we can continue with the installation.

3 - Installation
------------

    $ cd Visualization/graphiti
    $ make clean native

After installation, the **graphiti** executable will be available as <code>Visualization/graphiti/graphiti</code>.

To install the web version of OpenGraphiti there is one additional step required. Issue the <code>make web</code> command from within the <code>Visualization/graphiti/</code> directory.

	$ cd Visualization/graphiti
	$ make web

Upon successful installation, we can now visualize our data sets.

4 - Running
----------
Launch OpenGraphiti from the command line to display the available views.

    $ cd Visualization/graphiti/
    $ ./graphiti

*Note: At the time of this writing, the only available view was the <code>demo</code> view.*

In addition to an available **view** a valid semantic JSON graph data set must be specified.

    ./graphiti <view> <data set>
e.g.

	$ ./graphiti demo ../semanticnet/examples/fs.json


5 - Sample Data Sets
-----------

We have provided several and python scripts that leverage the <code>semanticnet</code> library. These can be found in the <code>./semanticnet/examples/</code> directory or on the Git repository at <a href="https://github.com/opendns/semanticnet/examples/">https://github.com/opendns/semanticnet/examples/</a>.

For more information on how to utilize the <code>semanticnet</code> library, please visit <a href="https://github.com/opendns/semanticnet">https://github.com/opendns/semanticnet</a>. 

6 - Contributors
----------
* Thibault Reuille, OpenDNS - <a href="https://twitter.com/ThibaultReuille">@ThibaultReuille</a>
* Andrew Hay, OpenDNS - <a href="https://twitter.com/andrewsmhay">@andrewsmhay</a>
* Skyler Hawthorne, OpenDNS - <a href="https://twitter.com/dead10ck">@dead10ck</a>

7 - Contact Information
----------
For any questions, problems, or feature requests in reference to OpenGraphiti, semanticnet, or Raindance, please use the  **issues** link associated with the repository in question. We will attempt to address all issues as quickly as possible.

8 - Links
----------
* <a href="https://www.blackhat.com/us-14/briefings.html#unveiling-the-open-source-visualization-engine-for-busy-hackers">Unveiling The Open Source Visualization Engine For Busy Hackers</a> by Thibault Reuille and Andrew Hay for <a href="https://www.blackhat.com/us-14">Black Hat 2014</a>
* White Paper: Open Source Visualization with OpenGraphiti, by Thibault Reuille and Andrew Hay for <a href="https://www.blackhat.com/us-14">Black Hat 2014</a>
* Blog Post: <a href="http://labs.opendns.com/2014/02/06/data-exploration-virtual-tour-security-graph/">Data Exploration, A Virtual Tour Of The Security Graph</a>, by Thibault Reuille

9 - About OpenDNS Security Labs
----------

At OpenDNS Security Labs we thrive on continual innovation. But we don’t simply look at data from new threats and ask ourselves, “If we had known that before, what would we have done differently?” We look at our extensive data collection network built on top of the largest security infrastructure and predict what’s coming next.  We’re a team of world-class engineers, mathematicians, and security researchers, and we’re taking an innovative and proactive approach to security research. More than any other area of technology, we see history repeat itself most often in information security. The way we have traditionally defended against malware has become a cycle: new technologies are created and adopted, attackers leverage the opportunity to expand the attack surface and security solutions are released in reaction to the latest threats. But this method is tired, and we can do better. Our goals are simple: Continually innovate ahead of the pace of technology change and build the best security protection and security delivery network platform possible without compromising performance or productivity.

10 - License
----------
Copyright (c) 2014, OpenDNS, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.