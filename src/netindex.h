/* 
 * File:	netindex.h
 * Author:	Arnold Kuzniar
 * Date:	29-04-2008
 * Version:	1.0
 *
 ***************************************************************************
 * Copyright 2008 Arnold Kuzniar
 *
 * This file is part of netclust/netindex.
 *
 * netclust/netindex is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * netclust/netindex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with netclust/netindex.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETINDEX_H
#define	NETINDEX_H

#define BUF 255			/* string buffer */
#define NODE_IDX 0		/* node indexing starts from zero */
#define HASH_TABLE_SZ 5000000	/* hash table size */
#define FWRITE(fp, fn, s, t) if(!fwrite(s, t, 1, fp)) OOPS(fn);	/* write binary file */

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
    char infile[BUF];
    char outfile_nidx[BUF];
    char outfile_eidx[BUF];
    char outfile_sube[BUF];
    char outfile_log[BUF];
};

struct _linked_list_ {
    char *key;
    unsigned int value;
    LINKED_LIST *next;
};

struct _hash_table_ {
    unsigned int size;
    LINKED_LIST **table;
};

/* function prototypes */
extern void NidxUsage();
extern NIDX_IO_SET NidxStoreIOset(int, char **);
extern void WriteFileNidxHeader(FILE *, char *, unsigned int);
extern void WriteFileNidx(FILE *, char *, EDGE *, HASH_TABLE *,
			  unsigned int *);
extern void UpdateFileNidxHeader(FILE *, char *, unsigned int *);
extern void WriteFileEidxHeader(FILE *, char *);
extern void UpdateFileEidxHeader(FILE *, char *, unsigned int);
extern HASH_TABLE *NewHashTable(unsigned int);
extern unsigned int *GetHashValue(HASH_TABLE *, char *);
extern char *AddNewKVPToHash(HASH_TABLE *, char *, unsigned int);
extern void IndexGraph(NIDX_IO_SET *);
extern void NidxWriteLogFile(NIDX_IO_SET *);

#endif				/* NETINDEX_H */
