#include "page.h"
#include "list.h"

#define PAGE_NUM (1UL << 7)
#define PAGE_SLOT_NUM (1UL << 4)
#define VISIT_TIME (200)
#define MAX_SEQ_PAGE	3

struct page PageList[PAGE_NUM];
struct page_slot PageSlotList[PAGE_SLOT_NUM]; 

struct list swapable_list, frequse_list; 
unsigned short VisitList[VISIT_TIME];

//atomic_t PageUseCount = 0;
pthread_t VisitThreadID = 0;

void GenerateVisit() {
	int nPageCount = 0, nIndex = 0;
	int nPage = 0;
	srand(time(NULL));
	for(nPageCount = 0; nPageCount < VISIT_TIME;) {
		VisitList[nPageCount] = rand()%(PAGE_SLOT_NUM/2);
		nPage = 2+rand()%MAX_SEQ_PAGE;
		for(nIndex=0; nIndex < nPage && nPageCount < VISIT_TIME; nIndex++) {
			VisitList[nPageCount+1] = VisitList[nPageCount]+1;
			nPageCount++;
		}
		if(nPageCount >= VISIT_TIME) {
			break;
		}
		VisitList[nPageCount] = rand()%(PAGE_SLOT_NUM/2);
		nPageCount++;
		if(nPageCount >= VISIT_TIME) {
			break;
		}
		VisitList[nPageCount] = rand()%(PAGE_SLOT_NUM/2);
		nPageCount++;
		if(nPageCount >= VISIT_TIME) {
			break;
		}

		VisitList[nPageCount] = PAGE_SLOT_NUM/2-4+rand()%(PAGE_SLOT_NUM/2);
		for(nIndex=0; nIndex < (6-nPage) && nPageCount < VISIT_TIME; nIndex++) {
			VisitList[nPageCount+1] = VisitList[nPageCount]+1;
			nPageCount++;
		}
		if(nPageCount >= VISIT_TIME) {
			break;
		}
		VisitList[nPageCount] = PAGE_SLOT_NUM/2+rand()%(PAGE_SLOT_NUM/2);
		nPageCount++;
		if(nPageCount >= VISIT_TIME) {
			break;
		}
		VisitList[nPageCount] = PAGE_SLOT_NUM/2+rand()%(PAGE_SLOT_NUM/2);
		nPageCount++;
		if(nPageCount >= VISIT_TIME) {
			break;
		}
	}
//	for(nIndex=0; nIndex<VISIT_TIME; nIndex++) {
//		printf("%d\t", VisitList[nIndex]);
//	}
}

void* Visit_thread(void * para) {
	int pageID = 0, nIndex = 0, i = 0;
	srand(time(NULL));
	for(nIndex = 0; nIndex < VISIT_TIME; nIndex++) {
		int PageVisit = VisitList[nIndex];
		//slot not mapped
		if(FALSE == PageSlotList[PageVisit].mapped_bit){
			for(i=0; i<PAGE_NUM; i++) {
				if (FALSE == PageList[i].mapped_bit)
				  break;
			}
			if(i <= PAGE_NUM) {
				pthread_mutex_lock(&PageList[i].mutex);
				list_append(&frequse_list, &PageList[i].list);
				PageList[i].pos_bit = TRUE;
				PageSlotList[PageVisit].page = i;
				PageSlotList[PageVisit].mapped_bit = TRUE;
				PageList[i].mapped_bit = TRUE;
				PageList[i].access_bit = TRUE;
				pthread_mutex_unlock(&PageList[i].mutex);
			}
		}
		else {
			if(FALSE == PageSlotList[PageVisit].swaped_bit){
				if(FALSE == PageList[PageSlotList[PageVisit].page].access_bit) {
					PageList[PageSlotList[PageVisit].page].access_bit = TRUE;
				}
				else {
					if(FALSE == PageList[PageSlotList[PageVisit].page].pos_bit) {
						pthread_mutex_lock(&PageList[PageSlotList[PageVisit].page].mutex);
						list_remove(&PageList[PageSlotList[PageVisit].page].list);
						list_append(&swapable_list, &PageList[PageSlotList[PageVisit].page].list);
						PageList[PageSlotList[PageVisit].page].pos_bit = FALSE;
						pthread_mutex_unlock(&PageList[PageSlotList[PageVisit].page].mutex);
					}
				}
			} else {

			}
		}
		printf("%d:%d\t", PageVisit, PageSlotList[PageVisit].page);
		if(nIndex%10 == 0)
		  printf("\n");
	}
	return;
}
int main() {
	int nIndex = 0;
	struct list *pos, *n;
	//init list head
	list_init(&swapable_list);
	list_init(&frequse_list);
	//init pagelist
	for(nIndex = 0; nIndex < PAGE_NUM; nIndex++) {
		PageList[nIndex].pos_bit = FALSE;
		PageList[nIndex].access_bit = FALSE;
		PageList[nIndex].mapped_bit = FALSE;
		pthread_mutex_init(&PageList[nIndex].mutex, NULL);
		list_init(&PageList[nIndex].list);
	}
	//init pageslotlist
	for(nIndex = 0; nIndex < PAGE_SLOT_NUM; nIndex++) {
		PageSlotList[nIndex].swaped_bit = FALSE;
		PageSlotList[nIndex].mapped_bit = FALSE;
		PageSlotList[nIndex].page = 0;
	}
	//init visit list
	GenerateVisit();
	//create thread
	if(pthread_create(&VisitThreadID, NULL, Visit_thread, NULL)) {
		printf("create visit thread error\n");
	}
	pthread_join(VisitThreadID, NULL);
	printf("\n");
	list_for_each_safe(pos, n, &frequse_list) {
		struct page * page = list_entry(pos, struct page, list);
		printf("%d\t",(page-PageList));
	}
	return 0;
}
