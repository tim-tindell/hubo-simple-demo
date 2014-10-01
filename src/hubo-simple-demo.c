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

/* Move the hips to move the center of gravity over right foot*/
    double amount=0;
    while(amount<0.2){
       H_ref.ref[LHR] = amount;
       H_ref.ref[RHR] = amount;
       H_ref.ref[LAR] = -amount;
       H_ref.ref[RAR] = -amount;

       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(1000000);
       amount+=0.025;
    }
    sleep(5);
    printf("Finished Hip Movement to right leg\n");
    

/*Move the left leg into the air*/
    amount=0;
    while(amount<0.8){
       H_ref.ref[LHP] = -amount;
       H_ref.ref[LKN] = 2*amount;
       H_ref.ref[LAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(2500000);
       amount+=0.025;
    }
    sleep(15);
    printf("Finished Putting left Leg in the Air\n");
    ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));

/*bend right knee and ankle sinusoidally twice*/
    int x=0;
    amount=0.05;
    int count=0;
    while(count!=2){
       /* Get the current feed-forward (state) */
       r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
       if(ACH_OK != r) {
           assert( sizeof(H_state) == fs );
       }
       double before_move=H_state.time;
       //Go Down
       if(x==0){
          H_ref.ref[RHP] = -amount;
          H_ref.ref[RKN] = 2*amount;
          H_ref.ref[RAP] = -amount;
          if(amount>=0.75){
	      x=1;
      	      printf("Went Down\r\n");
         }
	 else{
            amount=amount+0.05;
	 }
      }
      //Go  Up
      else{
          H_ref.ref[RHP] =-amount;
          H_ref.ref[RKN] = 2*amount;
          H_ref.ref[RAP] = -amount;
  	  if(amount<=0.05){
              x=0;
	      count++;
              printf("Went Up\r\n");
          }
	  else{
             amount=amount-0.05;
	  }
      }
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
             /* Get the current feed-forward (state) */
       r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
       if(ACH_OK != r) {
           assert( sizeof(H_state) == fs );
       }
      double runtime=H_state.time-before_move;
      wsleep(0.0625,runtime,H_state);
    
   }
   printf("Right leg got its work out\n");
   usleep(1000000);

/* Put leg back down*/
   amount=0.8;
   while(amount>0){
       H_ref.ref[LHP] = -amount;
       H_ref.ref[LKN] = 2*amount;
       H_ref.ref[LAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(2500000);
       amount-=0.025;
    }
   printf("left leg is back down\n");

/* Move the hips to move the center of gravity over left foot*/ 
    amount=0.2;
    while(amount>-0.2){
       H_ref.ref[LHR] = amount;
       H_ref.ref[RHR] = amount;
       H_ref.ref[LAR] = -amount;
       H_ref.ref[RAR] = -amount;

       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(1000000);
       amount-=0.025;
    }
    sleep(5);
    printf("Finished Hip Movement to left leg\n");

/*Move the right leg into the air*/
    amount=0;
    while(amount<0.5){
       H_ref.ref[RHP] = -amount;
       H_ref.ref[RKN] = 2*amount;
       H_ref.ref[RAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(2500000);
       amount+=0.025;
    }
    sleep(15);
    printf("Finished Putting right Leg in the Air\n");
    ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));

/*bend left knee and ankle sinusoidally*/
    x=0;
    amount=0.05;
    count=0;
    while(count!=2){
       /* Get the current feed-forward (state) */
       r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
       if(ACH_OK != r) {
           assert( sizeof(H_state) == fs );
       }
       double before_move=H_state.time;
       //Go Down
       if(x==0){
          H_ref.ref[LHP] = -amount;
          H_ref.ref[LKN] = 2*amount;
          H_ref.ref[LAP] = -amount;
          if(amount>=0.4){
	      x=1;
      	      printf("Went Down\r\n");
         }
	 else{
            amount=amount+0.025;
	 }
      }
      //Go  Up
      else{
          H_ref.ref[LHP] =-amount;
          H_ref.ref[LKN] = 2*amount;
          H_ref.ref[LAP] = -amount;
  	  if(amount<=0.05){
              x=0;
	      count++;
              printf("Went Up\r\n");
          }
	  else{
             amount=amount-0.025;
	  }
      }
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
             /* Get the current feed-forward (state) */
       r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
       if(ACH_OK != r) {
           assert( sizeof(H_state) == fs );
       }
      double runtime=H_state.time-before_move;
      wsleep(0.0625,runtime,H_state);
    
   }
   printf("Right leg got its work out\n");
   usleep(1000000);

/* Put leg back down*/
   amount=0.5;
   while(amount>0){
       H_ref.ref[RHP] = -amount;
       H_ref.ref[RKN] = 2*amount;
       H_ref.ref[RAP] = -amount;
       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(2500000);
       amount-=0.025;
    }
   printf("left leg is back down\n");

/*Move hips to 0*/
    amount=-0.2;
    while(amount<0){
       H_ref.ref[LHR] = amount;
       H_ref.ref[RHR] = amount;
       H_ref.ref[LAR] = -amount;
       H_ref.ref[RAR] = -amount;

       /* Write to the feed-forward channel */
       ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
       usleep(1000000);
       amount+=0.025;
    }
    printf("Ah I am done!! Great leg work out\n");
}
