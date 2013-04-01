/* Standard Stuff */
#include <string.h>
#include <stdio.h>

/* Required Hubo Headers */
#include <hubo.h>

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
ach_channel_t chan_hubo_ref;      // Feed-Forward (Reference)
ach_channel_t chan_hubo_state;    // Feed-Back (State)
ach_channel_t chan_hubo_to_sim;    // To Sim
ach_channel_t chan_hubo_from_sim;    // From Sim


int main(int argc, char **argv) {
    
    int i = 0;

    /* Open Ach Channel */
    int r = ach_open(&chan_hubo_ref, HUBO_CHAN_REF_NAME , NULL);
    assert( ACH_OK == r );

    r = ach_open(&chan_hubo_state, HUBO_CHAN_STATE_NAME , NULL);
    assert( ACH_OK == r );



    /* Create initial structures to read and write from */
    struct hubo_ref H_ref;
    struct hubo_ref H_ref_your_sim;  // this is a place holder for what ever way your store your sim ref data
    struct hubo_state H_state_your_sim; // this is a place holder for what ever way your store your sim state data
    struct hubo_state H_state;
    struct hubo_virtual H_sim;
    memset( &H_ref,   0, sizeof(H_ref));
    memset( &H_ref,   0, sizeof(H_ref_your_sim));
    memset( &H_state, 0, sizeof(H_state));
    memset( &H_state, 0, sizeof(H_state_your_sim));
    memset( &H_sim, 0, sizeof(H_sim));

    /* for size check */
    size_t fs;

    /* Flush old messages */
    ach_flush(&chan_hubo_to_sim);
    ach_flush(&chan_hubo_from_sim);


    /* send the from sim trigger */
    ach_put( &chan_hubo_from_sim, &H_sim, sizeof(H_sim));

    /* Start the sim time loop */
    while(1) {
        /* Waits for hubo-ach trigger */
        r = ach_get( &chan_hubo_to_sim, &H_sim, sizeof(H_sim), &fs, NULL, ACH_O_WAIT );
        if(ACH_OK != r) {
            assert( sizeof(H_sim) == fs );
        }

        /* Get the current feed-forward (state) */
        r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
        if(ACH_OK != r) {
            assert( sizeof(H_state) == fs );
        }

        
        /* Sets the commanded joint value to your simulators feed forward */
        for( i = 0; i < HUBO_JOINT_COUNT; i++){
	    H_ref_your_sim.ref[i] = H_state.joint[i].ref;		
        }


       /* ----------------------------- */
       /* run your simulator stuff here */
       /* ----------------------------- */
       /* Note: you can run multiple itterations of your sim here before you post the data from the sim */
       /*       i.e. if you want to run your sim at 1khz then step your sim N times */
       /*       where N = ceil(1000/HUBO_LOOP_PERIOD)   */
       /*       HUBO_LOOP_PERIOD is defigned in hubo.h  */


       /* Note2: Hubo-Ach updates the time via the H_sim.time. */
       /* Set time to simtime */
       /* H_sim.time = Time from your simulation in seconds*/

        /* set your state data to the hubo-ach state data */
        /* Joint pos*/
        for( i = 0; i < HUBO_JOINT_COUNT; i++){
            H_state.joint[i].pos = H_state_your_sim.joint[i].pos;  // actuial joint position from simulator
        }

        /* FT */
        for( i = 0; i < 4; i++){        // force torque from sim  note: 4 will soon be changed to HUBO_FT_COUNT
            H_state.ft[i].m_x = H_state_your_sim.ft[i].m_x;
            H_state.ft[i].m_y = H_state_your_sim.ft[i].m_y;
            H_state.ft[i].f_z = H_state_your_sim.ft[i].f_z;
        }


        for( i = 0; i < HUBO_IMU_COUNT; i++){        // IMU Data from your sim
            H_state.imu[i].a_x = H_state_your_sim.imu[i].a_x;
            H_state.imu[i].a_y = H_state_your_sim.imu[i].a_y;
            H_state.imu[i].a_z = H_state_your_sim.imu[i].a_z;
            H_state.imu[i].w_x = H_state_your_sim.imu[i].w_x;
            H_state.imu[i].w_y = H_state_your_sim.imu[i].w_y;
            H_state.imu[i].w_z = H_state_your_sim.imu[i].w_z;
       }


    /* at this point hubo-ach has been waiting for the sim to be done */
    /* now that all of the sim data has been set to the state you can */
    /* tell hubo-ach that it can resume.  This is done by giving it the */
    /* from_sim trigger */

        /* send the from sim trigger */
        ach_put( &chan_hubo_from_sim, &H_sim, sizeof(H_sim));

    }

}

