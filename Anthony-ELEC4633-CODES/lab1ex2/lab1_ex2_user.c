#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>
#include <rtai_fifos.h>

int main(void)
{
  /* Add code */
  int *udata;
  int count;
  int FIFOcount;
  udata = rtai_malloc(nam2num("count"), sizeof(int));
  // Open pipe as read only - return error message if failed
  if((FIFOcount = open("/dev/rtf0", O_RDONLY))<0){
    printf("Error opening FIFO pipe");
  }

  while(){
    usleep(1)                                 // sleep for 1us because of context switching
    count = *udata
    printf("\nShared Memory: %d", count);
    read(FIFOcount, &count, sizeof(count));   // Read pipe and save value into 'count' with sizeof 'count'
    printf("\tFIFO buffer: %d", count);
  }
  rtai_free(nam2num("counter"));
  return 0;
}


