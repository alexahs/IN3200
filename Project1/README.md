# PageRank algorithm

This program runs the PageRank algorithm, with a web graph as its data source. It is implemented with OpenMP parallelism,
which must be installed to run the program. Running the program will produce a results table in the terminal window,
which lists the top scoring pages and their scores. The page graph file must be formatted in the following way:

~~~
# Directed graph (each unordered pair of nodes is saved once): 8-webpages.txt
# Just an example
# Nodes: 8 Edges: 17
# FromNodeId ToNodeId
0 1
0 2
1 3
2 4
2 1
3 4
..
~~~

Compiling the program is done by running the Makefile, which automatically links the .c and .h files and
creates an executable 'PageRank.exe'.

The executable requires a number of command line arguments to be specified at runtime:
- Filename of the web graph
- Number of pages to be listed in the results
- The damping constant d used in the matrix multiplication
- The tolerance for when the score vector has converged
- (Optional) specification of number of threads to be run in parallel


EXAMPLE
~~~
~$ ./pageRank.exe web-NotreDame.txt 10 0.85 1e-9 5
~~~
Typing this command after compiling will run the program and produce the following results:

~~~
Loading web graph web-NotreDame.txt...
-----------------Graph info----------------
Number of nodes:                325729
Number of outbound links:       1469679
Number of self links:           27455
Number of dangling pages:       188795
-------------------------------------------
Iterating page ranks with tolerance=1e-09 and damping=0.85
Iterations: 75
-------------------Results-----------------
  Rank:   | Node Number: |   Score:
    1     |      1963    |  0.00562677
    2     |         0    |  0.00540451
    3     |    124802    |  0.00332623
    4     |     12129    |  0.00285706
    5     |    191267    |  0.00274873
    6     |     32830    |  0.00273183
    7     |      3451    |  0.00258998
    8     |     83606    |  0.00245969
    9     |      1973    |  0.0023759
   10     |    142732    |  0.00234064
----------------Timings [sec]--------------
Reading web graph:    0.435537
Page rank iterations: 0.572468
Sorting top pages:    0.011383
Total:                1.019416
-------------------------------------------
~~~
