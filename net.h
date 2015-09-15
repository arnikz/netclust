/* 
 * File:	net.h
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

#ifndef NET_H
#define	NET_H

#define _FILE_OFFSET_BITS 64	/* large file support (LFS) */
#define LINE_BUF 100		/* line buffer */
#define NODE_BUF 30		/* node alias buffer */
#define OOPS(s) { perror(s); exit(EXIT_FAILURE); }	/* exit with error message */
#define MALLOC(s, t) if (((s) = malloc(t)) == NULL) OOPS("Error: malloc()\n");	/* allocate memory */
#define FOPEN(fp, fn, fm) if ((fp = fopen(fn, fm)) == NULL) OOPS(fn);
#define FCLOSE(fp, fn) if (fclose(fp)) OOPS(fn);	/* close file */

/* typedefs */
typedef struct _idx_edge_ IDX_EDGE;	/* indexed graph edge */
typedef struct _edge_ EDGE;	/* graph edge */

/* type declarations */
struct _edge_ {
    char nodeA[NODE_BUF];
    char nodeB[NODE_BUF];
    float weight;
};

struct _idx_edge_ {
    unsigned int nodeA;		/* index node A */
    unsigned int nodeB;		/* index node B */
    float weight;		/* edge weight */
};

#endif				/* NET_H */
