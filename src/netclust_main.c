/*
 * File:	netclust_main.c
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
 *
 ****************************************************************************
 *
 * Program description:
 *  netclust is a very efficient graph-based clustering program suitable for
 *  finding single-linkage clusters in very large biological graphs (networks).
 *  The program makes a use of an efficient implementation of the union-find
 *  algorithm (UFA) that requires linear space and has a quasi-linear time
 *  complexity [Tarjan and van Leeuwen, 1984].
 *
 * Usage:
 *  Before the actual clustering process, the user must first index the input
 *  graph using the netindex program. 
 * 
 * Inputs and outputs:
 *  FILE (mandatory) - input file of a (non-indexed) graph
 *  
 *  OUTPUT MODE (mandatory) - output clusters either into STDOUT (O) or into a file (F)
 *      Clusters can be written in two distinct formats:
 *      
 *      O1/F1 - one cluster member per line, the columns are separated by "\t"
 *      
 *         [cluster] [member]
 *
 *      O2/F2 - all cluster members per line, the columns are separated by "\t",
 *      cluster members are separated by " " 
 *
 *         [cluster] [cluster size] [members]
 *  
 *  WEIGHT TYPE - edge weights might refer to either similarity (S) or distance (D) measure (default S)
 *  WEIGHT CUTOFF - cutoff value for edge weights
 *
 *  Note:
 *   The netclust program does not process the input FILE per se, but it uses two binary files
 *   (*.nidx and *.eidx) derived by indexing of the input FILE. Clusters are written into an
 *   output file (*.clst) only if F1/F2 output modes are selected. A log file (*.netindex.log)
 *   is written for each netclust run.
 */

#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "netclust.h"

int main(int argc, char **argv)
{
    /* declare variables */
    PARTITION *par;
    NCLS_IO_SET ioset, *pt_ioset;
    NIDX_TABLE table, *pt_table;

    pt_ioset = &ioset;
    pt_table = &table;

    /* validate the number of input args */
    if (argc < 3 || argc > 5)
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

