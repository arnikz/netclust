#ifndef NET_H
#define	NET_H
#define VER "1.1.0"
#define _FILE_OFFSET_BITS 64 /* large file support (LFS) */
#define LINE_BUF 100 /* line buffer */
#define NODE_BUF 30 /* node alias buffer */
#define OOPS(s) { perror(s); exit(EXIT_FAILURE); } /* exit with error message */
#define MALLOC(s, t) if (((s) = malloc(t)) == NULL) OOPS("Error: malloc()\n");
#define FOPEN(fp, fn, fm) if ((fp = fopen(fn, fm)) == NULL) OOPS(fn);
#define FCLOSE(fp, fn) if (fclose(fp)) OOPS(fn);

/* typedefs */
typedef struct _idx_edge_ IDX_EDGE; /* indexed graph edge */
typedef struct _edge_ EDGE; /* graph edge */

/* type declarations */
struct _edge_ {
	char  nodeA[NODE_BUF];
	char  nodeB[NODE_BUF];
	float weight;
};

struct _idx_edge_ {
	unsigned int nodeA; /* index node A */
	unsigned int nodeB; /* index node B */
	float        weight; /* edge weight */
};

#endif /* NET_H */
