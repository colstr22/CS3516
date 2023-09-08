#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt3;
struct NeighborCosts   *neighbor3;

/* students to write the following two routines, and maybe some others */
void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr );

void rtinit3() {
    printf("rtinit3 called at: %f\n", clocktime);
    // Initialize dt
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == 3) {
                dt3.costs[i][j] = 0;
            }
            else {
                dt3.costs[i][j] = INFINITY;
            }
        }
    }   

    // Neighbors
    neighbor3 = getNeighborCosts(3);
    if (TraceLevel >= 1) printf("Node 3 called getNeighborCosts(3) and at %f got : \n", clocktime);
    for (int i = 0; i < neighbor3->NodesInNetwork; i++) {
        dt3.costs[i][3] = neighbor3->NodeCosts[i];
        printf("(%i:%i) ", i, neighbor3->NodeCosts[i]);
    }
    if (TraceLevel >= 1) printf("\n");

    // Make Packet
    struct RoutePacket outpacket3;
    outpacket3.sourceid = 3;
    for (int i = 0; i < neighbor3->NodesInNetwork; i++) {
        outpacket3.mincost[i] = neighbor3->NodeCosts[i];
    }

    // Send out Packets
    if (TraceLevel >= 1) printf("Node 3 sends packets to");
    for (int i = 0; i < neighbor3->NodesInNetwork; i++) {
        if (neighbor3->NodeCosts[i]  || i == 3) {
            continue;
        }
        outpacket3.destid = i;
        printf(" %i", i);
        toLayer2(outpacket3);
    }
    if (TraceLevel >= 1) printf("\n");
    if (TraceLevel >= 3) printdt3(3, neighbor3, &dt3);
}


void rtupdate3( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate1 called at %f\n", clocktime);
    int sender = rcvdpkt->sourceid;
    int receiver = rcvdpkt->destid;
    int mincost[neighbor3->NodesInNetwork];
    mincost[0] = rcvdpkt->mincost[0];
    mincost[1] = rcvdpkt->mincost[1];
    mincost[2] = rcvdpkt->mincost[2];
    mincost[3] = rcvdpkt->mincost[3];
    int changed = FALSE;
    for (int i = 0; i < neighbor3->NodesInNetwork; i++) {
        int new_dist3 = mincost[i] + dt3.costs[sender][i];
        if (new_dist3 < dt3.costs[i][sender]) {
            dt3.costs[sender][i] = new_dist3;
            changed = TRUE;
        }
    }
    
    if (changed == TRUE) {
        struct RoutePacket outpacket3;
        outpacket3.sourceid = 3; 
        outpacket3.mincost[0] = dt3.costs[3][0];
        outpacket3.mincost[1] = dt3.costs[3][1];
        outpacket3.mincost[2] = dt3.costs[3][2];
        outpacket3.mincost[3] = dt3.costs[3][3];

        for (int i = 0; i < neighbor3->NodesInNetwork; i++) {
            if (i == 3) {
                continue;
            }
            outpacket3.destid = i;
            toLayer2(outpacket3);
        }
    }
    printdt3(3, neighbor3, &dt3);
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
void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt3

