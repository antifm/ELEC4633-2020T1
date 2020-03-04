#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>


int main(void)
{
  /* Add code */
  int *udata;
  int count;
  udata = rtai_malloc(nam2num("count"), sizeof(int));
  while(){
    count = *udata
    printf("\n %d", count);
  }
  rtai_free(nam2num("counter"));
  return 0;
}


