#include <stdio.h>
#include "project3.h"
#include <string.h>

extern int TraceLevel;
struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt1;
struct NeighborCosts   *neighbor1;

void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr );

/* students to write the following two routines, and maybe some others */

void rtinit1() {
    printf("rtinit1 called at: %f\n", clocktime);    
    // Initialize dt
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == 1) {
                dt1.costs[i][j] = 0;
            }
            else {
                dt1.costs[i][j] = INFINITY;
            }
        }
    }

    // Neighbors
    neighbor1 = getNeighborCosts(1);
    if (TraceLevel >= 1) printf("Node 1 called getNeighborCosts(1) and at %f got : \n", clocktime);
    for (int i = 0; i < neighbor1->NodesInNetwork; i++) {
        dt1.costs[i][1] = neighbor1->NodeCosts[i];
        printf("(%i:%i) ", i, neighbor1->NodeCosts[i]);
    }
    if (TraceLevel >= 1) printf("\n");

    struct RoutePacket outpacket1;
    outpacket1.sourceid = 1;
    for (int i = 0; i < neighbor1->NodesInNetwork; i++) {
        outpacket1.mincost[i] = neighbor1->NodeCosts[i];
    }
    if (TraceLevel >= 1) printf("Node 1 sends packets to");
    for (int i = 0; i < neighbor1->NodesInNetwork; i++) {
        if (dt1.costs[i][1] == INFINITY || i == 1) {
            continue;
        }
        outpacket1.destid = i;
        printf(" %i", i);
        toLayer2(outpacket1);
    }
    if (TraceLevel >= 1) printf("\n");
    if (TraceLevel >= 2) printdt1(1, neighbor1, &dt1);
}


void rtupdate1( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate1 called at %f\n", clocktime);
    int sender = rcvdpkt->sourceid;
    int receiver = rcvdpkt->destid;
    int mincost[neighbor1->NodesInNetwork];
    mincost[0] = rcvdpkt->mincost[0];
    mincost[1] = rcvdpkt->mincost[1];
    mincost[2] = rcvdpkt->mincost[2];
    mincost[3] = rcvdpkt->mincost[3];
    int changed = FALSE;
    for (int i = 0; i < neighbor1->NodesInNetwork; i++) {
        int new_dist1 = mincost[i] + dt1.costs[sender][i];
        if (new_dist1 < dt1.costs[i][sender]) {
            dt1.costs[sender][i] = new_dist1;
            changed = TRUE;
        }
    }
    
    if (changed == TRUE) {
        struct RoutePacket outpacket1;
        outpacket1.sourceid = 2; 
        outpacket1.mincost[0] = dt1.costs[1][0];
        outpacket1.mincost[1] = dt1.costs[1][1];
        outpacket1.mincost[2] = dt1.costs[1][2];
        outpacket1.mincost[3] = dt1.costs[1][3];

        for (int i = 0; i < neighbor1->NodesInNetwork; i++) {
            if (i == 1) {
                continue;
            }
            outpacket1.destid = i;
            toLayer2(outpacket1);
        }
    }
    printdt1(1, neighbor1, &dt1);
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
void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt1

