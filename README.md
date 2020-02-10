Cells 0.1 - README  2020-02-10
==============================
Work in progress!
This is Cells, a new way to ANNs (artifical neural networks).
Each cell can have multiple nodes (ANNs) which can be linked together.

In the cells-demo.c demo a XOR and an OR are connected by an AND logic.
For this the XOR and OR are on a layer 0 and the outputs are copied over to the linked AND in layer 1.

You can define a number of Cells and put some nodes in it which all can contain ANNs which can be linked together by the "set_node_link" function. See the example cells-demo.c for more!

This is a work in progress.

The libfann ANN library is required to build the library!

Cells load/save
---------------
With the "fann_save_cells" function all Cells can be saved as a plain text file.
And with the "fann_load_cells" function the Cells can be load into a new allocated
Cells structure. The Cells are saved with the FANN ANN names and with all links.

INSTALLATION
------------
Run the "make-cells.sh" bash script in the lib/ directory first.
Then run the "make.sh" script to build the demo cells-demo.c.
That's it for now!
