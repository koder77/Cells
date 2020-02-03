Cells 0.1 - README  2020-02-03
==============================
Work in progress!
This is Cells, a new way to ANNs (artifical neural networks).
Each cell can have multiple nodes (ANNs) which can be linked together.

In the main.c demo an XOR and an OR are connected by an AND logic.
For this the XOR and OR are on a layer 0 and the outputs are copied over to the linked AND in layer 1.

You can define a number of cells and put some nodes in it which all can contain ANNs which can be linked together by the "set_node_link" function. See the example main.c for more!

This is a work in progress. And later I will build this as a library.
