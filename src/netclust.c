#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
#include "netclust.h"

/* print program usage */
void 
NclsUsage()
{
	printf
		("************\n");
	printf
		("* Netclust *\n");
	printf
		("********************************************************************\n");
	printf
		("* Version:\t%s\n", VER);
	printf
		("* Author:\tArnold Kuzniar\n");
	printf
		("* Description:\tDetect single-linkage clusters of nodes in a graph.\n");
	printf
		("********************************************************************\n");
	printf
		("Usage:\n");
	printf
		("\tnetclust FILE [OUTPUT MODE] [WEIGHT TYPE] [WEIGHT CUTOFF]\n\n");
	printf
		("Mandatory args:\n");
	printf
		("\tFILE            - input graph file (indexed by netindex)\n\n");
	printf
		("Optional args:\n");
	printf
		("\tOUTPUT_MODE     - output clusters into FILE [F1/F2] or STDOUT [S1/S2] (default: F1)\n");
	printf
		("\tWEIGHT TYPE     - similarity or distance between nodes [S/D] (default: S)\n");
	printf
		("\tWEIGHT CUTOFF   - edge weight cutoff value\n");
	printf
		("\t\t\t  range of values for S - [0..N] (default: 0)\n");
	printf
		("\t\t\t  range of values for D - [0..1] (default: 1)\n\n");
	exit(EXIT_FAILURE);
}

/* read node indices into table from file */
unsigned int 
StoreNodeIdx(NIDX_TABLE *table, NCLS_IO_SET *pt_ioset)
{
	unsigned int i, n_nodes, node_buffer_size;
	FILE *fp;

	/* open file for reading */
	FOPEN(fp, pt_ioset->infile_nidx, "r");

	/* read the file header & get the number of nodes */
	if (fread(&n_nodes, sizeof(unsigned int), 1, fp) == 0)
		OOPS(pt_ioset->infile_nidx);

	/* read the file header & get the node buffer size  */
	if (fread(&node_buffer_size, sizeof(unsigned int), 1, fp) == 0)
		OOPS(pt_ioset->infile_nidx);

	/* allocate memory for the lookup table */
	MALLOC(table->nodes, n_nodes * sizeof(unsigned char *));

	for (i = 0; i < n_nodes; i++) {
		MALLOC(table->nodes[i], node_buffer_size * sizeof(unsigned char));

		/* read the file & fill-in the lookup table */
		if (fread
		    (table->nodes[i], node_buffer_size * sizeof(unsigned char), 1,
		     fp) == 0)
			OOPS(pt_ioset->infile_nidx);
	}

	/* update table size */
	table->table_size = n_nodes;

	/* close the index file */
	FCLOSE(fp, pt_ioset->infile_nidx);

	return 0;
}

/* set I/O file names */
NCLS_IO_SET 
NclsStoreIOset(int argc, char **args)
{
	unsigned int weight_type;
	unsigned int output_mode;
	float        weight_cutoff;
	NCLS_IO_SET	 ioset, *pt_ioset;

	/* init variables */
	pt_ioset = &ioset;
	weight_type = 0;	/* 0 - similarity */
	output_mode = 3;
	
	/* parse output mode */
	if (argc > 2) {
		if (!strcmp(args[2], "S1"))
			output_mode = 1;
		else if (!strcmp(args[2], "S2"))
			output_mode = 2;
		else if (!strcmp(args[2], "F1"))
			output_mode = 3;
		else if (!strcmp(args[2], "F2"))
			output_mode = 4;
		else
			NclsUsage();
	}
	/* parse weight type (optional parameter) */
	if (argc > 3) {
		if (!strcmp(args[3], "S"))	/* if similarity */
			weight_type = 0;
		else if (!strcmp(args[3], "D"))	/* if distance */
			weight_type = 1;
		else
			NclsUsage();
	}
	/* parse weight cutoff (optional parameter) */
	if (argc > 4) {
		weight_cutoff = atof(args[4]);
		if (weight_cutoff < 0 || (weight_cutoff > 1 && weight_type == 1))
			NclsUsage();
	} else
		weight_cutoff = (weight_type == 0) ? 0 : 1;

	/* set IO_SET elements */
	strcpy(pt_ioset->infile, args[1]);
	pt_ioset->output_mode = output_mode;
	pt_ioset->weight_type = weight_type;
	pt_ioset->weight_cutoff = weight_cutoff;

	sprintf(pt_ioset->infile_nidx, "%s.nidx", pt_ioset->infile);
	sprintf(pt_ioset->infile_eidx, "%s.eidx", pt_ioset->infile);
	sprintf(pt_ioset->outfile_log, "%s.netclust.log", pt_ioset->infile);

	/* set output cluster file */
	if (output_mode == 3 || output_mode == 4)
		sprintf(pt_ioset->outfile_clst, "%s.clst", pt_ioset->infile);
	else
		sprintf(pt_ioset->outfile_clst, "None [STDOUT]");

	return ioset;
}

/* cluster size comparison function for qsort() */
int 
CmpBySize(const void *pt_a, const void *pt_b)
{
	const CLUSTER *pt_ca = (const CLUSTER *)pt_a;
	const CLUSTER *pt_cb = (const CLUSTER *)pt_b;

	return (const int)pt_cb->size - pt_ca->size;
}

/* find clusters in a graph */
PARTITION *
GetGraphPartition(NIDX_TABLE *table, NCLS_IO_SET *pt_ioset)
{
	unsigned int i, j, k;
	unsigned int weight_type, n_nodes, n_edges, n_edges_passed;
	unsigned int n_clusters, cls_size, cls_id, new_cls_id, mem_id;
	unsigned int *set, *set_size, *cls_conv;
	float        weight, weight_cutoff;
	IDX_EDGE     iedge, *pt_iedge;
	PARTITION    *par;
	CLUSTER      *cls;
	FILE         *fp;

	n_edges_passed = 0;
	new_cls_id = 0;
	cls_size = 0;
	pt_iedge = &iedge;

	/* allocate memory for union sets and set sizes */
	n_nodes = table->table_size;
	MALLOC(set, sizeof(unsigned int) * n_nodes);
	MALLOC(set_size, sizeof(unsigned int) * n_nodes);
	MALLOC(cls_conv, sizeof(unsigned int) * n_nodes);

	for (i = 0; i < n_nodes; i++) {
		set[i] = i;
		set_size[i] = 1;
		cls_conv[i] = 0;
	}
	FOPEN(fp, pt_ioset->infile_eidx, "rb");

	if (fread(&n_edges, sizeof(unsigned int), 1, fp) == 0)
		OOPS(pt_ioset->infile_eidx);

	for (i = n_edges; i != 0; i--) {
		/* get indexed edge (nodes) */
		if (fread(pt_iedge, sizeof(IDX_EDGE), 1, fp) == 0)
			OOPS(pt_ioset->infile_eidx);
		weight = pt_iedge->weight;
		weight_cutoff = pt_ioset->weight_cutoff;
		weight_type = pt_ioset->weight_type;

		/* filter edges depending on weight type */
		if ((weight_type && weight > weight_cutoff)
		    || (!weight_type && weight < weight_cutoff))
			continue;

		n_edges_passed++;

		/* set union algorithm */
		for (j = pt_iedge->nodeA; j != set[j]; j = set[j])
			set[j] = set[set[j]];

		for (k = pt_iedge->nodeB; k != set[k]; k = set[k])
			set[k] = set[set[k]];

		if (j == k)
			continue;

		if (set_size[j] < set_size[k]) {
			set[j] = k;
			set_size[k] += set_size[j];
		} else {
			set[k] = j;
			set_size[j] += set_size[k];
		}
	}
	FCLOSE(fp, pt_ioset->infile_eidx);

	free(set_size);

	/* post-process sets by finding nodes' roots */
	for (i = 0; i < n_nodes; i++) {
		cls_id = set[i];

		/* find the root of a node */
		if (cls_id != i) {
			cls_id = FindRoot(set, cls_id);
			set[i] = cls_id;
		}
		/* assign new clusterIDs */
		if (!cls_conv[cls_id]) {
			new_cls_id++;
			cls_conv[cls_id] = new_cls_id;	/* cluster ID starts with 1 */
		}
	}

	/* get cluster count (including singletons) */
	n_clusters = new_cls_id;

	/* init CLUSTER structure */
	MALLOC(cls, sizeof(CLUSTER) * n_clusters);

	for (i = 0; i < n_clusters; i++) {
		cls[i].id = 0;
		cls[i].size = 0;
	}

	/* store cluster sizes */
	for (i = 0; i < n_nodes; i++)
		cls[cls_conv[set[i]] - 1].size++;

	/* store cluster members */
	for (i = 0; i < n_nodes; i++) {
		cls_id = cls_conv[set[i]] - 1;
		cls_size = cls[cls_id].size;

		if (!cls[cls_id].id) {
			MALLOC(cls[cls_id].member, sizeof(unsigned char *) * cls_size);

			for (j = 0; j < cls_size; j++)
				MALLOC(cls[cls_id].member[j],
				       sizeof(unsigned char) * NODE_BUF);

			cls[cls_id].id = 1;
		}
		mem_id = cls[cls_id].id - 1;
		sprintf((char *)cls[cls_id].member[mem_id], "%s",
			(char *)table->nodes[i]);
		cls[cls_id].id++;
	}

	free(set);
	free(cls_conv);

	/* sort clusters by size */
	qsort(cls, n_clusters, sizeof(CLUSTER), CmpBySize);

	MALLOC(par, sizeof(PARTITION));
	par->size = n_clusters;
	par->cluster = cls;
	pt_ioset->n_edges = n_edges_passed;

	return par;
}

/* find the root of a node */
unsigned int 
FindRoot(unsigned int *set, unsigned int node)
{
	if (set[node] == node)
		return node;
	else
		return FindRoot(set, set[node]);
}

/* write clusters into file */
unsigned int 
OutputGraphPartition(PARTITION *par, NCLS_IO_SET *pt_ioset)
{
	unsigned int i, j, sz, mode, n_nodes, n_clusters;
	CLUSTER      *cls;
	FILE         *fp;

	fp = NULL;
	n_nodes = 0;
	n_clusters = 0; /* number of non-singleton clusters */
	mode = pt_ioset->output_mode;
	cls = par->cluster;

	/*
	 * open cluster file for writing only if output mode is 3 (F1) or 4
	 * (F2)
	 */
	if (mode > 2)
		FOPEN(fp, pt_ioset->outfile_clst, "w");

	for (i = 0; i < par->size; i++) {
		sz = cls[i].size;
		if (sz < 2)
			continue;	/* skipp singletons */
		if (mode == 2)
			printf("%d\t%d\t", i + 1, sz);
		else if (mode == 4)
			fprintf(fp, "%d\t%d\t", i + 1, sz);

		n_nodes += sz;
		n_clusters++;

		for (j = 0; j < sz; j++) {
			switch (mode) {
			case 1:
				printf("%d\t%s\n", i + 1, cls[i].member[j]);
				break;
			case 2:
				printf("%s ", cls[i].member[j]);
				break;
			case 3:
				fprintf(fp, "%d\t%s\n", i + 1, cls[i].member[j]);
				break;
			case 4:
				fprintf(fp, "%s ", cls[i].member[j]);
				break;
			default:
				fprintf(stderr, "Error:Output mode not supported.");
				break;
			}
		}

		if (mode == 2)
			printf("\n");
		else if (mode == 4)
			fputs("\n", fp);
	}

	/* close file */
	if (mode > 2)
		FCLOSE(fp, pt_ioset->outfile_clst);

	/* update PARTITION */
	pt_ioset->n_nodes = n_nodes;
	pt_ioset->n_clusters = n_clusters;
	return 0;
}

/* write clustering summary into log file */
void 
NclsWriteLogFile(NCLS_IO_SET *pt_ioset)
{
	FILE *fp;

	/* open log file */
	FOPEN(fp, pt_ioset->outfile_log, "w");

	/* write log file */
	fprintf(fp, "****************\n");
	fprintf(fp, "* Netclust Log *\n");
	fprintf(fp, "****************\n");
	fprintf(fp, " InputFile\t\t%s\n", pt_ioset->infile);
	fprintf(fp, " InputNidxFile\t\t%s\n", pt_ioset->infile_nidx);
	fprintf(fp, " InputEidxFile\t\t%s\n", pt_ioset->infile_eidx);
	fprintf(fp, " OutputClstFile\t\t%s\n", pt_ioset->outfile_clst);
	fprintf(fp, " OutputLogFile\t\t%s\n", pt_ioset->outfile_log);

	if (pt_ioset->weight_type == 0)
		fprintf(fp, " WeightType\t\tSimilarity\n");
	else
		fprintf(fp, " WeightType\t\tDistance\n");

	fprintf(fp, " WeightCutoff\t\t%.2f\n", pt_ioset->weight_cutoff);
	fprintf(fp, " NumNodes\t\t%d\n", pt_ioset->n_nodes);
	fprintf(fp, " NumEdges\t\t%d\n", pt_ioset->n_edges);
	fprintf(fp, " NumClusters\t\t%d\n\n", pt_ioset->n_clusters);

	/* close log file */
	FCLOSE(fp, pt_ioset->outfile_log);
}
