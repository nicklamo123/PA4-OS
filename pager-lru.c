/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) {

    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0;
	    }
	}
	initialized = 1;
    }

    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++){

      if (q[proctmp].active) { // if this process is active
        pagetmp = q[proctmp].pc / PAGESIZE; // get page that we are going to need

        if (!q[proctmp].pages[pagetmp]){ // if page isn't loaded into memory attempt to load page into memory using pagein
          if (!pagein(proctmp, pagetmp)){

            //if we make it here it means that we haven't been able to load page into memory
            int oldest = -1;
            int oldest_page = 0;
            //we need to find which page we want to load out of main memory
            //we find the correct page by finding the page that hasn't been used in the longest time. aka lowest time in timestamps
            for (int i = 0; i < q[proctmp].npages; i++){
              int ticktime = tick - timestamps[proctmp][i];
              if (oldest < ticktime && q[proctmp].pages[i]) { // must be paged in
                oldest = ticktime;
                oldest_page = i;
              }
            }
            //now we pageout this page
            pageout(proctmp, oldest_page);
          }
        }
        //update process that has been paged in to current tick
        timestamps[proctmp][pagetmp] = tick;
        tick++;
      }
    }
}
