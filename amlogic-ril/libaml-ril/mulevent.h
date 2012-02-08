/*实现多事件类型,可以同时等待多个事件
Author:libing.
Date:2010-01-14
*/
#ifndef _MULEVENT_H
#define _MULEVENT_H

#include<semaphore.h>
#include<pthread.h> 


typedef struct _MULEVENT{
 unsigned char sig;
 pthread_mutex_t   lock;
 sem_t sem;
}mulevent_t,*pmulevent_t;
void mulevent_init(pmulevent_t pmulevent);
int mulevent_set_event(pmulevent_t pmulevent,unsigned char  nr);
void mulevent_clear_event(pmulevent_t pmulevent,unsigned char nr);
unsigned int mulevent_wait(pmulevent_t pmulevent);
long mulevent_wait_timeout(pmulevent_t pmulevent,const struct timespec *abs_timeout);
void mulevent_destroy(pmulevent_t pmulevent);
#endif
