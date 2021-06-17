#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "slab.h"

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

struct
{
    struct spinlock lock;
    struct slab slab[NSLAB]; 
} stable;              

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

char *kmalloc(int size)
{ 
    int slab_size = -1;
    int idx = -1;                
    slab_size = nextPowerOf2(size); 
    char *addr;                    
              
    acquire(&stable.lock);

    for (int i = 0; i < NSLAB; i++)
    {
        if (slab_size == stable.slab[i].size)
            idx = i;
    }

    if (stable.slab[idx].num_free_objects == 0)
    {
        int curr_page = stable.slab[idx].num_pages;

        stable.slab[idx].page[curr_page] = kalloc();
        stable.slab[idx].num_pages += 1;
        if (stable.slab[idx].num_pages > MAX_PAGES_PER_SLAB)
        {
            cprintf("overflow!\n");
            return 0;
        }
        stable.slab[idx].num_free_objects = stable.slab[idx].num_objects_per_page; 
    }

    int pageidx = 0;              
    int objectidx = 0;   

    for (int i = 0; i <= stable.slab[idx].num_objects_per_page * stable.slab[idx].num_pages - 1; i++)
    { 
        if ((*(stable.slab[idx].bitmap + (i / 8)) & (1 << (i % 8))) == 0)
        {                                                      
            pageidx = i / stable.slab[idx].num_objects_per_page; 
            objectidx = i % stable.slab[idx].num_objects_per_page;

            *(stable.slab[idx].bitmap + (i / 8)) = *(stable.slab[idx].bitmap + (i / 8)) | (1 << (i % 8));
            stable.slab[idx].num_free_objects -= 1;
            stable.slab[idx].num_used_objects += 1;
            break;
        }
    }

    addr = stable.slab[idx].page[pageidx] + (objectidx * stable.slab[idx].size);

    release(&stable.lock);

    return addr;
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


//print slab table
void slabdump()
{
    cprintf("__slabdump__\n");

    struct slab *s;

    cprintf("size\tnum_pages\tused_objects\tfree_objects\n");

    for (s = stable.slab; s < &stable.slab[NSLAB]; s++)
    {
        cprintf("%d\t%d\t\t%d\t\t%d\n",
        s->size, s->num_pages, s->num_used_objects, s->num_free_objects);
    }
}
