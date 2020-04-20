// ELEC4633 Lab02 - Linux Server Program


#include <stdio.h>
#include <sys/msg.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

#define READLENGTH 100
#define WRITELENGTH 100

int *readIndexMR_c;
int *writeIndexMR_c;
int *setpoint;

int *motorReadBuffer_c;

struct mymsgbuf {
    long mtype;
    int setpoint;
};

struct mymsgbuf rmsg.setpoint;

int motorReadBuffer_len = READLENGTH;
int motorWriteBuffer_len = WRITELENGTH;

int main(void)
{
    int status,i;
    int rqueue;
    key_t msgkey;
    
    // generate the message key to obtain a queue id - same as in send program
    msgkey = ftok(".",'m');

  // obtain a queue id - should be same id as in send program
    rqueue = msgget(msgkey, IPC_CREAT | 0660);
    
 	// ***** EDITED ***** //
  // Shared Memory Variables allocated 
  readIndexMR_c = rtai_malloc(nam2num("mem_readIndexMR"), sizeof(int));
  writeIndexMR_c = rtai_malloc(nam2num("mem_writeIndexMR_c"), sizeof(int));
  setpoint = rtai_malloc(nam2num("mem_setpoint"), sizeof(int));
  motorReadBuffer_c = rtai_malloc(nam2num("mem_motorReadBuffer_c"), sizeof(int));
  // ***************** //

 
    while(1)  {
    
    // ***** EDITED ***** //
    int id;

    for(id=0;id<50;id++)  {
      // 
        if(*readIndexMR_c == *writeIndexMR_c) {
          break;
        }
        printf("value at index %d is %d \n",*readIndexMR_c,motorReadBuffer_c[*readIndexMR_c]);
        (*readIndexMR_c)++;
        if(*readIndexMR_c == motorReadBuffer_len) {   
          *readIndexMR_c=0; 
        }
     }
     
     status = msgrcv(rqueue, &setpoint, sizeof(rmsg01.setpoint), 0, 0);
     if(rmsg01.mtype==1)  {
      *setpoint = rmsg01.setpoint;
     }
     usleep(5000000);
    }
    // ***************** //
    
	// ***** EDITED ***** //
  // Free Shared Memory 
  rtai_free(nam2num("mem_readIndexMR"),&readIndexMR);
  rtai_free(nam2num("mem_writeIndexMR_c"),&writeIndexMR_c);
  rtai_free(nam2num("mem_setpoint"),&setpoint);
  rtai_free(nam2num("mem_motorReadBuffer_c"),&motorReadBuffer_c);
  // ***************** //
	
  return 0;
}
