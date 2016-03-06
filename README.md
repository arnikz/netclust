netclust/netindex
=================
are fast and memory-efficient C programs developed to detect clusters of nodes in very large graphs (networks) with billions of edges using a scalable nearest-neighbor approach with (nearly) linear time and space complexity *~O(E)*, *E* number of edges ([Kuzniar *et al.*, 2010](http://www.ncbi.nlm.nih.gov/pubmed/20679333)). While *netindex* pre-processes the input graph into an efficient binary format, *netclust* performs the actual cluster detection in the graph by taking a single user-defined cutoff value (similarity or distance).


Compile and run
---------------

```
cd src
make        # default make 'all' binaries or separately 'netindex' | 'netclust'
make clean  # remove binaries
```

```
netindex ../examples/graph.tsv          # index the input (weighted) graph
netclust ../examples/graph.tsv O1 S 500 # detect clusters in the graph
```
