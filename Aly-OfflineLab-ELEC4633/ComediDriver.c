// ELEC4633 Lab02 - Comedi

#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_nam2num.h>

#include <linux/comedi.h>
#include <linux/comedilib.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()

// Set the periods so they run at 10 Hz
// ********** EDITED ****************//
// Changed to 0.1 Seconds f = 10Hz
#define PERIOD_1 nano2count(0.1e9)
#define PERIOD_2 nano2count(0.1e9)
//**********************************//
//Shared memory locations
#define READ_INPUT 800
#define WRITE_OUTPUT 900

// ADC/DAC related vriables
#define READ_SUBDEVICE 0
#define WRITE_SUBDEVICE 1
#define READ_CHANNEL 0
#define WRITE_CHANNEL 0
#define RANGE 0
#define AREF AREF_GROUND

#define READLENGTH 100
#define WRITELENGTH 100

/* Store data needed for the thread */
RT_TASK threadRead; // thread which reads motor position/speed 
RT_TASK threadWrite; // thread which send out control value to motor

/* Data needed by comedi */
comedi_t *comedi_dev;

int motorRead;
int motorWrite;
volatile int *readIndexMR;
volatile int *writeIndexMR;

int *motorReadBuffer;
int *motorWriteBuffer;
double Kp=0.5; //propotional gain
int *setPoint; //position setpoint

int motorReadBuffer_len = READLENGTH;
int motorWriteBuffer_len = WRITELENGTH;

/* The code that is run */
void rtMotorRead(long arg)
{
    *readIndexMR = 0;  
    *writeIndexMR = 0; 
  while (1)
  {

    comedi_data_read(comedi_dev, READ_SUBDEVICE, READ_CHANNEL, RANGE, AREF, &motorRead);

    //*************** EDITED ****************//
        //Obtain the current motor position and write it into the read buffer.*/
        motorReadBuffer[*writeIndexMR] = motorRead;
        (*writeIndexMR)++;


        
        //Circular buffer
        // If write index is greater than buffer len then bring back to 0
        if(*writeIndexMR == motorWriteBuffer_len) { 
          *writeIndexMR = 0; 
        }

        //If write index is still within the buffer length
        if(*writeIndexMR == *readIndexMR)  {  
          
          (*readIndexMR)++;
        
          if(*readIndexMR == motorReadBuffer_len) {
            *readIndexMR = 0;  
          }
        }
    //***********************************//

        printk("Read Value = %d\n", motorReadBuffer[*writeIndexMR]);
        printk("Buffer index %d \n",*writeIndexMR);// check whether data is properly read
    rt_task_wait_period();
  }
}


void rtMotorWrite(long arg)
{
    while (1) 
  {
  //*************** EDITED ****************//
      /* Motor has to reach the setpoint from its initial position through the shortest path - refer to lab instructions for more details*/
	  if ((*setPoint - motorRead) > 2048) {

      motorWrite = Kp*((*setPoint - motorRead) - 2048);

    } else {
      
      motorWrite = Kp*((*setPoint - motorRead) + 2048);
         
    }

    comedi_data_write(comedi_dev, WRITE_SUBDEVICE, WRITE_CHANNEL, RANGE, AREF, motorWrite);
    
  //**************************************//

    printk("Write Value = %d\n", motorWrite); // check whether data is properly read
    rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD_1);
  start_rt_timer(PERIOD_2);

  // Open Hardware
  comedi_dev = comedi_open("/dev/comedi0");

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&threadRead, rtMotorRead, ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_init(&threadWrite, rtMotorWrite, ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  
  // ***** EDITED ***** //
  // Shared memory allocation to the variables
  readIndexMR = rtai_malloc(nam2num("mem_readIndexMR"), sizeof(int));
  writeIndexMR = rtai_malloc(nam2num("mem_writeIndexMR_c"), sizeof(int));
  setpoint = rtai_malloc(nam2num("mem_setpoint"), sizeof(int));
  motorReadBuffer = rtai_malloc(nam2num("mem_motorReadBuffer_c"), sizeof(int));
  // ***************** //

  rt_task_make_periodic(&threadRead, NOW, PERIOD_1);
  rt_task_make_periodic(&threadWrite, NOW, PERIOD_2);
  
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&threadRead);
  rt_task_delete(&threadWrite);

  // ***** EDITED ***** //
  // Free Shared Memory
  rtai_kfree(nam2num("mem_readIndexMR"));
  rtai_kfree(nam2num("mem_writeIndexMR_c"));
  rtai_kfree(nam2num("mem_setpoint"));
  rtai_kfree(nam2num("mem_motorReadBuffer_c"));
  // ***************** //
  
  comedi_close(comedi_dev);
}

module_init(template_init);
module_exit(template_exit);
