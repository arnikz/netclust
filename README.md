netclust/netindex
=================
are fast and memory-efficient tools developed to detect clusters of nodes in very large graphs (networks) with billions of edges using a scalable nearest-neighbor approach with (nearly) linear time and space complexity *~O(E)* (*E* - number of edges). While *netindex* pre-processes the input graph into an efficient binary format, *netclust* performs the actual cluster detection in the graph by taking a single user-defined cutoff value (similarity or distance).
