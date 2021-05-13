#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "slab.h"

int nextPowerOf2(int n)
{ //return larger 2^x
    int count = 0;
    if (n < 8)
        return 8; //if n is smaller than 8
    else if (n && !(n & (n - 1)))
        return n;
    while (n != 0)
    {
        n >>= 1;
        count += 1;
    }
    return 1 << count;
}
int powerOf2(int n)
{
    int result = 1;
    for (int i = 1; i <= n + 3; i++)
    {
        result = result * 2;
    }
    return result;
}

struct
{
    struct spinlock lock;
    struct slab slab[NSLAB]; //each slab's array
} stable;                    //slab table

void slabinit()
{
    /* fill in the blank */
    //make initial 9 slabs (8~2048)
    acquire(&stable.lock);
    for (int i = 0; i < NSLAB; i++)
    {
        stable.slab[i].bitmap = kalloc();                                 // slab cache's bitmap
        memset(stable.slab[i].bitmap, 0, PGSIZE);                         // fill bitmap with 0
        stable.slab[i].page[0] = kalloc();                                //at least one page should exist
        stable.slab[i].size = powerOf2(i);                                //2^3 ~ 2^11
        stable.slab[i].num_pages = 1;                                     //init, there should be at least one page
        stable.slab[i].num_objects_per_page = 4096 / stable.slab[i].size; //need for add more page
        stable.slab[i].num_free_objects = 4096 / stable.slab[i].size;     //first there is no usde_object
        stable.slab[i].num_used_objects = 0;
    }
    release(&stable.lock);
    //일단 초기화에서는 다 할당만 해줌
}

char *kmalloc(int size)
{ //size만큼 할당해줌 0< size <= 2048
    /* fill in the blank */
    //plus if current page is full, allocate new page!
    int slab_size = -1;
    int idx = -1;                   //slab_index
    slab_size = nextPowerOf2(size); //들어온 사이즈를 어디 슬랩에 넣을지 판단하기 위해
    char *addr;                     //address of kmalloc
    int p_idx = 0;                  //page_address
    int o_idx = 0;                  //object_address

    acquire(&stable.lock);
    //search which slab
    for (int i = 0; i < NSLAB; i++)
    {
        if (slab_size == stable.slab[i].size)
            idx = i;
    }

    if (stable.slab[idx].num_free_objects == 0)
    { //is page is full
        int curr_page = stable.slab[idx].num_pages;

        stable.slab[idx].page[curr_page] = kalloc(); // 다 찼으니 다음 페이지 할당
        stable.slab[idx].num_pages += 1;             // page num +1
        if (stable.slab[idx].num_pages > MAX_PAGES_PER_SLAB)
        { //if page is over 100, can't handle
            cprintf("overflow!\n");
            return 0;
        }
        stable.slab[idx].num_free_objects = stable.slab[idx].num_objects_per_page; //same as inital
    }

    for (int i = 0; i <= stable.slab[idx].num_objects_per_page * stable.slab[idx].num_pages - 1; i++)
    { //search matching page & object
        if ((*(stable.slab[idx].bitmap + (i / 8)) & (1 << (i % 8))) == 0)
        {                                                      //bitmap check
            p_idx = i / stable.slab[idx].num_objects_per_page; //for return address...
            o_idx = i % stable.slab[idx].num_objects_per_page;

            *(stable.slab[idx].bitmap + (i / 8)) = *(stable.slab[idx].bitmap + (i / 8)) | (1 << (i % 8)); //check 0->1

            stable.slab[idx].num_free_objects -= 1;
            stable.slab[idx].num_used_objects += 1;
            break;
        }
    }
    //calculate return address
    addr = stable.slab[idx].page[p_idx] + (o_idx * stable.slab[idx].size);

    release(&stable.lock);

    return addr;
}

void kmfree(char *addr, int size)
{
    /* fill in the blank */
    //slab단위로 free!
    acquire(&stable.lock);

    int slab_size = -1;
    int idx = -1;
    slab_size = nextPowerOf2(size);
    for (int i = 0; i < NSLAB; i++)
    {
        if (slab_size == stable.slab[i].size)
        idx = i;
    }

    //search target address
    for (int p_idx = 0; p_idx < stable.slab[idx].num_pages; p_idx++)
    { //
        for (int o_idx = 0; o_idx < stable.slab[idx].num_objects_per_page; o_idx++)
        {
            if ((stable.slab[idx].page[p_idx] + (o_idx * stable.slab[idx].size)) == addr)
            {
                int target_idx = p_idx * stable.slab[idx].num_objects_per_page + o_idx; //몇번째인지 찾았다!

                int mask = ~(1 << (idx % 8)); //mask on!

                *(stable.slab[idx].bitmap + (target_idx / 8)) = *(stable.slab[idx].bitmap + (target_idx / 8)) & mask;

                stable.slab[idx].num_free_objects += 1;
                stable.slab[idx].num_used_objects -= 1;

                memset(addr, 1, stable.slab[idx].size); //like kfree chagne all bit -> 1

                release(&stable.lock);
                return;
            }
        }
    }
    release(&stable.lock); //if addr is wrong...just release
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