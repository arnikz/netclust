#ifndef NETINDEX_H
#define	NETINDEX_H
#define BUF 255 /* string buffer */
#define NODE_IDX 0 /* node indexing starts from zero */
#define HASH_TABLE_SZ 5000000	/* hash table size */
#define FWRITE(fp, fn, s, t) if(!fwrite(s, t, 1, fp)) OOPS(fn);

/* typedefs */
typedef struct _nidx_io_set NIDX_IO_SET;
typedef struct _linked_list_ LINKED_LIST;
typedef struct _hash_table_ HASH_TABLE;

struct _nidx_io_set {
	unsigned int weight_type;
	unsigned int n_nodes;
	unsigned int n_edges;
	unsigned int n_edges_passed;
	float weight_cutoff;
	char  infile[BUF];
	char  outfile_nidx[BUF];
	char  outfile_eidx[BUF];
	char  outfile_sube[BUF];
	char  outfile_log[BUF];
};

struct _linked_list_ {
	char         *key;
	unsigned int value;
	LINKED_LIST  *next;
};

struct _hash_table_ {
	unsigned int size;
	LINKED_LIST  **table;
};

/* function prototypes */
extern void	NidxUsage();
extern NIDX_IO_SET NidxStoreIOset(int, char **);
extern void	InitFileNidxHeader(FILE *, char *);
extern void	InitFileEidxHeader(FILE *, char *);
extern void	UpdateFileNidxHeader(FILE *, char *, unsigned int);
extern void	UpdateFileEidxHeader(FILE *, char *, unsigned int);
extern void WriteFileNidx(FILE *, char *, EDGE *, HASH_TABLE *, unsigned int *);
extern HASH_TABLE *NewHashTable(unsigned int);
extern unsigned int *GetHashValue(HASH_TABLE *, char *);
extern char *AddNewKVPToHash(HASH_TABLE *, char *, unsigned int);
extern void	IndexGraph(NIDX_IO_SET *);
extern void	NidxWriteLogFile(NIDX_IO_SET *);

#endif /* NETINDEX_H */
