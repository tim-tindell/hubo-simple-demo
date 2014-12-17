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
       r = ach_get( &chan_hubo_state, &Hs, sizeof(Hs), &fs, NULL, ACH_O_WAIT);
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
 /* set the Left Shoulder to face up and the should to make hand face forward*/
    H_ref.ref[LSP] = -3.14;
    H_ref.ref[LSY] = -1.5;
    ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
    sleep(4);
    /* for size check */
    size_t fs;
    int x=0;
    int y=0;
    double amount=0;
    int phase=0;
    int count=0;
    int wave=1;
    while(1){
      ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_WAIT );
      if (x==0&&wave==1){//Wave left
        H_ref.ref[LSR]=0.2;
        H_ref.ref[LWP]=0.4;
        x=1;
      }
      else if(wave==1){//Wave right
        H_ref.ref[LSR]=-0.2;
        H_ref.ref[LWP]=-0.4;
        x=0;
      }
      else{//Put hand back in center
        H_ref.ref[LSR]=0;
        H_ref.ref[LWP]=0;
      }//move center of gravity
      if(phase==0){
         H_ref.ref[LHR] = amount;
         H_ref.ref[RHR] = amount;
         H_ref.ref[LAR] = -amount;
         H_ref.ref[RAR] = -amount;
         amount+=0.05;
         if(amount>0.24){
           phase=1;
           amount=0;
         }
      }//Lift left leg
      else if(phase==1){
         H_ref.ref[LHP] = -amount;
         H_ref.ref[LKN] = 2*amount;
         H_ref.ref[LAP] = -amount;
         amount+=0.05;
         if(amount>0.7){
           phase=2;
           amount=0;
         }
       }//Go down
       else if(phase==2){
          H_ref.ref[RHP] = -amount;
          H_ref.ref[RKN] = 2*amount;
          H_ref.ref[RAP] = -amount;
         amount+=0.1;
         if(amount>=0.6){
           phase=3;
         }
       }//Go Up
       else if(phase==3){
          H_ref.ref[RHP] =-amount;
          H_ref.ref[RKN] = 2*amount;
          H_ref.ref[RAP] = -amount; 
         amount-=0.1;
         if(amount<=0.05){
           phase=2;
           count++;
           if(count==3){
             phase=4;
             amount=0.7;
           }
         }
       }//put left leg down
       else if(phase==4){
         H_ref.ref[LHP] = -amount;
         H_ref.ref[LKN] = 2*amount;
         H_ref.ref[LAP] = -amount;
         amount-=0.05;
         if(amount<=0.05){
           phase=5;
           amount=0.24;
         }
       }//stand straight
       else if(phase==5){
         H_ref.ref[LHR] = amount;
         H_ref.ref[RHR] = amount;
         H_ref.ref[LAR] = -amount;
         H_ref.ref[RAR] = -amount;
         amount-=0.1;
         if(amount<=0.00){
           phase=6;
           wave=0;
         }
      }
      else if(phase==6){//One more stage for waving to stop
      phase=7;
      }
      else if(phase==7){//sleep for a little
         wsleep(3,0,H_state);
         phase=8;
       }//put arm down
       else if(phase==8){
         
         H_ref.ref[LSP] = 0;
         H_ref.ref[LSY] = 0;
         phase=9;
      }
      else if(phase==9)//We have finished return
         return 1;
      ach_put( &chan_hubo_ref, &H_ref, sizeof(H_ref));
      wsleep(.25,0,H_state);//frequency of 25  Hertz
    
}
}


