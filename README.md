netclust
========

*netclust/netindex* are fast and memory-efficient tools developed to detect clusters of nodes in very large graphs (networks) using a scalable nearest-neighbor approach with (nearly) linear time and space complexity (*~O(N)). While *netindex* pre-processes the input into an efficient binary format, *netclust* performs the actual cluster detection using a single (dis)similarity-based cutoff.