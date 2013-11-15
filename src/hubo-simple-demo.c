/* Standard Stuff */
#include <string.h>
#include <stdio.h>

/* Required Hubo Headers */
#include <hubo.h>
#include <control-daemon.h>

/* For Ach IPC */
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <inttypes.h>
#include "ach.h"


/* Ach Channel IDs */
ach_channel_t chan_hubo_ll;      // left leg
ach_channel_t chan_hubo_rl;    // right leg

int main(int argc, char **argv) {

    /* Open Ach Channel */
    int r = ach_open(&chan_hubo_ll, HUBO_CHAN_LL_CTRL_NAME , NULL);
    assert( ACH_OK == r );

    r = ach_open(&chan_hubo_rl, HUBO_CHAN_RL_CTRL_NAME , NULL);
    assert( ACH_OK == r );



    /* Create initial structures to read and write from */
    struct hubo_leg_control H_ll;
    struct hubo_leg_control H_rl;
    memset( &H_ll,   0, sizeof(H_ll));
    memset( &H_rl, 0, sizeof(H_rl));

    /* for size check */
    size_t fs;

  while(1) {
    /* Get the current legs */
    r = ach_get( &chan_hubo_ll, &H_ll, sizeof(H_ll), &fs, NULL, ACH_O_LAST );
    if(ACH_OK != r) {
        assert( sizeof(H_ll) == fs );
    }
    r = ach_get( &chan_hubo_rl, &H_rl, sizeof(H_rl), &fs, NULL, ACH_O_LAST );
    if(ACH_OK != r) {
        assert( sizeof(H_rl) == fs );
    }
    

    /* clear screen */
    printf("\033[2J");

    printf("---- left leg ----");
    int i = 0;
    for( i = 0; i < LEG_JOINT_COUNT; i++) {
        printf(" %f\r\n", H_ll.joint[i].position
    }

    printf("---- right leg ----");
    for( i = 0; i < LEG_JOINT_COUNT; i++) {
        printf(" %f\r\n", H_rl.joint[i].position
    }
    usleep(10000);

  }
}

