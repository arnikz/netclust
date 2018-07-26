/*
 * netclust is an efficient graph-based program for detecting single-linkage
 * clusters in very large biological graphs (networks). The program makes use of
 * an efficient implementation of the union-find algorithm (UFA) that requires
 * (nearly) linear time and space complexity (Tarjan and van Leeuwen, 1984).
 * netclust takes the index graph files (.nidx and .eidx) and outputs clusters
 * into a text file (.clst) or STDOUT, including a log (.netclust.log).
 */

#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "netclust.h"

int 
main(int argc, char **argv)
{
	/* declare variables */
	PARTITION *par;
	NCLS_IO_SET	ioset, *pt_ioset;
	NIDX_TABLE	table, *pt_table;

	pt_ioset = &ioset;
	pt_table = &table;

	/* validate the number of input args */
	if (argc < 2 || argc > 5)
		NclsUsage();

	/* store IO parameters */
	ioset = NclsStoreIOset(argc, argv);

	/* store node indices into lookup table */
	StoreNodeIdx(pt_table, pt_ioset);

	/* retrieve all graph clusters (partition) */
	par = GetGraphPartition(pt_table, pt_ioset);

	/* output clusters */
	OutputGraphPartition(par, pt_ioset);

	/* write a log file */
	NclsWriteLogFile(pt_ioset);

	exit(EXIT_SUCCESS);
}
