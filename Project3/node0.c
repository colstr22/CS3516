#include <stdio.h>
#include "project3.h"
#include <string.h>

extern int TraceLevel;
struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts   *neighbor0;

void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, 
	struct distance_table *dtptr );
/* students to write the following two routines, and maybe some others */

void rtinit0() {
    printf("rtinit0 called at: %f\n", clocktime);
    // Initialize dt
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == 0) {
                dt0.costs[i][j] = 0;
            }
            else {
                dt0.costs[i][j] = INFINITY;
            }
        }
    }

    // Neighbors
    neighbor0 = getNeighborCosts(0);
    if (TraceLevel >= 1) printf("Node 0 called getNeighborCosts(0) and at %f got : \n", clocktime);
    for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
        dt0.costs[i][0] = neighbor0->NodeCosts[i];
        printf("(%i:%i) ", i, neighbor0->NodeCosts[i]);
    }
    if (TraceLevel >= 1) printf("\n");

    struct RoutePacket outpacket0;
    outpacket0.sourceid = 0;
    for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
        outpacket0.mincost[i] = neighbor0->NodeCosts[i];
    }
    if (TraceLevel >= 1) printf("Node 0 sends packets to");
    for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
        if (neighbor0->NodeCosts[i] == INFINITY || i == 0) {
            continue;
        }
        outpacket0.destid = i;
        printf(" %i", i);
        toLayer2(outpacket0);
    }
    if (TraceLevel >= 1) printf("\n");
    if (TraceLevel >= 2) printdt0(0, neighbor0, &dt0);
}

void rtupdate0( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate0 called at %f\n", clocktime);
    int sender = rcvdpkt->sourceid;
    int receiver = rcvdpkt->destid;
    int mincost[neighbor0->NodesInNetwork];
    mincost[0] = rcvdpkt->mincost[0];
    mincost[1] = rcvdpkt->mincost[1];
    mincost[2] = rcvdpkt->mincost[2];
    mincost[3] = rcvdpkt->mincost[3];
    int changed = FALSE;

    // for each edge out from sender)
    for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
        if (i == 0) {
            continue;
        }
        int weight = neighbor0->NodeCosts[i];
        int new_dist0 = dt0.costs[sender][0] + weight;
        if (new_dist0 < dt0.costs[i][sender]) {                
            dt0.costs[i][sender] = new_dist0;
            changed = TRUE;
        }        
    }
    
    if (changed == TRUE) {
        struct RoutePacket outpacket0;
        outpacket0.sourceid = 0; 
        outpacket0.mincost[0] = dt0.costs[0][0];
        outpacket0.mincost[1] = dt0.costs[0][1];
        outpacket0.mincost[2] = dt0.costs[0][2];
        outpacket0.mincost[3] = dt0.costs[0][3];

        for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
            if (i == 0) {
                continue;
            }
            outpacket0.destid = i;
            toLayer2(outpacket0);
        }
        if (TraceLevel == 2) printdt0(0, neighbor0, &dt0);
    }

}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt0

