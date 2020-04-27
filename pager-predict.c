/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"

// this struct will be assigned to every process to keep track of it's pages
struct proc_data {
  int prev_page;
  int counts[MAXPROCPAGES][MAXPROCPAGES];
  double state_matrix[MAXPROCPAGES][MAXPROCPAGES];

};

void pageit(Pentry q[MAXPROCESSES]) {

    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static struct proc_data procs[MAXPROCESSES];
    static int initialized = 0;
    static int tick = 1;

    /* Local vars */
    int proctmp;
    int pagetmp;
    int i;
    int j;


    /* initialize static vars on first run */
    if(!initialized){
	     /* Init complex static vars here */
       // all the process data used for predictions
       // make all counts where i+1 = j to 1 to help mitigate page misses in the beginning
       // everything else is 0
       for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++){

         for (i = 0; i < MAXPROCPAGES; i ++){
           for (j = 0; j < MAXPROCPAGES; j++){
             if (i+1 == j) {
               procs[proctmp].counts[i][j] = 1;
             }
             else {
               procs[proctmp].counts[i][j] = 0;
             }
             procs[proctmp].state_matrix[i][j] = 0;
           }
         }
         procs[proctmp].prev_page = -1;
       }

	     initialized = 1;
    }


    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
      if (q[proctmp].active) {
        // current page that we are on. mandatory to page in
        pagetmp = q[proctmp].pc / PAGESIZE;

        // update counts matrix
        if (procs[proctmp].prev_page != -1 && procs[proctmp].prev_page != pagetmp){
          procs[proctmp].counts[ procs[proctmp].prev_page ][pagetmp]++;
        }
        procs[proctmp].prev_page = pagetmp;

        //make sums array
        int sum = 0;
        for (j=0; j< MAXPROCPAGES; j++) {
          sum += procs[proctmp].counts[pagetmp][j];
        }

        //update state matrix based on counts
          if (sum > 0) {
            for (j=0; j< MAXPROCPAGES; j++) {
                procs[proctmp].state_matrix[pagetmp][j] = (double)procs[proctmp].counts[pagetmp][j] / sum;
            }
          }
          else {
            for (j=0; j< MAXPROCPAGES; j++) {
              procs[proctmp].state_matrix[pagetmp][j] = 0;
            }
          }


          //gen will_pagein array from state matrix, this determines what pages to page in next
          int will_pagein[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

          for (i = 0; i < MAXPROCPAGES; i++) {
            if (procs[proctmp].state_matrix[pagetmp][i] > 0) {
              will_pagein[i] = 1;
            }
          }

          will_pagein[pagetmp] = 1;
          for (i = 0; i < MAXPROCPAGES; i++) {
            if (procs[proctmp].state_matrix[pagetmp][i] > 0){
              for (j = 0; j < MAXPROCPAGES; j++) {
                if (procs[proctmp].state_matrix[i][j] > 0){
                  will_pagein[j] = 1;
                }
              }
            }
          }

          //pagin and out pages based on prediction from state matrix
          for (i = 0; i < MAXPROCPAGES; i++) {
            if (!will_pagein[i]){
              pageout(proctmp, i);
            }
          }
          for (i = 0; i < MAXPROCPAGES; i++) {
            if (will_pagein[i]){
              pagein(proctmp, i);
            }
          }

      }

    }
    if ( tick == 100000 ) {
    for (int k = 0; k < 20; k++){
      printf("Process %d \n", k);
      for (i = 0; i < 20; i++ ) {
        for (j = 0; j < 20; j++ ) {
          printf("%.2f ", procs[k].state_matrix[i][j]);
        }
        printf("\n");
      }
      printf("\n");
      for (i = 0; i < 20; i++ ) {
        for (j = 0; j < 20; j++ ) {
          printf("%d ", procs[k].counts[i][j]);
        }
        printf("\n");
      }
       printf("\n\n\n");
     }
    }


    tick++;
  }
