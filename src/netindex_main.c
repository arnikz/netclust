/*
 * netindex is a graph (network) indexing program used together with netclust
 * for detecting clusters in the graph. It takes graph edges in the form
 * [nodeA] [nodeB] [weightAB] and generates several output files:
 *  .nidx - binary file with node indices
 *  .eidx - binary file with indexed edges
 *  .sube - text file with a subset of input edges; created only if at least
 *          one edge passes a weight threshold
 *  .netindex.log - log file includes information about netindex execution
 */

#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "netindex.h"

int 
main(int argc, char **argv)
{
	NIDX_IO_SET	ioset, *pt_ioset;

	pt_ioset = &ioset;

	/* validate the number of input parameters */
	if (argc < 2 || argc > 4)
		NidxUsage();

	/* store IO parameters */
	ioset = NidxStoreIOset(argc, argv);

	/* index the input graph */
	IndexGraph(pt_ioset);

	/* write log file */
	NidxWriteLogFile(pt_ioset);

	exit(EXIT_SUCCESS);
}
