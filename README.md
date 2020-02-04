Cells 0.1 - README  2020-02-03
==============================
Work in progress!
This is Cells, a new way to ANNs (artifical neural networks).
Each cell can have multiple nodes (ANNs) which can be linked together.

In the cells-demo.c demo a XOR and an OR are connected by an AND logic.
For this the XOR and OR are on a layer 0 and the outputs are copied over to the linked AND in layer 1.

You can define a number of cells and put some nodes in it which all can contain ANNs which can be linked together by the "set_node_link" function. See the example cells-demo.c for more!

This is a work in progress.

The libfann ANN library is required to build the library!

INSTALLATION
============
Run the "make-cells.sh" bash script in the lib/ directory first.
Then run the "make.sh" script to build the demo cells-demo.c.
That's it for now!
