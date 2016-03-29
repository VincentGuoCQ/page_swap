#ifndef PAGE_H
#define PAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

typedef char bool;

#define TRUE	1
#define FALSE	0

struct page {
	bool pos_bit:1;
	bool access_bit:1;
	bool mapped_bit:1;
	struct list list;
	pthread_mutex_t mutex;
};
struct page_slot {
	bool mapped_bit:1;
	bool swaped_bit:1;
	unsigned int page;
};

#endif	//page_H
