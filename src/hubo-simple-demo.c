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

//Sleep with respect to Sim Time
void wsleep(double ts,double runtime,struct hubo_state Hs){
   double t;
   t=ts-runtime;
   size_t fs;
   //Get Current Sim Time
   int r = ach_get( &chan_hubo_state, &Hs, sizeof(Hs), &fs, NULL, ACH_O_WAIT );
       if(ACH_OK != r) {
          assert( sizeof(Hs) == fs );
       }
   t+=Hs.time;
   while(Hs.time<t){
      
       /* Get the current feed-forward (state) */
       r = ach_get( &chan_hubo_state, &Hs, sizeof(Hs), &fs, NULL, ACH_O_WAIT );
       if(ACH_OK != r) {
          assert( sizeof(Hs) == fs );
       }
       //printf("%f<%f\r\n",Hs.time,t);
}
return;
}
/*=======================Begin main function=======================*/
int main(int argc, char **argv) {
    /* Open Ach Channel */
    int r = ach_open(&chan_hubo_ref, HUBO_CHAN_REF_NAME , NULL);
    assert( ACH_OK == r );

    r = ach_open(&chan_hubo_state, HUBO_CHAN_STATE_NAME , NULL);
    assert( ACH_OK == r );

    /* Create initial structures to read and write from */
    struct hubo_ref H_ref;
    struct hubo_state H_state;
    memset( &H_ref,   0, sizeof(H_ref));
    memset( &H_state, 0, sizeof(H_state));

    /* for size check */
    size_t fs;

/*<PHASE 1> Bend your knees first!!!*/
    double amount=0;
    while(amount<=0.42){
       H_ref.ref[LHP] = -amount;
       H_ref.ref[LKN] = 2*amount;
       H_ref.ref[LAP] = -amount;

       H_ref.ref[RHP] = -amount;
       H_ref.ref[RKN] = 2*amount;
       H_ref.ref[RAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.1,0,H_state);
       amount+=0.025;
    }
    printf("Finished bending knees\n");
   // printf("LHP: %f \nLKN: %f \nLAP: %f \n",H_ref.ref[LHP],H_ref.ref[LKN],H_ref.ref[LAP]);
    wsleep(2,0,H_state);
int count=0;
amount=0;
while(1){

/*<PHASE 2> Move the hips to move the center of gravity over right foot*/
    while(amount<=0.22){
       H_ref.ref[LHR] = amount;
       H_ref.ref[RHR] = amount;
       H_ref.ref[LAR] = -amount;
       H_ref.ref[RAR] = -amount;

       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.1,0,H_state);
       amount+=0.025;
    }
    wsleep(2,0,H_state);
    printf("Finished Hip Movement to right leg\n");

/*<PHASE 3> Move the left leg into the air*/
    amount=0.4;

    while(amount<=0.52){
       H_ref.ref[LHP] = -amount;
       H_ref.ref[LKN] = 2*amount;
       H_ref.ref[LAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.2,0,H_state);
       amount+=0.025;
    }
    
    wsleep(2,0,H_state);
    printf("Finished Putting left Leg in the Air\n");
    printf("LHP: %f \nLKN: %f \nLAP: %f \n",H_ref.ref[LHP],H_ref.ref[LKN],H_ref.ref[LAP]);
    if(count>0){
    	while(amount>=0.42){
           H_ref.ref[RHP] = -amount;
		/* Write to the feed-forward channel */
	  ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
          wsleep(0.2,0,H_state);
          amount-=0.025;
        }
   }
   amount=0.5;
   double ankle=amount;

/*<PHASE 4> Put the left leg down more forward then before*/
    while(amount>=0.42){

       H_ref.ref[LKN] = 2*amount;
       H_ref.ref[LAP] = -ankle;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.3,0,H_state);
       amount-=0.025;
       ankle-=0.025;
    }
    wsleep(2,0,H_state);

 
    printf("Finished Putting left Leg back down\n");
   // printf("LHP: %f \nLKN: %f \nLAP: %f \n",H_ref.ref[LHP],H_ref.ref[LKN],H_ref.ref[LAP]);

/*<PHASE 5> Move the hips to move the center of gravity over left foot*/
    amount=0.2;
    while(amount>=-0.22){
       H_ref.ref[LHR] = amount;
       H_ref.ref[RHR] = amount;
       H_ref.ref[LAR] = -amount;
       H_ref.ref[RAR] = -amount;

       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.1,0,H_state);
       amount-=0.025;
    }
    wsleep(2,0,H_state);
    printf("Finished Hip Movement to left leg\n");

/*<PHASE 6> Move the right leg into the air*/
    amount=0.4;
    while(amount<=0.52){
       
       H_ref.ref[RHP] = -amount;
       H_ref.ref[RKN] = 2*amount;
       H_ref.ref[RAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.2,0,H_state);
       amount+=0.025;
    }
    wsleep(2,0,H_state);
    printf("Finished Putting right Leg in the Air\n");
    printf("RHP: %f \nRKN: %f \nRAP: %f \n",H_ref.ref[RHP],H_ref.ref[RKN],H_ref.ref[RAP]);

    while(amount>=0.42){
       H_ref.ref[LHP] = -amount;
	/* Write to the feed-forward channel */
      ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
      wsleep(0.2,0,H_state);
      amount-=0.025;
    }
   amount=0.5;
   ankle=amount;
/*<PHASE 7> Put the right leg down more forward then before*/
    while(amount>=0.42){
       H_ref.ref[RKN] = 2*amount;
       H_ref.ref[RAP] = -ankle;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       wsleep(0.2,0,H_state);
       amount-=0.025;
       ankle-=0.025;
    }
    wsleep(2,0,H_state);
   amount=-0.2;
   
    printf("Finished Putting left Leg back down\n");
    printf("RHP: %f \nRKN: %f \nRAP: %f \n",H_ref.ref[RHP],H_ref.ref[RKN],H_ref.ref[RAP]);
count++;
	printf("\nTook %d steps!\n\n",2*count);
}
    
}


