/*
 * File:	netindex.c
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
 ****************************************************************************
 *
 * netindex is a graph (network) indexing program used together with the netclust program.
 * An input graph must be stored in the form of edge list (one edge per line). Please, see
 * the input format below:
 *
 * [nodeA]	[nodeB]	[weight]
 * 
 *   - columns separated by space " " or tab "\t"
 *		- space(s) are not allowed within the columns
 *		- both 'nodeA' and 'nodeB' are read as strings (max. size specified by NODE_BUF variable)
 *		- 'weight' can be integer or float
 * 
 * The program generates 2 or 3 output files from an input FILE with the following suffixes:
 * 
 * FILE.A_B_C
 *
 *    A - indicates the weight type (1 character)
 *        'S' for similarity cutoff
 *        'D' for distance cutoff
 *
 *    B - indicates the weight threshold (floating point number)
 *
 *    C - indicates a file type (4 characters)
 *        'nidx' - binary file of node indices
 *        'eidx' - binary file of indexed edges
 *        'sube' - text file with a subset of input edges
 */

#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netindex.h"

/* print program usage */
void NidxUsage()
{
    printf
	("**************************************************\n");
    printf
	(" netindex v1.0 - graph (network) indexing program\n");
    printf
	("**************************************************\n");
    printf
        (" Author:\n\tArnold Kuzniar\n");
    printf
        (" Last update:\n\tApr 2008\n");
    printf
	(" Description:\n\tThe program indexes an input graph for the 'netclust' program.\n");
    printf
	(" Usage:\n");
    printf("       netindex [FILE] [WEIGHT TYPE] [WEIGHT CUTOFF]\n\n");
    printf
	("       FILE (mandatory)         - input file of graph edges\n");
    printf
	("       WEIGHT TYPE (optional)   - similarity or distance between nodes [S/D] (default S)\n");
    printf("       WEIGHT CUTOFF (optional) - edge weight cutoff value\n");
    printf
	("                                  range of values for S - 0..N (default 0)\n");
    printf
	("                                  range of values for D - 0..1 (default 1)\n\n");

    exit(EXIT_FAILURE);
}

void NidxWriteLogFile(NIDX_IO_SET * pt_ioset)
{
    char str_weight_type[11];
    FILE *fp;

    FOPEN(fp, pt_ioset->outfile_log, "w");

    if (pt_ioset->weight_type)
	strcpy(str_weight_type, "Distance");
    else
	strcpy(str_weight_type, "Similarity");

    /* print summary into STDOUT */
    fputs("*************\n", fp);
    fputs(" Netidx Log *\n", fp);
    fputs("*************\n", fp);
    fprintf(fp, " InputFile          %s\n", pt_ioset->infile);
    fprintf(fp, " WeightType         %s\n", str_weight_type);
    fprintf(fp, " WeightCutoff       %f\n", pt_ioset->weight_cutoff);
    fprintf(fp, " NumNodes           %d\n", pt_ioset->n_nodes);
    fprintf(fp, " NumEdges           %d\n", pt_ioset->n_edges);
    fprintf(fp, " NumEdgesPassed     %d\n", pt_ioset->n_edges_passed);
    fprintf(fp, " OutputLogFile      %s\n", pt_ioset->outfile_log);
    fprintf(fp, " OutputNidxFile     %s\n", pt_ioset->outfile_nidx);
    fprintf(fp, " OutputEidxFile     %s\n", pt_ioset->outfile_eidx);
    fprintf(fp, " OutputSubeFile     %s\n\n", pt_ioset->outfile_sube);

    FCLOSE(fp, pt_ioset->outfile_log);
}

/* set I/O file names */
NIDX_IO_SET NidxStoreIOset(int argc, char **args)
{
    unsigned int weight_type;
    float weight_cutoff;
    char infile[255];
    NIDX_IO_SET ioset;

    /* init variables */
    weight_type = 0;
    strcpy(infile, args[1]);

    /* parse weight type (optional) */
    if (argc > 2) {
	if (!strcmp(args[2], "S"))
	    weight_type = 0;
	else if (!strcmp(args[2], "D"))
	    weight_type = 1;
	else
	    NidxUsage();
    }

    /* parse weight cutoff (optional) */
    if (argc > 3) {
	weight_cutoff = atof(args[3]);

	if (weight_cutoff < 0 || (weight_cutoff > 1 && weight_type))
	    NidxUsage();
    } else
	weight_cutoff = (weight_type == 0) ? 0 : 1;

    /* set IO_SET elements */
    strcpy(ioset.infile, infile);
    ioset.weight_type = weight_type;
    ioset.weight_cutoff = weight_cutoff;

    sprintf(ioset.outfile_nidx, "%s.nidx", infile);
    sprintf(ioset.outfile_eidx, "%s.eidx", infile);
    sprintf(ioset.outfile_sube, "%s.sube", infile);
    sprintf(ioset.outfile_log, "%s.netindex.log", infile);

    return ioset;
}

/* write a header of the indexed edge file */
void WriteFileEidxHeader(FILE * fp, char *file_name)
{
    unsigned int num_edges = 0;

    /* reserve 4 bytes for total number of edges */
    FWRITE(fp, file_name, &num_edges, sizeof(unsigned int));
}

/* update header of the indexed edge file */
void UpdateFileEidxHeader(FILE * fp, char *file_name,
			  unsigned int num_edges)
{
    /* rewind file position indicator */
    rewind(fp);

    /* write total number of nodes into header */
    FWRITE(fp, file_name, &num_edges, sizeof(int));
}

/* write node index file - header */
void WriteFileNidxHeader(FILE * fp, char *file_name,
			 unsigned int node_buffer_size)
{
    unsigned int node_idx = 0;

    /* reserve 4 bytes for total number of nodes */
    FWRITE(fp, file_name, &node_idx, sizeof(int));

    /* write size of the node buffer (4 bytes) */
    FWRITE(fp, file_name, &node_buffer_size, sizeof(int));
}

/* write node index file - array of node labels */
void WriteFileNidx(FILE * fp, char *file_name, EDGE * pt_edge,
		   HASH_TABLE * pt_table, unsigned int *pt_node_idx)
{
    /* write node label A & increase the node index */
    if (AddNewKVPToHash(pt_table, pt_edge->nodeA, *pt_node_idx) != NULL) {
	FWRITE(fp, file_name, pt_edge->nodeA, sizeof(char) * NODE_BUF);
	*pt_node_idx += 1;
    }

    /* write node label A & increase the node index */
    if (AddNewKVPToHash(pt_table, pt_edge->nodeB, *pt_node_idx) != NULL) {
	FWRITE(fp, file_name, pt_edge->nodeB, sizeof(char) * NODE_BUF);
	*pt_node_idx += 1;
    }
}

/* update header of the node index file */
void UpdateFileNidxHeader(FILE * fp, char *file_name,
			  unsigned int *pt_node_idx)
{
    /* rewind file position indicator */
    rewind(fp);

    /* write total number of nodes into header */
    FWRITE(fp, file_name, pt_node_idx, sizeof(int));
}

/* create a new hash table */
HASH_TABLE *NewHashTable(unsigned int size)
{
    unsigned int i;
    HASH_TABLE *table;

    if (size < 1) {
	fputs("Error: Hash table cannot be of zero size.\n", stderr);
	exit(EXIT_FAILURE);
    }

    /* allocate memory */
    MALLOC(table, sizeof(HASH_TABLE));
    MALLOC(table->table, size * sizeof(LINKED_LIST *));

    /* init table */
    for (i = 0; i < size; i++)
	table->table[i] = NULL;

    /* set table size */
    table->size = size;

    return table;
}

/* hashing function */
unsigned int HashKeyToValue(const HASH_TABLE * pt_table, char *key)
{
    unsigned int value;

    for (value = 0; *key != '\0'; key++)
	value = *key + (value << 5) - value;

    return value % pt_table->size;
}

/* lookup function for hash value */
unsigned int *GetHashValue(HASH_TABLE * pt_table, char *key)
{
    unsigned int value;
    LINKED_LIST *list;

    value = HashKeyToValue(pt_table, key);

    for (list = pt_table->table[value]; list != NULL; list = list->next) {
	if (strcmp(key, list->key) == 0) {
	    return &list->value;
	}
    }
    return NULL;
}

/* add a key-value pair (KVP) to a hash table */
char *AddNewKVPToHash(HASH_TABLE * pt_table, char *key, unsigned int value)
{
    unsigned int hash_value;
    LINKED_LIST *new_list;

    if (GetHashValue(pt_table, key) == NULL) {	/* add KVP if new key found */
	/* allocate memory for the list */
	MALLOC(new_list, sizeof(LINKED_LIST));

	/* get hash value of the key */
	hash_value = HashKeyToValue(pt_table, key);

	/* update hash table */
	if ((new_list->key = (char *) strdup(key)) == NULL) {
	    fputs("Error: strdup() returned NULL. Out of memory!\n",
		  stderr);
	    exit(EXIT_FAILURE);
	}

	new_list->value = value;
	new_list->next = pt_table->table[hash_value];
	pt_table->table[hash_value] = new_list;

	return key;
    } else {
	return NULL;
    }
}

void IndexGraph(NIDX_IO_SET * pt_ioset)
{
    /* declare variables */
    unsigned int n_edges;
    unsigned int n_edges_skipped;
    unsigned int node_idx;
    float weight;
    float weight_cutoff;
    char line[LINE_BUF];
    char nodeA[NODE_BUF];
    char nodeB[NODE_BUF];
    HASH_TABLE *pt_table;
    EDGE edge, *pt_edge;
    IDX_EDGE iedge, *pt_iedge;
    FILE *fp_infile;
    FILE *fp_outfile_nidx;
    FILE *fp_outfile_eidx;
    FILE *fp_outfile_sube;

    /* set variables */
    n_edges = 0;
    n_edges_skipped = 0;
    node_idx = NODE_IDX;
    pt_edge = &edge;
    pt_iedge = &iedge;
    pt_table = NewHashTable(HASH_TABLE_SZ);
    weight_cutoff = pt_ioset->weight_cutoff;

    /* open I/O files for reading & writing */
    FOPEN(fp_infile, pt_ioset->infile, "rb");
    FOPEN(fp_outfile_nidx, pt_ioset->outfile_nidx, "wb");
    FOPEN(fp_outfile_eidx, pt_ioset->outfile_eidx, "wb");
    FOPEN(fp_outfile_sube, pt_ioset->outfile_sube, "wb");

    /* write header into node index file */
    WriteFileNidxHeader(fp_outfile_nidx, pt_ioset->outfile_nidx, NODE_BUF);

    /* write header into edge index file */
    WriteFileEidxHeader(fp_outfile_eidx, pt_ioset->outfile_eidx);

    /* read input file - To-do: put this block into a function to reduce overhead */
    while (fgets(line, LINE_BUF, fp_infile) != NULL) {
	/* parse input file with two or three columns */
	if (sscanf(line, "%s %s %f\n", nodeA, nodeB, &weight) == 3) {
	    n_edges++;

	    /* filter edges depending on weight type */
	    if (pt_ioset->weight_type) {	/* if distance values */
		if (weight > 1 || weight < 0) {
		    fputs
			("Input error: Edge weighes (distances) out of range.\n",
			 stderr);
		    remove(pt_ioset->outfile_nidx);
		    remove(pt_ioset->outfile_eidx);
		    remove(pt_ioset->outfile_sube);
		    exit(EXIT_FAILURE);
		}
		if (weight > weight_cutoff) {
		    n_edges_skipped++;
		    continue;
		}
	    } else {		/* if similarity values */

		if (weight < 0) {
		    fputs
			("Input error: Edge weights (similarities) cannot have negative values.\n",
			 stderr);
		    remove(pt_ioset->outfile_nidx);
		    remove(pt_ioset->outfile_eidx);
		    remove(pt_ioset->outfile_sube);
		    exit(EXIT_FAILURE);
		}

		if (weight < weight_cutoff) {
		    n_edges_skipped++;
		    continue;
		}
	    }

	    /* set EDGE structure */
	    strcpy(pt_edge->nodeA, nodeA);
	    strcpy(pt_edge->nodeB, nodeB);
	    pt_edge->weight = weight;

	    /* write a subset of edges into file */
	    fprintf(fp_outfile_sube, "%s\t%s\t%f\n", nodeA, nodeB, weight);

	    /* write node indices into file */
	    WriteFileNidx(fp_outfile_nidx, pt_ioset->outfile_nidx, pt_edge,
			  pt_table, &node_idx);

	    /* set IDX_EDGE structure */
	    pt_iedge->nodeA = *GetHashValue(pt_table, nodeA);
	    pt_iedge->nodeB = *GetHashValue(pt_table, nodeB);
	    pt_iedge->weight = weight;

	    /* write indexed edges into file */
	    FWRITE(fp_outfile_eidx, pt_ioset->outfile_eidx, pt_iedge,
		   sizeof(IDX_EDGE));
	} else {
	    fputs("Error: Input file format is incorrect!\n", stderr);

	    /* clean-up */
	    remove(pt_ioset->outfile_nidx);
	    remove(pt_ioset->outfile_eidx);
	    remove(pt_ioset->outfile_sube);
	    exit(EXIT_FAILURE);
	}
    }

    /* remove file(s) depending on the indexing results */
    if (n_edges == n_edges_skipped) {	/* if no edges left after filtering */
	remove(pt_ioset->outfile_nidx);
	remove(pt_ioset->outfile_eidx);
	remove(pt_ioset->outfile_sube);

	strcpy(pt_ioset->outfile_nidx, "(None)");
	strcpy(pt_ioset->outfile_eidx, "(None)");
	strcpy(pt_ioset->outfile_sube, "(None)");
    } else if (!n_edges_skipped) {	/* if input & output graphs are the same */
	remove(pt_ioset->outfile_sube);
	strcpy(pt_ioset->outfile_sube, "(None)");
    }

    /* update node index file */
    UpdateFileNidxHeader(fp_outfile_nidx, pt_ioset->outfile_nidx,
			 &node_idx);
    UpdateFileEidxHeader(fp_outfile_eidx, pt_ioset->outfile_eidx,
			 n_edges - n_edges_skipped);

    /* set I/O parameters */
    pt_ioset->n_nodes = node_idx;
    pt_ioset->n_edges = n_edges;
    pt_ioset->n_edges_passed = n_edges - n_edges_skipped;

    /* close I/O files */
    FCLOSE(fp_infile, pt_ioset->infile);
    FCLOSE(fp_outfile_nidx, pt_ioset->outfile_nidx);
    FCLOSE(fp_outfile_eidx, pt_ioset->outfile_eidx);
    FCLOSE(fp_outfile_sube, pt_ioset->outfile_sube);
}
