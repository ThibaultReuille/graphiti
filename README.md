OpenGraphiti
============

This is the public repository referencing the information to install, run and work with the data visualization platform.

0. Prerequisites
----------------

Create a Visualization (or other name) to store the whole dataviz platform.

    $ mkdir Visualization

1. Cloning projects
-------------------

Enter the data visualization folder

    $ cd Visualization

First you will need to clone the 3D engine sitting under the data visualiation engine :

    $ git clone git@github.com:ThibaultReuille/raindance.git

Then OpenGraphiti :

    $ git clone https://github.com/ThibaultReuille/graphiti.git

Also you will need to install NetworkX (Only tested with version 1.8.1)

    $ pip install networkx

If you are unable to find the correct version, a tarball is included in the graphiti/Lib/ folder.

3. Compiling
------------

    $ cd Visualization/graphiti
    $ make clean native

If you don't have 'make' installed start Xcode, go to Xcode->Preferences->Downloads and install component named "Command Line Tools".
After that all the relevant tools will be placed in /usr/bin folder and you will be able to use it.

You will also notice that you can compile the web version with "make web" but you will have to install the emscripten llvm compiler.

4. Running
----------

    $ ./graphiti

You will see a list of various views you may choose. To run the desired view :

    $ ./graphiti <view>

5. Datasets
-----------

If you wish to create your own graph datasets, you will need to clone the SemanticNet library.

    $ cd ../
    $ git clone git@github.com:ThibaultReuille/semantic-net.git

Take at look at the semantic-net examples for a quick description on how to use it.

6. Further
----------

To get you started with the python scripts, you want to look into graphiti/Scripts/start.py, this is the first script launched by the engine.