/*
Author:libing.
Date:2010-01-14
*/
//#include <linux/bitmap.h>
#include <errno.h>

#include <assert.h>
#include "mulevent.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>


static inline  unsigned char  test_bit(unsigned char bit,unsigned char * num)
{
	if( (((*num)>>bit)&0x01) ==1){
		return 1;
	}
	else{
		return 0;
	}
}

static inline unsigned char clear_bit(unsigned char bit,unsigned char  * num)
{
	unsigned char bit_value[]={1,2,4,8,16,32,64,128};
	return *num = (*num)&(~bit_value[bit]);
}

static inline unsigned char set_bit(unsigned char bit,unsigned char  * num)
{
         unsigned char  bit_value[]={1,2,4,8,16,32,64,128};
	 return   (*num) = (*num)|bit_value[bit];
}

static inline unsigned char  find_first_bit(unsigned char  * num,unsigned char max)
{
	 unsigned char i = 0 ;
	 for(i=0;i<sizeof(unsigned char) * 8 && i < max;i++){
	 	if(test_bit(i, num)){
	 		break;
	 	}
	 }
	 return i;
}
void mulevent_init(pmulevent_t pmulevent)
{
	assert(pmulevent); 
	pmulevent->sig = 0;
	sem_init(&pmulevent->sem,0,0);
	pthread_mutex_init(&pmulevent->lock,NULL);
}

int mulevent_set_event(pmulevent_t pmulevent,unsigned char nr)
{ 
	assert(nr >=0 && nr < 8);
	pthread_mutex_lock(&pmulevent->lock); 
	if(test_bit(nr,&pmulevent->sig)){
		pthread_mutex_unlock(&pmulevent->lock);
		return -1;
	}
	set_bit(nr,&pmulevent->sig);
	sem_post(&pmulevent->sem);
	pthread_mutex_unlock(&pmulevent->lock);

	return 0;
}

void mulevent_clear_event(pmulevent_t pmulevent,unsigned char nr)
{ 
	int ret = 0;
	assert(nr >=0 && nr < 8);
	pthread_mutex_lock(&pmulevent->lock);
	if(test_bit(nr,&pmulevent->sig)){
		 clear_bit(nr,&pmulevent->sig);
		 ret =sem_trywait(&pmulevent->sem);
	}
	pthread_mutex_unlock(&pmulevent->lock);
}

unsigned int mulevent_wait(pmulevent_t pmulevent)
{
	unsigned int nr=0;
	sem_wait(&pmulevent->sem);
	pthread_mutex_lock(&pmulevent->lock);
	nr = find_first_bit(&pmulevent->sig,8);
	assert(nr< 8);
	if(nr< 8){
		clear_bit(nr,&pmulevent->sig);
	}
	pthread_mutex_unlock(&pmulevent->lock);
	return nr;
}

long mulevent_wait_timeout(pmulevent_t pmulevent,const struct timespec *abs_timeout)
{
	long nr=0;
	while( sem_timedwait(&pmulevent->sem,abs_timeout)){	
		if( errno==ETIMEDOUT) return ETIMEDOUT;
	}
	pthread_mutex_lock(&pmulevent->lock);
	nr = find_first_bit(&pmulevent->sig,8);
	assert(nr< 8);
	if(nr< 8){
		clear_bit(nr,&pmulevent->sig);
	}
	pthread_mutex_unlock(&pmulevent->lock);
	return nr;
}

void mulevent_destroy(pmulevent_t pmulevent)
{
	sem_destroy(&pmulevent->sem);
}
