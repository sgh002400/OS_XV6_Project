#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "slab.h"

typedef int bool;
#define true 1
#define false 0

struct {
	struct spinlock lock;
	struct slab slab[NSLAB];
} stable;


int nextPowerOf2 (int n) {

	unsigned count = 0;

	if (n && ! (n & (n - 1)))
		return n;

	while ( n != 0) {
		n >>= 1;
		count += 1;
	}

	return 1 << count;
}

void slabinit(){
	
	for(int i = 0; i < 9; i++) { 
		stable.slab[i].size = 8 << i;
		stable.slab[i].num_pages = 1;
		stable.slab[i].num_free_objects = PGSIZE >> (i+3);
		stable.slab[i].num_used_objects = 0;
		stable.slab[i].num_objects_per_page = PGSIZE >> (i+3);
		stable.slab[i].bitmap = kalloc(); 
		memset(stable.slab[i].bitmap, 0, PGSIZE);
		
		stable.slab[i].page[0] = kalloc();
		memset(stable.slab[i].page[0], 0, PGSIZE);
	}
}

char *kmalloc(int size){

	struct slab *s;

	if(size > 0 && size <= 2048) {

		int np2 = nextPowerOf2(size);
		int byteaddr = 0;

		for(s=stable.slab; s<&stable.slab[NSLAB]; s++) {

			if(s->size == np2) {
				
				if(s->num_free_objects == 0) {				
					s->page[s->num_pages] = kalloc();
					memset(s->page[s->num_pages], 0, PGSIZE);
					s->num_pages++;
					s->num_free_objects += s->num_objects_per_page;
				}

				s->bitmap[byteaddr>>3] |= 1<<(byteaddr%8);
				s->num_used_objects++;
				s->num_free_objects--;

				break;
			}
		}
		
		byteaddr++;
		
		return &s->page[s->num_pages-1][s->num_objects_per_page - (s->num_free_objects+1)];
	}	

	return 0;
}

void kmfree(char *addr, int size){

	struct slab *s;

	if(size > 0 && size <= 2048) {

		int np2 = nextPowerOf2(size);
		int byteaddr = 0;

		for(s=stable.slab; s<&stable.slab[NSLAB]; s++){

			int i = 0;
			if(np2 == s->size) {

				if(*addr == s->page[i][s->num_objects_per_page - (s->num_free_objects+1)]) {
					
					s->bitmap[byteaddr>>3] |= 0<<(byteaddr%8);

					s->num_used_objects--;
					s->num_free_objects++;

					break;
				}
			}
			i++;
		}
	}
}

void slabdump(){
	cprintf("__slabdump__\n");

	struct slab *s;

	cprintf("size\tnum_pages\tused_objects\tfree_objects\n");

	for(s = stable.slab; s < &stable.slab[NSLAB]; s++){
		cprintf("%d\t%d\t\t%d\t\t%d\n", 
			s->size, s->num_pages, s->num_used_objects, s->num_free_objects);
	}
}
