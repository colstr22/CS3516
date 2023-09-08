#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts   *neighbor2;

void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr );

/* students to write the following two routines, and maybe some others */

void rtinit2() {
    printf("rtinit2 called at: %f\n", clocktime);
    // Initialize dt
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == 2) {
                dt2.costs[i][j] = 0;
            }
            else {
                dt2.costs[i][j] = INFINITY;
            }
        }
    }   

    // Neighbors
    neighbor2 = getNeighborCosts(2);
    if (TraceLevel >= 1) printf("Node 2 called getNeighborCosts(2) and at %f got : \n", clocktime);
    for (int i = 0; i < neighbor2->NodesInNetwork; i++) {
        dt2.costs[i][2] = neighbor2->NodeCosts[i];
        printf("(%i:%i) ", i, neighbor2->NodeCosts[i]);
    }
    if (TraceLevel >= 1) printf("\n");

    // Make Packet
    struct RoutePacket outpacket2;
    outpacket2.sourceid = 2;
    for (int i = 0; i < neighbor2->NodesInNetwork; i++) {
        outpacket2.mincost[i] = neighbor2->NodeCosts[i];
    }

    // Send out Packets
    if (TraceLevel >= 1) printf("Node 2 sends packets to");
    for (int i = 0; i < neighbor2->NodesInNetwork; i++) {
        if (neighbor2->NodeCosts[i] == INFINITY || i == 2) {
            continue;
        }
        outpacket2.destid = i;
        printf(" %i", i);
        toLayer2(outpacket2);
    }
    if (TraceLevel >= 1) printf("\n");
    if (TraceLevel >= 2) printdt2(2, neighbor2, &dt2);
}


void rtupdate2( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate1 called at %f\n", clocktime);
    int sender = rcvdpkt->sourceid;
    int receiver = rcvdpkt->destid;
    int mincost[neighbor2->NodesInNetwork];
    mincost[0] = rcvdpkt->mincost[0];
    mincost[1] = rcvdpkt->mincost[1];
    mincost[2] = rcvdpkt->mincost[2];
    mincost[3] = rcvdpkt->mincost[3];
    int changed = FALSE;
    // For each Node
    for (int i = 0; i < neighbor2->NodesInNetwork; i++) {    
        // If your edge plus 
        int new_dist2 = mincost[i] + dt2.costs[sender][i];
        if (new_dist2 < dt2.costs[i][sender]) {
            dt2.costs[sender][i] = new_dist2;
            changed = TRUE;
        }
    }
    
    if (changed == TRUE) {
        struct RoutePacket outpacket2;
        outpacket2.sourceid = 2; 
        outpacket2.mincost[0] = dt2.costs[2][0];
        outpacket2.mincost[1] = dt2.costs[2][1];
        outpacket2.mincost[2] = dt2.costs[2][2];
        outpacket2.mincost[3] = dt2.costs[2][3];

        for (int i = 0; i < neighbor2->NodesInNetwork; i++) {
            if (i == 2) {
                continue;
            }
            outpacket2.destid = i;
            toLayer2(outpacket2);
        }
    }
    printdt2(2, neighbor2, &dt2);
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
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt2

