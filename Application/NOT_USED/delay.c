#include "stm32f10x_it.h"
#include "delay.h"

#define MAX_TICK_HANDLERS 10
#define MAX_SCHEDULED_TASKS 20

/* Tick handlers list */
void (*tick_handlers[MAX_TICK_HANDLERS])(void);
int tick_handlers_count = 0;

/* Append new tick handler to system timer */
int Add_TickHandler(void (*tick_handler)(void))
{
  if(tick_handlers_count == MAX_TICK_HANDLERS)
    return -1;
  tick_handlers[tick_handlers_count] = tick_handler;
  tick_handlers_count++;
  return 0;
}

/* Counter for ms delay */
volatile int delay_counter = 0;

/* Handler of system timer interrupt,
 * executes all additional handlers in tick handlers list
 */ 
void SysTick_Handler(void)
{
  int i;
  /* Call all tick handlers in list */
  for(i = 0; i < tick_handlers_count; i++)
    tick_handlers[i]();
    
  /* ms delay processing if present */  
  if(delay_counter)
    delay_counter--;
}

/* Waiting for event during specified time
 *  when ms=0 - wait infinite
 *  return time elapsed if event occurs
 *  return -1 if timeout 
 */
int WaitForSingleEvent(int ms, int (*EventCheck)(void *arg), void *arg)
{
  delay_counter = ms;
  if(ms)
    while(delay_counter && !EventCheck(arg));
  else
    while(!EventCheck(arg));
  
  if(delay_counter)
    return delay_counter;
  else
    return -1;
}

/* ms and us delays */
void mdelay(volatile int ms)
{
  delay_counter = ms;
  while(delay_counter);
}

void udelay(volatile int us)
{
  __disable_irq();
  us <<= 1;
  while(us--)
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
  __enable_irq();
}

/* Scheduled task */
struct ScheduledTask
{
  /* Task to execute after delay. 
   * Task must return 0, if it don't want to reschedule 
   */
  int (*task)(void);
  /* Delay of task execution */
  int delay;
  /* Timer for task */
  int ms;
};

struct ScheduledTask tasks[MAX_SCHEDULED_TASKS];

/* Scheduler tick handler */
void ScheduleTickHandler(void)
{
  int i;
  
  /* Process all defined tasks */
  for(i = 0; i < MAX_SCHEDULED_TASKS; i++)
  {
    if(tasks[i].delay)
    {
      /* If time is not out then wait */
      if(tasks[i].ms > 0)
        tasks[i].ms--;
      /* Otherwise execute task and if it wants reschedule it */
      else
      {
        if(tasks[i].task())
          tasks[i].ms = tasks[i].delay;
        else
          tasks[i].delay = 0;
      }
    }
  }
}

/* Scheduler initialization */
void Scheduler_Initialize()
{
  int i;
  
  /* Free all tasks */
  for(i = 0; i < MAX_SCHEDULED_TASKS; i++)
    tasks[i].delay = 0;
 
  /* Append scheduler tick handler */
  Add_TickHandler(&ScheduleTickHandler);
}

/* Add task to scheduler,
 *  return value - task's id or zero if no space
 */
int AddScheduledTask(int (*task)(void), int ms)
{
  /* Find free space for task and insert here */
  int i;
  for(i = 0; i < MAX_SCHEDULED_TASKS; i++)
  {
    if(!tasks[i].delay)
    {
      __disable_irq();
      tasks[i].task = task;
      tasks[i].delay = ms;
      tasks[i].ms = ms;
      __enable_irq();
      return (i + 1);
    }
  }
  return 0;
}

/* Remove task from scheduler by it's id */
int RemoveScheduledTask(int task_id)
{
  if(task_id <= 0 && task_id > MAX_SCHEDULED_TASKS)
    return 0;
  if(!tasks[task_id - 1].delay)
    return 0;
  tasks[task_id - 1].delay = 0;
  return task_id;
}
