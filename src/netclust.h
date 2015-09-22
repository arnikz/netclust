/*
 * File:	netclust.h
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

#ifndef NETCLUST_H
#define	NETCLUST_H
#define BUF 255

/* typedefs */
typedef struct _ncls_io_set_ NCLS_IO_SET;	/* IO settings */
typedef struct _nidx_table_ NIDX_TABLE;	/* lookup table of node aliases */
typedef struct _cluster_ CLUSTER;	/* cluster members */
typedef struct _partition_ PARTITION;	/* graph partition i.e union of clusters */

/* type declarations */
struct _ncls_io_set_ {
    unsigned int output_mode;	/* output mode for clusters */
    unsigned int weight_type;	/* weight type for edges */
    unsigned int n_nodes;	/* number of nodes */
    unsigned int n_edges;	/* number of edges */
    unsigned int n_clusters;	/* number of clusters */
    float weight_cutoff;	/* cutoff value for edge weights */
    char infile[BUF];		/* input file of graph edges */
    char infile_nidx[BUF];	/* input file of node indices */
    char infile_eidx[BUF];	/* input file of indexed edges */
    char outfile_clst[BUF];	/* output file of clusters */
    char outfile_log[BUF];	/* output log file */
};

struct _nidx_table_ {
    unsigned int table_size;	/* size of the lookup table */
    unsigned char **nodes;	/* an array of node aliases */
};

struct _cluster_ {
    unsigned int id;		/* cluster ID */
    unsigned int size;		/* cluster size */
    unsigned char **member;	/* array of cluster members */
};

struct _partition_ {
    unsigned int size;		/* number of clusters */
    CLUSTER *cluster;		/* array of clusters */
};

/* function prototypes */
extern void NclsUsage();
extern NCLS_IO_SET NclsStoreIOset(int, char **);
extern unsigned int StoreNodeIdx(NIDX_TABLE *, NCLS_IO_SET *);
extern int CmpBySize(const void *, const void *);
extern PARTITION *GetGraphPartition(NIDX_TABLE *, NCLS_IO_SET *);
extern unsigned int FindRoot(unsigned int *, unsigned int);
extern unsigned int OutputGraphPartition(PARTITION *, NCLS_IO_SET *);
extern void NclsWriteLogFile(NCLS_IO_SET *);

#endif				/* NETCLUST_H */
