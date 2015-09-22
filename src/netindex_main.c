/*
 * File:	netindex_main.c
 * Author:	Arnold Kuzniar
 * Date:	29-04-2008
 * Version:	1.0
 *
 ****************************************************************************
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
 * Program description: 
 *  netindex is a graph (network) indexing program used to index the input graph
 *  for the netclust clustering program. netindex creates node/edge indices in
 *  binary format to reduce the overhead of parsing text.
 *
 * Inputs:
 *  FILE (mandatory) - input text file containing graph edges (one per line) in the follwing format:
 *
 *   [nodeA] [nodeB] [weightAB]
 *
 *  WEIGHT TYPE - edge weights might refer to either similarity (S) or distance (D) measure (default S)
 *  WEIGHT CUTOFF - cutoff value for edge weights
 *
 *  Output files:
 * 
 *   *.nidx - a binary file of node indices
 *   *.eidx - a binary file of indexed edges
 *   *.sube - a text file containing a subset of input edges (created only if theat least one edge passes a weight threshold) 
 *   *.netindex.log - a log file with some information about a run
 */

#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "netindex.h"

int main(int argc, char **argv)
{
    NIDX_IO_SET ioset, *pt_ioset;

    pt_ioset = &ioset;

    /* validate the number of input parameters */
    if (argc < 2 || argc > 4)
	NidxUsage();

    /* store IO parameters */
    ioset = NidxStoreIOset(argc,argv);

    /* index the input graph */
    IndexGraph(pt_ioset);

    /* write log file */
    NidxWriteLogFile(pt_ioset);

    exit(EXIT_SUCCESS);
}
