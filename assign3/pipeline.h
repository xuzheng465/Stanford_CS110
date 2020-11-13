/**
 * File: pipeline.h
 * ----------------
 * Exports the pipeline routine, which launches
 * two sister executables such that the standout
 * output of the first is routed to the standard
 * input of the second.  Check out the following
 * test framework to see how pipeline should work:

     int main(int argc, char *argv[]) {
       char *argv1[] = {"cat", "pipeline-test.c", NULL};
       char *argv2[] = {"wc", NULL};
       pid_t pids[2];
       pipeline(argv1, argv2, pids);
       waitpid(pids[0], NULL, 0);
       waitpid(pids[1], NULL, 0);
       return 0;
     }

 *
 */

#ifndef _pipeline_h_
#define _pipeline_h_

#include <unistd.h>

/**
 * Function: pipeline
 * ------------------
 * Spawns off sister processes, the first around the argument
 * vector supplied via argv1, the second around the argument
 * vector supplied via argv2, and places the process ids of
 * each in pids[0] and pids[1].  Furthermore, the standard
 * output of the first process is piped to the standard input
 * of the second process.
 */

void pipeline(char *argv1[], char *argv2[], pid_t pids[]);

#endif
