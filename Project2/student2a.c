#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
#include <string.h>
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/

// Constants
int MAX_PKT_QUEUE_SIZE = 100;
double INTERRUPT_INTERVAL = 1500.0;

// States
int current_seqnum_A;
int acked_seqnum_A;
int current_seqnum_B;
int waiting_for_ack_A;

// Queue Implementation
struct pkt_slot{
  struct pkt packet;
  struct pkt_slot* next;
};
int pkt_queue_size;
struct pkt_slot* pkt_queue;         // Points to the head of the queue
struct pkt_slot* last_pkt_in_queue; // Points to the tail of the queue

// Helper Functions
//calculate checksum; this function comes from internet
void update_queue() {
  while (pkt_queue->packet.seqnum + 20 != acked_seqnum_A) {
    // Dequeue last unacked pkt up until correct
    struct pkt_slot* temp = pkt_queue;
    pkt_queue = pkt_queue->next;
    pkt_queue_size--;
    free(temp);
  }
}
int get_checksum(struct pkt *packet) {
  char* vdata = packet->payload;
  int acknum = packet->acknum;
  int seqnum = packet->seqnum;
  int i, checksum = 0;
  for(i = 0; i < MESSAGE_LENGTH; i++){
    checksum += (int)(vdata[i]) * i;
  }
  checksum += acknum * 21;
  checksum += seqnum * 22;
  return checksum;
}

void send_ack(int AorB, int ack) {
  if ( TraceLevel >= 2) printf("  Sending ACK with seqnum %i\n", ack);
  struct pkt packet;
  packet.acknum = ack;
  packet.checksum = get_checksum(&packet);
  tolayer3(AorB, packet);
}

void print_queue(){
  int i = 0;
  struct pkt_slot* current = pkt_queue;
  printf("  Printing queue:\n");
  while (current != NULL) {
    printf("    Packet %i: %s\n", i, (current->packet).payload);
    current = current->next;
    i++;
  }
}

void resend_packet() {
  stopTimer(AEntity);
  struct pkt_slot* packet_slot = pkt_queue;
  int total = pkt_queue_size;
  for (int i = 0; i < total; i++) {
    if ( TraceLevel >= 2) printf("  Resending ACK with seqnum %i\n", packet_slot->packet.seqnum);
    tolayer3(AEntity, packet_slot->packet);
    packet_slot = packet_slot->next;
  }
  startTimer(AEntity, INTERRUPT_INTERVAL);
}

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
  /* if (waiting_for_ack_A) {
    if ( TraceLevel >= 2 )  printf("  A_output: not yet acked. drop the message: %s\n", message.data);
    return;
  } */
  
  if (pkt_queue_size == MAX_PKT_QUEUE_SIZE) {
    if ( TraceLevel >= 2 )  printf("  A_output: pakcet queue full. drop the message: %s\n", message.data);
    return;
  }
  // Make pkt
  struct pkt new_packet;
  memcpy(new_packet.payload, message.data, 20);
  new_packet.seqnum = current_seqnum_A;
  new_packet.checksum = get_checksum(&new_packet);
  // Make pkt_slot and add it to end of queue
  struct pkt_slot* new_packet_slot = (struct pkt_slot*)malloc(sizeof(struct pkt_slot));
  new_packet_slot->packet = new_packet;
  new_packet_slot->next = NULL;
  if (pkt_queue == NULL) {
    if ( TraceLevel >= 2 ) printf("  A_output: Creating new Queue head\n");
    pkt_queue = new_packet_slot;
  }
  else {
    if ( TraceLevel >= 2 ) printf("  A_output: Adding new message to Queue\n");
    last_pkt_in_queue->next = new_packet_slot;
    if ( TraceLevel >= 2 ) printf("  A_output: Added new message to Queue successfully\n");
  }
  last_pkt_in_queue = new_packet_slot;
  pkt_queue_size++;
  if ( TraceLevel >= 2 ) printf("  A_output: Sending new Packet \n");
  if ( TraceLevel >= 2 ) printf("  A_output: pakcet queue contains: %i packets\n", pkt_queue_size);
  if ( TraceLevel >= 2 ) print_queue();
  tolayer3(AEntity, new_packet);
  current_seqnum_A += 20;
  startTimer(AEntity, INTERRUPT_INTERVAL);
  waiting_for_ack_A = TRUE;
}

/*
 * Just like A_output, but residing on the B side.  USED only when the 
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {
  // Nothing
}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
  if (!waiting_for_ack_A) {
    if ( TraceLevel >= 2 )  printf("  A_input: not expecting a packet. drop.\n");
    return;
  }

  if (packet.checksum != get_checksum(&packet)) {
    if ( TraceLevel >= 2 )  printf("  A_input: ACK corrupted. drop.\n");
    resend_packet();
    return;
  }

  if ( TraceLevel >= 2 ) printf("  A_input: acked.\n");
  stopTimer(AEntity);
  acked_seqnum_A = packet.acknum;
  update_queue();

  if (pkt_queue_size == 0) {
    waiting_for_ack_A = FALSE;
  }
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
  if (!waiting_for_ack_A) {
    if ( TraceLevel >= 2 )  printf("  A_timerinterrupt: not waiting ACK. ignore event.\n");
    return;
  }
  else {
    if ( TraceLevel >= 2 )  printf("  A_timerinterrupt: resend pakcet queue.\n");
    resend_packet();
    return;
  }
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
  // Set sender states
  waiting_for_ack_A = FALSE;
  current_seqnum_A = 0;
  acked_seqnum_A = 0;
  pkt_queue = NULL;
  pkt_queue_size = 0;
}

/* 
 * Note that with simplex transfer from A-to-B, there is no routine  B_output() 
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
  if (packet.seqnum != current_seqnum_B) {
    if ( TraceLevel >= 2 )  printf("  B_input: got seqnum %i, expected %i. drop.\n", packet.seqnum, current_seqnum_B);
    // Drop it and wait for timeout, when packets will be resent in correct order
    return;
  }
  if (packet.checksum != get_checksum(&packet)) {
    if ( TraceLevel >= 2 )  printf("  B_input: packet with seqnum %i corrupted. drop.\n", packet.seqnum);
    // Drop it and wait for timeout, when corrupted packets will be resent
    return;
  }
  if ( TraceLevel >= 2 )  printf("  B_input: recv message: %s\n", packet.payload);
  if ( TraceLevel >= 2 )  printf("  B_input: sending ACK.\n");
  // Update seqnum
  current_seqnum_B += 20;
  send_ack(BEntity, current_seqnum_B); // another 20 bytes were received
  // Send message to layer 5
  struct msg new_message;
  memcpy(new_message.data, packet.payload, 20);
  tolayer5(BEntity, new_message);
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
  // Nothing, not bidirectional
}

/* 
 * The following routine will be called once (only) before any other   
 * entity B routines are called. You can use it to do any initialization 
 */
void B_init() {
  // Set sender states
  current_seqnum_B = 0;
}