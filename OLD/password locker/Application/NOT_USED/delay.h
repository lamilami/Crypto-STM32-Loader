#ifndef __DELAY__
#define __DELAY__

/* Append new tick handler to system timer */
int Add_TickHandler(void (*tick_handler)(void));

/* Waiting for event during specified time
 *  when ms=0 - wait infinite
 *  return time elapsed if event occurs
 *  return -1 if timeout 
 */
int WaitForSingleEvent(int ms, int (*EventCheck)(void *arg), void *arg);

/* ms and us delays */
void mdelay(volatile int ms);
void udelay(volatile int us);

/* Scheduler initialization */
void Scheduler_Initialize();

/* Add task to scheduler,
 *  return value - task's id or zero if no space
 */
int AddScheduledTask(int (*task)(void), int ms);

/* Remove task from scheduler by it's id */
int RemoveScheduledTask(int task_id);

#endif