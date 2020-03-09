#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_shm.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(1e9)

#define FIFO 0 

// Store global data
int counter = 0;
int *kdata;           // Pointer to the shared memory - declared in 'init'

/* Store data needed for the thread */
RT_TASK increment;
RT_TASK display;

/* The code that is run */
int increment_code()
{
  while (1)
  {
    /* Add code here */
    counter = counter + 1;
    rt_task_wait_period();
    // something
  }
}

int display_code()
{
  while (1)
  {
    /* Add code here */
    //printk("\n %d",counter);
    *kdata = counter;
    rtf_put(FIFO,&counter,sizeof(counter));
	  rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);
  
  // Creating FIFO memory
  rtf_create(FIFO, sizeof(int));
  rtf_reset(FIFO);

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&increment, increment_code,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&increment, NOW, PERIOD);

    rt_task_init(&display, display_code,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&display, NOW, 3*PERIOD);

  /* Allocating memory in RTAI space*/
  kdata = rtai_kmalloc(nam2num("count"),sizeof(int));



  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&increment);
  rt_task_delete(&display);
  rtai_kfree(nam2num("counter"));
  rtf_destroy(FIFO);
}

module_init(template_init);
module_exit(template_exit);

